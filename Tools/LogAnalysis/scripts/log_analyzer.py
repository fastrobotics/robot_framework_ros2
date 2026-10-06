import sys
import os
import yaml
from datetime import datetime

def generate_metadata_summary(bag_path: str):
    analysisStart = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    bag_path = bag_path.rstrip("/")
    metadata_file = os.path.join(bag_path, "metadata.yaml")
    
    if not os.path.exists(metadata_file):
        print(f"Error: metadata.yaml not found in '{bag_path}'")
        return

    print(f"Reading metadata from: {metadata_file}")
    with open(metadata_file, 'r') as f:
        try:
            metadata_data = yaml.safe_load(f)
        except Exception as e:
            print(f"Error parsing YAML file: {e}")
            return

    bag_info = metadata_data.get('rosbag2_bagfile_information', {})
    
    storage_identifier = bag_info.get('storage_identifier', 'Unknown')
    duration_nanos = bag_info.get('duration', {}).get('nanoseconds', 0)
    duration_secs = duration_nanos / 1e9
    total_messages = bag_info.get('message_count', 0)
    
    starting_time_nanos = bag_info.get('starting_time', {}).get('nanoseconds', 0)
    try:
        start_time_str = datetime.fromtimestamp(starting_time_nanos / 1e9).strftime('%Y-%m-%d %H:%M:%S')
    except Exception:
        start_time_str = "Unknown"

    topics_and_types = bag_info.get('topics_with_message_count', [])
    output_md_path = os.path.join(bag_path, "rosbag_metadata_summary.md")

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
                # 🌟 Store the topic name and its data container inside the message type!
                subsections[t_type][t_name] = {
                    "count": t_count,
                    "serialization": t_format
                }
                found = True
                break
        if found == False:
            print("Topic: " + t_name + " Type: " + " Uncategorized!")
        
    with open(output_md_path, 'w') as md:
        # 1. 📊 Global Bag Stats Block
        md.write(f"# ROS2 Bag Metadata Summary\n\n")
        md.write(f"📅 **Analysis Generated:** {analysisStart}  \n")
        md.write(f"📁 **Bag Path:** `{bag_path}`  \n")
        md.write(f"⚙️ **Storage Plugin:** `{storage_identifier}`  \n")
        md.write(f"📅 **Log Start Time:** {start_time_str}  \n")
        md.write(f"⏳ **Duration:** {duration_secs:.2f} seconds  \n")
        md.write(f"🔢 **Total Messages:** {total_messages:,}  \n\n")
        
        md.write("---\n\n") 
        
        # 2. 📑 Table of Contents (ToC)
        md.write("## 🗂️ Table of Contents\n\n")
        # Clean bookmark link to the main wrapper section (strip emoji '📈')
        md.write("* [Message Statistics](#message-statistics)\n")
        
        # Nested system container links
        for container_name, subsections in systemMap.items():
            has_data = any(len(topics) > 0 for topics in subsections.values())
            if has_data:
                clean_anchor = f"system-{container_name.lower().replace(' ', '-')}"
                md.write(f"  * [{container_name}](#{clean_anchor})\n")

        md.write("\n---\n\n")

        # 3. 📈 High-Level Message Statistics Main Section
        md.write("## 📈 Message Statistics\n\n")
        md.write("This section breaks down recorded data categories grouped by functional system modules and ROS2 datatypes.\n\n")

        # 4. 🏗️ Sub-System Components Breakdown
        for container_name, subsections in systemMap.items():
            has_data = any(len(topics) > 0 for topics in subsections.values())
            if has_data:
                clean_anchor = f"system-{container_name.lower().replace(' ', '-')}"
                md.write(f'<a id="{clean_anchor}"></a>\n')
                md.write(f"### 🏢 System: {container_name}\n\n")

                # 5. Iterate through datatypes
                for msg_type, topics in subsections.items():
                    if not topics:
                        continue
                        
                    md.write(f"#### 🔹 Datatype: `{msg_type}`\n\n")
                    
                    # Detailed Data Table
                    md.write("| Topic Name | Message Count | Serialization Format |\n")
                    md.write("| :--- | :---: | :---: |\n")
                    
                    for topic_name, info in topics.items():
                        md.write(f"| `{topic_name}` | {info['count']:,} | `{info['serialization']}` |\n")
                    
                    md.write("\n") 

    print(f"🎉 Regenerated Markdown report with working bookmarks at: {output_md_path}")
        
   

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python log_analyzer.py /path/to/bag_directory")
        sys.exit(1)
        
    generate_metadata_summary(sys.argv[1])
