from PyQt5.QtWidgets import QWidget, QPushButton, QVBoxLayout

class CollapsibleWidget(QWidget):
    def __init__(self, title: str, content_widget: QWidget, parent: QWidget = None):
        super().__init__(parent)
        self.content_widget = content_widget

        # Layout principal
        main_layout = QVBoxLayout(self)
        main_layout.setSpacing(0)
        main_layout.setContentsMargins(0, 0, 0, 0)

        # Header
        self.header_button = QPushButton(title)
        self.header_button.setCheckable(True)
        self.header_button.setChecked(True)
        self.header_button.setObjectName("headerButton")
        main_layout.addWidget(self.header_button)

        # Contenu
        main_layout.addWidget(self.content_widget)
        self.content_widget.setVisible(True)

        # Connexion toggle
        self.header_button.toggled.connect(self.content_widget.setVisible)

    def setCollapsed(self, collapsed: bool):
        """Plie ou déplie le widget"""
        self.header_button.setChecked(not collapsed)
        self.content_widget.setVisible(not collapsed)

    def isCollapsed(self) -> bool:
        """Retourne True si le widget est plié"""
        return not self.header_button.isChecked()
