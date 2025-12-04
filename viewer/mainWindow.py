from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QHBoxLayout, QVBoxLayout, QPushButton
from PyQt5.QtCore import Qt, QEvent
from importlib import resources

from viewer.widgets import ViewGridWidget, SidebarWidget
from viewer.sharedGLResources import SharedGLResources
from viewer.back.pipeline import Pipeline

class MainWindow(QMainWindow):
    def __init__(self, loader):
        super().__init__()

        self.space_pressed = False
        self.data = {"selected_label": 1, "brush_size": 5.0}
        self.loader = loader

        self.label_path = resources.files("viewer.resources") / "labels.png"

        QApplication.instance().installEventFilter(self)

        # === Layout principal ===
        central_widget = QWidget(parent=self)
        main_layout = QHBoxLayout(central_widget)
        main_layout.setContentsMargins(0, 0, 0, 0)

        # ---- Ressources OpenGL partagées ----
        self.shared_res = SharedGLResources(loader.width, loader.height, data=self.data)
        self.shared_res.label_path = self.label_path

        self.view_grid = ViewGridWidget(loader, shared_res=self.shared_res, parent=central_widget)

        # ---- Pipeline ----
        self.pipeline = Pipeline(self, loader, self.shared_res)

        # ---- Panneau latéral gauche ----
        self.sidebar = SidebarWidget(self.data, views=self.view_grid.viewers, parent=central_widget)

        # ---- Bouton d’inférence ----
        self.run_button = QPushButton("Run Inference")
        self.run_button.clicked.connect(self.pipeline.start_inference)
        self.sidebar.layout().addWidget(self.run_button)

        # ---- Save button ----
        self.save_button = QPushButton("Save Labels")
        self.save_button.clicked.connect(self.save_labels)
        self.sidebar.layout().addWidget(self.save_button)

        main_layout.addWidget(self.sidebar)
        main_layout.addWidget(self.view_grid)
        main_layout.setStretch(0, 0)
        main_layout.setStretch(1, 1)

        self.viewers = self.view_grid.viewers
        self.setCentralWidget(central_widget)
        self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.setFocus()
        self.resize(1200, 800)

    def save_labels(self):
        self.shared_res.save_fbo_texture_to_file(self.label_path)

    # ===================== EVENT KEYS =====================
    def eventFilter(self, obj, event):
        if event.type() == QEvent.KeyPress:
            if event.key() == Qt.Key_Space:
                self.space_pressed = True
                return True
        elif event.type() == QEvent.KeyRelease:
            if event.key() == Qt.Key_Space:
                self.space_pressed = False
                return True
        return super().eventFilter(obj, event)
