class BagAnalyzerPlugin:
    """Base interface for topic-specific bag analyzers."""

    def matches(self, topic_name: str, message_type: str) -> bool:
        raise NotImplementedError

    def process(self, topic_name: str, message) -> None:
        raise NotImplementedError

    def configure_topic(self, topic_name: str, message_type: str) -> None:
        pass

    def close(self) -> None:
        pass

    def report_markdown(self) -> str:
        """Return this analyzer's report section, if it has one."""
        return ""

    def report_toc_entry(self):
        """Return this analyzer's ToC label and anchor when it has a report."""
        return None
