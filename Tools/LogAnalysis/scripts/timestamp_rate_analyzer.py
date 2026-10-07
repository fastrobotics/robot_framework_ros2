import csv
import hashlib
import math
import os
import re
from urllib.parse import quote

from bag_analyzer_plugin import BagAnalyzerPlugin


def _timestamp_seconds(message):
    """Find the first ROS Time-like value in a message, including nested fields."""
    primitive_types = (str, bytes, bytearray, int, float, bool)
    visited = set()

    def visit(value, depth=0):
        if value is None or depth > 12 or isinstance(value, primitive_types):
            return None

        seconds = getattr(value, "sec", None)
        nanoseconds = getattr(value, "nanosec", getattr(value, "nsec", None))
        if seconds is not None and nanoseconds is not None:
            try:
                return int(seconds) + int(nanoseconds) / 1e9
            except (TypeError, ValueError, OverflowError):
                pass

        if isinstance(value, (list, tuple)):
            for item in value:
                result = visit(item, depth + 1)
                if result is not None:
                    return result
            return None

        value_id = id(value)
        if value_id in visited:
            return None
        visited.add(value_id)

        get_fields = getattr(value, "get_fields_and_field_types", None)
        if callable(get_fields):
            field_names = list(get_fields())
        else:
            field_names = [name.lstrip("_") for name in getattr(value, "__slots__", ())]

        field_names.sort(key=lambda name: (name not in ("header", "stamp", "timestamp"), name))
        for field_name in field_names:
            result = visit(getattr(value, field_name, None), depth + 1)
            if result is not None:
                return result
        return None

    return visit(message)


class TimestampRateAnalyzer(BagAnalyzerPlugin):
    """Write selected topic timestamps to CSV and report their rate statistics."""

    def __init__(self, bag_path: str, options=None):
        self.bag_path = bag_path
        self.selected_topics = set((options or {}).get("topics", ()))
        self.log_start_time_seconds = (options or {}).get("log_start_time_seconds")
        self.log_duration_seconds = (options or {}).get("log_duration_seconds")
        self.csv_dir = os.path.join(bag_path, "csv")
        self.csv_files = {}
        self.csv_writers = {}
        self.message_indices = {}
        self.topic_stats = {}
        self.delta_timestamps = {}
        self.timestamp_deltas = {}
        self.plot_paths = {}
        self.plots_generated = False

    def matches(self, topic_name: str, message_type: str) -> bool:
        return topic_name in self.selected_topics

    def process(self, topic_name: str, message) -> None:
        self.message_indices[topic_name] = self.message_indices.get(topic_name, 0) + 1
        timestamp = _timestamp_seconds(message)
        if timestamp is None:
            return

        if topic_name not in self.csv_writers:
            os.makedirs(self.csv_dir, exist_ok=True)
            filename = f"{quote(topic_name.strip('/'), safe='') or 'root_topic'}.csv"
            csv_file = open(os.path.join(self.csv_dir, filename), "w", newline="")
            self.csv_files[topic_name] = csv_file
            self.csv_writers[topic_name] = csv.writer(csv_file)
            self.csv_writers[topic_name].writerow(["message_index", "timestamp_seconds"])

        self.csv_writers[topic_name].writerow(
            [self.message_indices[topic_name], f"{timestamp:.9f}"]
        )

        stats = self.topic_stats.setdefault(topic_name, {
            "type": None,
            "last_timestamp": None,
            "interval_count": 0,
            "mean_rate": 0.0,
            "rate_m2": 0.0,
        })
        previous_timestamp = stats["last_timestamp"]
        if previous_timestamp is not None:
            interval = timestamp - previous_timestamp
            self.delta_timestamps.setdefault(topic_name, []).append(timestamp)
            self.timestamp_deltas.setdefault(topic_name, []).append(interval)
            if interval > 0:
                rate = 1.0 / interval
                stats["interval_count"] += 1
                count = stats["interval_count"]
                delta = rate - stats["mean_rate"]
                stats["mean_rate"] += delta / count
                stats["rate_m2"] += delta * (rate - stats["mean_rate"])
        stats["last_timestamp"] = timestamp

    def configure_topic(self, topic_name: str, message_type: str) -> None:
        self.delta_timestamps.setdefault(topic_name, [])
        self.timestamp_deltas.setdefault(topic_name, [])
        self.topic_stats.setdefault(topic_name, {
            "type": message_type,
            "last_timestamp": None,
            "interval_count": 0,
            "mean_rate": 0.0,
            "rate_m2": 0.0,
        })

    def _generate_plots(self) -> None:
        if self.plots_generated:
            return
        self.plots_generated = True

        if not any(self.timestamp_deltas.values()):
            return

        try:
            import matplotlib
            matplotlib.use("Agg")
            import matplotlib.pyplot as plt
        except ImportError as error:
            print(f"Timestamp plots unavailable: Matplotlib is not installed: {error}")
            return

        plots_dir = os.path.join(self.bag_path, "plots")
        os.makedirs(plots_dir, exist_ok=True)
        for topic_name, deltas in self.timestamp_deltas.items():
            if not deltas:
                continue

            topic_slug = re.sub(r"[^A-Za-z0-9._-]+", "_", topic_name.strip("/"))
            topic_hash = hashlib.sha1(topic_name.encode("utf-8")).hexdigest()[:8]
            filename = f"{topic_slug or 'root_topic'}_{topic_hash}.png"
            output_path = os.path.join(plots_dir, filename)

            figure, axes = plt.subplots(figsize=(10, 4), constrained_layout=True)
            timestamps = self.delta_timestamps[topic_name]
            start_time = self.log_start_time_seconds
            if start_time is None:
                start_time = min(timestamps)
            elapsed_timestamps = [timestamp - start_time for timestamp in timestamps]
            axes.plot(elapsed_timestamps, deltas, linewidth=0.8)
            axes.set_title(f"Timestamp deltas: {topic_name}")
            axes.set_xlabel("Time since log start (seconds)")
            axes.set_ylabel("Timestamp delta (seconds)")
            axes.ticklabel_format(axis="x", style="plain", useOffset=False)
            if self.log_duration_seconds is not None and self.log_duration_seconds > 0:
                axes.set_xlim(0, self.log_duration_seconds)
            axes.grid(True, alpha=0.3)
            try:
                figure.savefig(output_path, dpi=150)
                self.plot_paths[topic_name] = os.path.relpath(
                    output_path,
                    self.bag_path,
                ).replace(os.sep, "/")
            except OSError as error:
                print(f"Could not save timestamp plot for {topic_name}: {error}")
            finally:
                plt.close(figure)

    def report_markdown(self) -> str:
        if not self.topic_stats:
            return ""

        self._generate_plots()
        report_lines = [
            "### Timestamp Rate Analysis",
            "",
            "Rates are the inverse of positive intervals between consecutive embedded message timestamps. Standard deviation is the population standard deviation of interval rates.",
            "",
            "| Topic | Datatype | Rate Samples | Mean Rate (Hz) | Rate Std Dev (Hz) |",
            "| :--- | :--- | ---: | ---: | ---: |",
        ]
        for topic_name, stats in sorted(self.topic_stats.items()):
            count = stats["interval_count"]
            mean_rate = f"{stats['mean_rate']:.3f}" if count else "N/A"
            rate_stddev = f"{math.sqrt(stats['rate_m2'] / count):.3f}" if count else "N/A"
            report_lines.append(
                f"| `{topic_name}` | `{stats['type']}` | {count:,} "
                f"| {mean_rate} | {rate_stddev} |"
            )
        for topic_name, plot_path in sorted(self.plot_paths.items()):
            report_lines.extend([
                "",
                f"#### `{topic_name}` timestamp deltas",
                "",
                f"![Timestamp delta plot for {topic_name}]({plot_path})",
            ])
        return "\n".join(report_lines)

    def report_toc_entry(self):
        if not self.topic_stats:
            return None
        return "Timestamp Rate Analysis", "timestamp-rate-analysis"

    def close(self) -> None:
        for csv_file in self.csv_files.values():
            csv_file.close()
        self.csv_files.clear()
        self.csv_writers.clear()
