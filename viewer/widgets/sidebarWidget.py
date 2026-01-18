from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (
    QWidget, QVBoxLayout, QLabel, QDoubleSpinBox, QTabWidget, QCheckBox
)
from viewer.widgets import ColorLabelList, CollapsibleWidget

class ViewerLayerWidget(QWidget):
    def __init__(self, viewer_name, view, parent=None):
        super().__init__(parent)
        self.layers = view.layers
        layout = QVBoxLayout(self)
        layout.setSpacing(10)
        layout.setContentsMargins(10,10,0,0)

        for layer in self.layers:
            checkbox = QCheckBox(layer.name)
            checkbox.setChecked(layer.visible)
            checkbox.stateChanged.connect(self._make_callback(layer, view))
            layout.addWidget(checkbox)

        layout.addStretch()

    def _make_callback(self, layer, view):
        def callback(state):
            layer.visible = state == 2  # Qt.Checked == 2
            view.update()

        return callback

class LayerTabWidget(QTabWidget):
    def __init__(self, views, parent=None):
        super().__init__(parent)

        self.setTabPosition(QTabWidget.North)
        self.tabBar().setExpanding(False)
        self.tabBar().setElideMode(Qt.ElideRight)
        self.tabBar().setUsesScrollButtons(False)

        for i, view in enumerate(views):
            tab = ViewerLayerWidget(f"Viewer {i+1}", view, parent=self)
            self.addTab(tab, f"Viewer {i+1}")

class SidebarWidget(QWidget):
    def __init__(self, data, views=None, parent=None):
        super().__init__(parent)
        self.data = data

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 5, 5)
        layout.setSpacing(10)
        self.setFixedWidth(250)

        # Section labels
        label_section = ColorLabelList(parent=self)
        collapsible = CollapsibleWidget("Labels", label_section)
        layout.addWidget(collapsible)

        # Brush size
        brush_label = QLabel("Taille pinceau:")
        brush_spin = QDoubleSpinBox()
        brush_spin.setRange(0.1, 100.0)
        brush_spin.setSingleStep(0.1)
        brush_spin.setValue(self.data["brush_size"])
        brush_spin.valueChanged.connect(self._on_brush_size_changed)
        layout.addWidget(brush_label)
        layout.addWidget(brush_spin)

        # Layers per viewer
        if views:
            layer_tabs = LayerTabWidget(views, parent=self)
            collapsible_layers = CollapsibleWidget("Layers par Viewer", layer_tabs)
            layout.addWidget(collapsible_layers)

        layout.addStretch()

    def _on_brush_size_changed(self, value: float):
        self.data["brush_size"] = value
