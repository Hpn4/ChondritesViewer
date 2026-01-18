from PyQt5.QtCore import QThread
from importlib import resources

from viewer.back.cnn import CNN
from viewer.back.watershed import Watershed
from viewer.back.inferenceWorker import InferenceWorker
from viewer.sharedGLResources import SharedTexture
from viewer.logger import Logger

class Pipeline:
    def __init__(self, window, loader, shared_res):
        super().__init__()

        self.loader = loader
        self.cnn = self.load_model()
        self.watershed = Watershed()
        self.shared_res = shared_res

        self.worker_thread = None
        self.worker = None
        self.texture = None

        self.window = window

        self.log = Logger("Pipeline").logger

    def load_model(self):
        model_path = resources.files("resources") / "lcnn_weights.pth"
        model_hp_path = resources.files("resources") / "hp.json"

        return CNN(str(model_path), str(model_hp_path), device="cpu")

    def start_inference(self):
        features = ["BSE", "Ca", "Al", "Mg", "Si", "Fe", "S", "O"]

        worker = InferenceWorker(self.cnn, self.watershed, self.loader.image_map, features)
        self.texture = SharedTexture.PREDICTION
        
        self.start_worker(worker, worker.run_inference)

    def start_segmentation(self):
        features = ["BSE", "Ca", "Al", "Mg", "Si", "Fe", "S", "O"]

        worker = InferenceWorker(self.cnn, self.watershed, self.loader.image_map, features)
        self.texture = SharedTexture.SEGMENTATION

        self.start_worker(worker, worker.run_object)

    def start_worker(self, worker, fnc):
        if self.worker_thread and self.worker_thread.isRunning():
            self.log.warning("Inference already running.")
            return

        self.worker_thread = QThread()
        self.worker = worker
        self.worker.moveToThread(self.worker_thread)

        self.worker.patch_ready.connect(self.on_patch_ready)
        self.worker.finished.connect(self.worker_thread.quit)
        self.worker_thread.started.connect(fnc)

        self.worker_thread.start()

    def on_patch_ready(self, x, y, w, h, sub_img):
        self.shared_res.update_texture_region(x, y, w, h, sub_img, self.texture)
        self.window.view_grid.update_view()
