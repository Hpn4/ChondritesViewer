from rich.logging import RichHandler
import logging

class Logger:
    """
    Wrapper around Rich
    """

    def __init__(self, name="app_logger", level=logging.DEBUG):
        self.logger = logging.getLogger(name)
        self.logger.setLevel(level)

        handler = RichHandler(
            rich_tracebacks=True,
            show_time=True,
            show_level=True
        )

        formatter = logging.Formatter("%(message)s")
        handler.setFormatter(formatter)

        if not self.logger.hasHandlers():
            self.logger.addHandler(handler)
