from PyQt5.QtCore import QThread
from importlib import resources

from viewer.back.cnn import CNN
from viewer.back.inferenceWorker import InferenceWorker
from viewer.logger import Logger

class Pipeline:
    def __init__(self, window, loader, shared_res):
        super().__init__()

        self.loader = loader
        self.cnn = self.load_model()
        self.shared_res = shared_res

        self.worker_thread = None
        self.worker = None

        self.window = window

        self.log = Logger("Pipeline").logger

    def load_model(self):
        model_path = resources.files("viewer.resources") / "lcnn_weights.pth"
        return CNN(str(model_path), device="cpu")

    # ===================== THREAD SETUP =====================
    def start_inference(self):
        """Lance le worker d’inférence dans un thread séparé."""
        if self.worker_thread and self.worker_thread.isRunning():
            self.log.warning("Inference already running.")
            return

        features = ["Ca","Al", "BSE", "Mg", "Si", "Fe", "S", "O"]

        # Crée le thread et le worker
        self.worker_thread = QThread()
        self.worker = InferenceWorker(self.cnn, self.loader.image_map, features)
        self.worker.moveToThread(self.worker_thread)

        # Connexions signaux/slots
        self.worker.patch_ready.connect(self.on_patch_ready)
        self.worker.finished.connect(self.worker_thread.quit)
        self.worker_thread.started.connect(self.worker.run)

        # Lancer le thread
        self.worker_thread.start()

    def on_patch_ready(self, x, y, w, h, sub_img):
        self.shared_res.update_texture_region(x, y, w, h, sub_img)
        self.window.view_grid.update_view()  # demande redraw
