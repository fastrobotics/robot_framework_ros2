import sys
import os
import yaml
import subprocess
import re
import shutil
import tempfile
import queue
from concurrent.futures import ProcessPoolExecutor
from contextlib import redirect_stderr, redirect_stdout
from datetime import datetime
from multiprocessing import Manager
from analyzer_plugins import ANALYZER_PLUGINS


# Toggle each analyzer plugin independently. Unlisted plugins are disabled.
ANALYZER_PLUGIN_CONFIG = {
    "RosoutTextAnalyzer": True,
    "TimestampRateAnalyzer": True,
}

# Number of bag folders to analyze at the same time.
MAX_PARALLEL_BAGS = 8

# Edit this set to choose which topic names get timestamp CSVs and rate analysis.
TIMESTAMP_ANALYSIS_TOPICS = {
    "/robot/front_cam/depth_registered/points",
}

ANALYZER_PLUGIN_OPTIONS = {
    "TimestampRateAnalyzer": {"topics": TIMESTAMP_ANALYSIS_TOPICS},
}


def reindex_bag(bag_path: str, metadata_file: str) -> bool:
    """Reindex a bag, temporarily skipping empty or unreadable MCAP files."""
    bag_realpath = os.path.realpath(bag_path)
    quarantine_dir = None
    quarantined_files = []
    success = False
    restore_ok = True

    def quarantine_file(file_path: str) -> bool:
        nonlocal quarantine_dir

        file_realpath = os.path.realpath(file_path)
        try:
            if os.path.commonpath([bag_realpath, file_realpath]) != bag_realpath:
                print(f"Error: refusing to move file outside bag directory: {file_realpath}")
                return False
        except ValueError:
            print(f"Error: cannot verify file path: {file_realpath}")
            return False

        if not os.path.isfile(file_realpath):
            print(f"Error: MCAP file does not exist: {file_realpath}")
            return False

        if quarantine_dir is None:
            quarantine_dir = tempfile.mkdtemp(
                prefix=".rosbag_reindex_quarantine_",
                dir=os.path.dirname(bag_realpath),
            )

        relative_path = os.path.relpath(file_realpath, bag_realpath)
        quarantined_path = os.path.join(quarantine_dir, relative_path)
        os.makedirs(os.path.dirname(quarantined_path), exist_ok=True)
        shutil.move(file_realpath, quarantined_path)
        quarantined_files.append((file_realpath, quarantined_path))
        print(f"Warning: temporarily skipping unreadable MCAP file: {file_realpath}")
        return True

    try:
        # Avoid asking ros2 bag reindex to open empty split files.
        for entry in os.scandir(bag_realpath):
            if (
                entry.is_file()
                and entry.name.lower().endswith(".mcap")
                and entry.stat().st_size == 0
            ):
                print(f"Warning: found zero-byte MCAP file: {entry.path}")
                if not quarantine_file(entry.path):
                    return False

        while True:
            try:
                result = subprocess.run(
                    ["ros2", "bag", "reindex", "-s", "mcap", bag_path],
                    capture_output=True,
                    text=True,
                    check=False,
                    timeout=300,
                )
            except FileNotFoundError:
                print("Error: 'ros2' command not found. Source your ROS 2 environment and try again.")
                break
            except subprocess.TimeoutExpired:
                print("Error: ros2 bag reindex timed out after 300 seconds.")
                break

            output = (result.stdout or "") + "\n" + (result.stderr or "")
            match = re.search(
                r"Could not open ['\"]([^'\"]+\.mcap)['\"].*?file too small",
                output,
                re.IGNORECASE | re.DOTALL,
            )

            if match:
                if not quarantine_file(match.group(1)):
                    break
                continue

            if result.returncode != 0:
                print(f"Error: ros2 bag reindex failed with exit code {result.returncode}")
                if output.strip():
                    print(output.strip())
                break

            success = os.path.exists(metadata_file)
            if not success:
                print(f"Error: reindex did not create metadata.yaml in '{bag_path}'")
            break

    finally:
        for original_path, quarantined_path in reversed(quarantined_files):
            try:
                os.makedirs(os.path.dirname(original_path), exist_ok=True)
                shutil.move(quarantined_path, original_path)
            except OSError as error:
                restore_ok = False
                print(f"Error restoring {original_path}: {error}")

        if quarantine_dir and os.path.isdir(quarantine_dir):
            if restore_ok:
                shutil.rmtree(quarantine_dir, ignore_errors=True)
            else:
                print(f"Warning: quarantined files remain in: {quarantine_dir}")

    return success and restore_ok


def run_analyzer_plugins(
    bag_path: str,
    storage_id: str,
    topics_and_types: list,
    plugins: list,
) -> None:
    """Dispatch each selected bag message to its matching analyzer plugins."""
    try:
        import rosbag2_py
        from rclpy.serialization import deserialize_message
        from rosidl_runtime_py.utilities import get_message
    except ImportError as error:
        print(f"Bag analyzer plugins unavailable: ROS 2 Python modules not found: {error}")
        for plugin in plugins:
            plugin.close()
        return

    message_classes = {}
    topic_plugins = {}
    for topic_info in topics_and_types:
        metadata = topic_info.get("topic_metadata", {})
        topic_name = metadata.get("name")
        message_type = metadata.get("type")
        if not topic_name:
            continue
        matching_plugins = [
            plugin for plugin in plugins
            if plugin.matches(topic_name, message_type)
        ]
        if not matching_plugins:
            continue
        try:
            message_classes[topic_name] = (message_type, get_message(message_type))
            for plugin in matching_plugins:
                plugin.configure_topic(topic_name, message_type)
            if matching_plugins:
                topic_plugins[topic_name] = matching_plugins
        except Exception as error:
            print(f"Skipping analysis for {topic_name} ({message_type}): {error}")

    if not message_classes:
        for plugin in plugins:
            plugin.close()
        return

    selected_topics = set(message_classes)
    expected_messages = sum(
        int(topic_info.get("message_count", 0) or 0)
        for topic_info in topics_and_types
        if topic_info.get("topic_metadata", {}).get("name") in selected_topics
    )
    progress_interval = max(expected_messages // 100, 1) if expected_messages else 10_000
    processed_messages = 0
    failed_plugins = set()
    scan_complete = False
    try:
        reader = rosbag2_py.SequentialReader()
        reader.open(
            rosbag2_py.StorageOptions(uri=bag_path, storage_id=storage_id),
            rosbag2_py.ConverterOptions("", ""),
        )
        reader.set_filter(rosbag2_py.StorageFilter(topics=list(selected_topics)))

        if expected_messages:
            print("Plugin analysis:   0.0%", end="\r", flush=True)
        else:
            print("Plugin analysis: percentage unavailable", end="\r", flush=True)
        while reader.has_next():
            topic_name, serialized_data, _bag_timestamp = reader.read_next()
            processed_messages += 1
            if expected_messages and processed_messages % progress_interval == 0:
                percentage = min(processed_messages / expected_messages * 100, 99.0)
                suffix = (
                    " (metadata total reached; scanning remaining messages)"
                    if processed_messages >= expected_messages
                    else ""
                )
                print(
                    f"Plugin analysis: {percentage:5.1f}%{suffix}",
                    end="\r",
                    flush=True,
                )

            if topic_name not in message_classes:
                continue

            message_type, message_class = message_classes[topic_name]
            message = deserialize_message(serialized_data, message_class)
            for plugin in topic_plugins.get(topic_name, []):
                if plugin in failed_plugins:
                    continue
                try:
                    plugin.process(topic_name, message)
                except Exception as error:
                    failed_plugins.add(plugin)
                    print(
                        f"Analyzer plugin {type(plugin).__name__} failed "
                        f"for '{topic_name}': {error}"
                    )
        scan_complete = True

    except Exception as error:
        print()
        print(f"Bag analyzer plugin scan failed for '{bag_path}': {error}")
    finally:
        for plugin in plugins:
            plugin.close()

    if scan_complete:
        print("Plugin analysis complete: 100.0%")


def generate_metadata_summary(bag_path: str):
    analysisStart = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    bag_path = bag_path.rstrip("/")
    metadata_file = os.path.join(bag_path, "metadata.yaml")
    
    if not os.path.exists(metadata_file):
        print(f"metadata.yaml not found in '{bag_path}'. Attempting MCAP reindex...")
        if not reindex_bag(bag_path, metadata_file):
            return

    print(f"Reading metadata from: {metadata_file}")
    with open(metadata_file, 'r') as f:
        try:
            metadata_data = yaml.safe_load(f)
        except yaml.YAMLError as e:
            print(f"Error parsing YAML file '{metadata_file}': {e}")
            return

    if not isinstance(metadata_data, dict):
        print(f"Error: metadata file is empty or invalid: {metadata_file}")
        return

    bag_info = metadata_data.get('rosbag2_bagfile_information')
    if not isinstance(bag_info, dict):
        print(f"Error: missing or invalid 'rosbag2_bagfile_information' in: {metadata_file}")
        return

    storage_identifier = bag_info.get('storage_identifier', 'Unknown')
    duration_nanos = bag_info.get('duration', {}).get('nanoseconds', 0)
    duration_secs = duration_nanos / 1e9
    total_messages = bag_info.get('message_count', 0)
    
    starting_time = bag_info.get('starting_time', {})
    if isinstance(starting_time, dict):
        starting_time_nanos = starting_time.get(
            'nanoseconds_since_epoch',
            starting_time.get('nanoseconds'),
        )
    else:
        starting_time_nanos = None

    try:
        if starting_time_nanos is None:
            start_time_str = "Unknown"
        else:
            start_time_str = datetime.fromtimestamp(
                starting_time_nanos / 1e9
            ).strftime('%Y-%m-%d %H:%M:%S')
    except (TypeError, ValueError, OSError):
        start_time_str = "Unknown"

    topics_and_types = bag_info.get('topics_with_message_count', [])
    output_md_path = os.path.join(bag_path, "rosbag_metadata_summary.md")
    plugins = []
    for plugin_type in ANALYZER_PLUGINS:
        if not ANALYZER_PLUGIN_CONFIG.get(plugin_type.__name__, False):
            continue
        plugin_options = dict(ANALYZER_PLUGIN_OPTIONS.get(plugin_type.__name__) or {})
        if plugin_type.__name__ == "TimestampRateAnalyzer":
            plugin_options.update({
                "log_start_time_seconds": (
                    starting_time_nanos / 1e9
                    if starting_time_nanos is not None
                    else None
                ),
                "log_duration_seconds": duration_secs,
            })
        plugins.append(plugin_type(bag_path, plugin_options))
    run_analyzer_plugins(
        bag_path,
        storage_identifier,
        topics_and_types,
        plugins,
    )
    plugin_reports = []
    for plugin in plugins:
        report = plugin.report_markdown()
        if report.strip():
            plugin_reports.append((plugin, report.strip()))

    systemMap = {
        "Infrastructure": {
            "robot_framework_ros2/msg/Heartbeat": {},
            "robot_framework_ros2/msg/ReadyToArm": {}
        },
        "Diagnostics": {
            "robot_framework_ros2/msg/Diagnostic": {}
        },
        "Pose": {
            "sensor_msgs/msg/Imu": {},
            "geometry_msgs/msg/AccelStamped": {},
            "sensor_msgs/msg/MagneticField": {},
            "geometry_msgs/msg/AccelWithCovarianceStamped": {},
            "nav_msgs/msg/Odometry": {},
            "tf2_msgs/msg/TFMessage": {},
        },
        "Perception": {
            "sensor_msgs/msg/CameraInfo": {},
            "sensor_msgs/msg/Image": {},
            "sensor_msgs/msg/PointCloud2": {},
        },
        "ROS2": {
            "rosbag2_interfaces/msg/WriteSplitEvent": {},
            "rcl_interfaces/msg/Log": {},
            "rcl_interfaces/msg/ParameterEvent": {}
        }
    }
    for topic in topics_and_types:
        t_info = topic.get('topic_metadata', {})
        t_name = t_info.get('name', 'Unknown')
        t_type = t_info.get('type', 'Unknown')
        t_count = topic.get('message_count', 0)
        t_format = t_info.get('serialization_format', 'Unknown')
        
            
        found = False
        for container, subsections in systemMap.items():
            if t_type in subsections:
                subsections[t_type][t_name] = {
                    "count": t_count,
                    "serialization": t_format
                }
                found = True
                break
        if found == False:
            print("Topic: " + t_name + " Type: " + " Uncategorized!")
        
    with open(output_md_path, 'w') as md:
        md.write(f"# ROS2 Bag Metadata Summary\n\n")
        md.write(f"📅 **Analysis Generated:** {analysisStart}  \n")
        md.write(f"📁 **Bag Path:** `{bag_path}`  \n")
        md.write(f"⚙️ **Storage Plugin:** `{storage_identifier}`  \n")
        md.write(f"📅 **Log Start Time:** {start_time_str}  \n")
        md.write(f"⏳ **Duration:** {duration_secs:.2f} seconds  \n")
        md.write(f"🔢 **Total Messages:** {total_messages:,}  \n\n")
        
        md.write("---\n\n") 
        md.write("## 🗂️ Table of Contents\n\n")
        if plugin_reports:
            md.write("* [Analyzer Reports](#analyzer-reports)\n")
            for plugin, _report in plugin_reports:
                toc_entry = plugin.report_toc_entry()
                if toc_entry:
                    md.write(f"  * [{toc_entry[0]}](#{toc_entry[1]})\n")

        md.write("* [Message Statistics](#message-statistics)\n")
        for container_name, subsections in systemMap.items():
            has_data = any(subsections.values())
            if has_data:
                clean_anchor = f"system-{container_name.lower().replace(' ', '-')}"
                md.write(f"  * [{container_name}](#{clean_anchor})\n")

        md.write("\n---\n\n")
        if plugin_reports:
            md.write("## Analyzer Reports\n\n")
            for _plugin, report in plugin_reports:
                md.write(f"{report}\n\n")
            md.write("---\n\n")

        md.write("## 📈 Message Statistics\n\n")
        md.write("This section breaks down recorded data categories grouped by functional system modules and ROS2 datatypes.\n\n")
        for container_name, subsections in systemMap.items():
            has_data = any(subsections.values())
            if has_data:
                clean_anchor = f"system-{container_name.lower().replace(' ', '-')}"
                md.write(f'<a id="{clean_anchor}"></a>\n')
                md.write(f"### 🏢 System: {container_name}\n\n")
                for msg_type, topics in subsections.items():
                    if not topics:
                        continue
                    md.write(f"#### 🔹 Datatype: `{msg_type}`\n\n")
                    md.write("| Topic Name | Message Count | Serialization Format |\n")
                    md.write("| :--- | :---: | :---: |\n")
                    
                    for topic_name, info in topics.items():
                        md.write(f"| `{topic_name}` | {info['count']:,} | `{info['serialization']}` |\n")
                    
                    md.write("\n") 

    print(f"🎉 Regenerated Markdown report with working bookmarks at: {output_md_path}")
        
   

def find_bag_directories(root_path: str) -> list[str]:
    """Find bag directories under root_path, including root_path itself."""
    bag_directories = []

    for current_path, subdirectories, files in os.walk(root_path):
        has_metadata = "metadata.yaml" in files
        has_mcap = any(filename.lower().endswith(".mcap") for filename in files)

        if has_metadata or has_mcap:
            bag_directories.append(current_path)
            subdirectories.clear()

    return sorted(bag_directories)


_WORKER_OUTPUT_QUEUE = None


def initialize_worker_output_queue(output_queue):
    global _WORKER_OUTPUT_QUEUE
    _WORKER_OUTPUT_QUEUE = output_queue


class WorkerOutputStream:
    """Send complete output lines from a worker to the parent process."""

    def __init__(self, bag_path: str, is_error: bool):
        self.bag_path = bag_path
        self.is_error = is_error
        self.buffer = ""

    def write(self, text: str):
        self.buffer += text
        while self.buffer:
            newline_index = self.buffer.find("\n")
            carriage_index = self.buffer.find("\r")
            delimiter_indices = [index for index in (newline_index, carriage_index) if index >= 0]
            if not delimiter_indices:
                break
            delimiter_index = min(delimiter_indices)
            line = self.buffer[:delimiter_index]
            self.buffer = self.buffer[delimiter_index + 1:]
            if line:
                _WORKER_OUTPUT_QUEUE.put((self.bag_path, self.is_error, line))
        return len(text)

    def flush(self):
        if self.buffer:
            _WORKER_OUTPUT_QUEUE.put((self.bag_path, self.is_error, self.buffer))
            self.buffer = ""


def process_bag_directory(bag_path: str):
    """Analyze one bag while streaming its output back to the parent process."""
    stdout_stream = WorkerOutputStream(bag_path, False)
    stderr_stream = WorkerOutputStream(bag_path, True)
    try:
        with redirect_stdout(stdout_stream), redirect_stderr(stderr_stream):
            print(f"Starting analysis: {bag_path}")
            generate_metadata_summary(bag_path)
    except Exception as error:
        print(f"Unexpected error analyzing '{bag_path}': {error}", file=stderr_stream)
    finally:
        stdout_stream.flush()
        stderr_stream.flush()

    return bag_path


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python log_analyzer.py /path/to/bag_directory_or_parent_folder")
        sys.exit(1)

    root_path = os.path.abspath(sys.argv[1])
    if not os.path.isdir(root_path):
        print(f"Error: directory does not exist: {root_path}")
        sys.exit(1)

    bag_directories = find_bag_directories(root_path)
    if not bag_directories:
        print(f"No bag directories found under: {root_path}")
        sys.exit(1)

    print(f"Found {len(bag_directories)} bag director(y/ies) under: {root_path}")
    total = len(bag_directories)
    worker_count = max(1, min(MAX_PARALLEL_BAGS, total))
    print(f"Analyzing with {worker_count} parallel worker(s).")
    with Manager() as manager:
        output_queue = manager.Queue()
        with ProcessPoolExecutor(
            max_workers=worker_count,
            initializer=initialize_worker_output_queue,
            initargs=(output_queue,),
        ) as executor:
            pending = {
                executor.submit(process_bag_directory, bag_path): bag_path
                for bag_path in bag_directories
            }
            completed = 0
            while pending:
                try:
                    bag_path, is_error, line = output_queue.get(timeout=0.1)
                    relative_path = os.path.relpath(bag_path, root_path)
                    destination = sys.stderr if is_error else sys.stdout
                    print(f"[{relative_path}] {line}", file=destination, flush=True)
                except queue.Empty:
                    pass

                finished = [future for future in pending if future.done()]
                for future in finished:
                    bag_path = pending.pop(future)
                    try:
                        future.result()
                    except Exception as error:
                        print(
                            f"[{os.path.relpath(bag_path, root_path)}] "
                            f"Unexpected worker failure: {error}",
                            file=sys.stderr,
                            flush=True,
                        )
                    completed += 1
                    print(
                        f"Progress: {completed}/{total} done, {total - completed} left",
                        flush=True,
                    )

                while True:
                    try:
                        bag_path, is_error, line = output_queue.get_nowait()
                    except queue.Empty:
                        break
                    relative_path = os.path.relpath(bag_path, root_path)
                    destination = sys.stderr if is_error else sys.stdout
                    print(f"[{relative_path}] {line}", file=destination, flush=True)
