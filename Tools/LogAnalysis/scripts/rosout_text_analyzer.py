import os
from html import escape

from bag_analyzer_plugin import BagAnalyzerPlugin


class RosoutTextAnalyzer(BagAnalyzerPlugin):
    """Write ROS log messages from rosout topics to rosout.txt."""

    LEVEL_NAMES = {
        10: "DEBUG",
        20: "INFO",
        30: "WARNING",
        40: "ERROR",
        50: "FATAL",
    }

    def __init__(self, bag_path: str, options=None):
        self.output_path = os.path.join(bag_path, "rosout.txt")
        self.output_file = None
        self.severity_counts = {
            "DEBUG": 0,
            "INFO": 0,
            "WARNING": 0,
            "ERROR": 0,
            "FATAL": 0,
            "UNKNOWN": 0,
        }
        self.warning_entries = []

    def matches(self, topic_name: str, message_type: str) -> bool:
        return (
            topic_name.rstrip("/").rsplit("/", 1)[-1] == "rosout"
            and message_type == "rcl_interfaces/msg/Log"
        )

    def process(self, topic_name: str, message) -> None:
        if self.output_file is None:
            self.output_file = open(self.output_path, "w", encoding="utf-8")

        stamp = getattr(message, "stamp", None)
        seconds = getattr(stamp, "sec", None)
        nanoseconds = getattr(stamp, "nanosec", 0)
        timestamp = (
            f"{int(seconds)}.{int(nanoseconds):09d}"
            if seconds is not None
            else "unknown-time"
        )
        try:
            numeric_level = int(message.level)
        except (AttributeError, TypeError, ValueError):
            numeric_level = 0
        level = self.LEVEL_NAMES.get(numeric_level, "UNKNOWN")
        self.severity_counts[level] += 1
        logger_name = getattr(message, "name", "")
        log_message = getattr(message, "msg", "")
        source_file = getattr(message, "file", "")
        source_function = getattr(message, "function", "")
        source_line = getattr(message, "line", 0)
        source = f"{source_file}:{source_line} {source_function}".strip()

        entry = f"[{timestamp}] [{level}] [{topic_name}] {logger_name}: {log_message}"
        if source_file or source_function:
            entry += f" ({source})"
        self.output_file.write(f"{entry}\n")
        if numeric_level >= 30:
            self.warning_entries.append(entry)

    def report_markdown(self) -> str:
        """Summarize rosout severities and list warnings and more severe logs."""
        total_messages = sum(self.severity_counts.values())
        if not total_messages:
            return ""

        report_lines = [
            "### ROSout Summary",
            "",
            "| Severity | Count |",
            "| :--- | ---: |",
        ]
        report_lines.extend(
            f"| {severity} | {count:,} |"
            for severity, count in self.severity_counts.items()
        )

        if self.warning_entries:
            report_lines.extend([
                "",
                f"#### WARNING and higher ({len(self.warning_entries):,})",
                "",
                "<details>",
                f"<summary>Show {len(self.warning_entries):,} log messages</summary>",
                "",
                "<pre>",
                escape("\n".join(self.warning_entries), quote=False),
                "</pre>",
                "</details>",
            ])

        return "\n".join(report_lines)

    def report_toc_entry(self):
        if not sum(self.severity_counts.values()):
            return None
        return "ROSout Summary", "rosout-summary"

    def close(self) -> None:
        if self.output_file is not None:
            self.output_file.close()
            self.output_file = None
