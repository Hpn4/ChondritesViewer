from PyQt5.QtWidgets import QFrame, QVBoxLayout, QRadioButton, QButtonGroup
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtGui import QColor

class ColorLabelList(QFrame):
    labelSelected = pyqtSignal(int)  # signal émis quand un label est sélectionné

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setObjectName("colorListContainer")

        # Layout
        self.content_layout = QVBoxLayout(self)
        self.content_layout.setContentsMargins(0, 0, 0, 0)
        self.content_layout.setSpacing(2)

        # Button group (exclusif)
        self.button_group = QButtonGroup(self)
        self.button_group.setExclusive(True)

        # Liste des labels et couleurs
        labels = [
            ("Chondre I", "#577277"),
            ("Chondre II", "#468232"),
            ("Matrix", "#c65197"),
            ("CAIs", "#73bed3"),
            ("Carbonate", "#a8ca58"),
            ("Soufre", "#e7d5b3"),
            ("Fe Oxydé", "#7a367b"),
            ("Fe", "#a53030"),
            ("Epoxy", "#da863e")
        ]

        app = QApplication.instance()

        for i, (name, color_hex) in enumerate(labels):
            btn = QRadioButton(name)
            btn.setObjectName("colorListButton")
            btn.setProperty("index", i)

            btn.setStyleSheet(app.styleSheet() + f"""
                QRadioButton::indicator {{
                    background-color: {color_hex};
                }}"""
            )

            self.button_group.addButton(btn)
            self.content_layout.addWidget(btn)

            btn.toggled.connect(lambda checked, b=btn: self._on_button_toggled(b, checked))

            if i == 0:
                btn.setChecked(True)

    def _on_button_toggled(self, btn, checked):
        if checked:
            index = btn.property("index")
            self.window().data["selected_label"] = index + 1
