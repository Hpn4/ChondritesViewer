import sys
import time

from PyQt5.QtCore import Qt, QCoreApplication
from PyQt5.QtWidgets import QApplication
from importlib import resources

from viewer.imageLoader import ImageLoader
from viewer.mainWindow import MainWindow

VIEWER = True

if __name__ == "__main__":
    QCoreApplication.setAttribute(Qt.AA_ShareOpenGLContexts)
    app = QApplication(sys.argv)

    config_path = resources.files("resources") / "project1.json"
    loader = ImageLoader(config_path) # Load images

    style_path = resources.files("resources.style") / "style.css"
    app.setStyleSheet(style_path.read_text())

    window = MainWindow(loader)
    window.show()

    sys.exit(app.exec())
