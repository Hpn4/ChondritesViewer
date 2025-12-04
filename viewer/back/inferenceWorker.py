from PyQt5.QtCore import QObject, pyqtSignal, QThread
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TimeElapsedColumn
import math

from viewer.logger import Logger

class InferenceWorker(QObject):
    patch_ready = pyqtSignal(int, int, int, int, object)
    finished = pyqtSignal()

    def __init__(self, cnn, img_dict, feature_order):
        super().__init__()
        self.cnn = cnn
        self.img_dict = img_dict
        self.feature_order = feature_order
        self._stop = False

        self.log = Logger("InferenceWorker").logger

    def stop(self):
        self._stop = True

    def run(self):
        # Estimation du nombre de patches
        H, W = next(iter(self.img_dict.values())).shape
        total_patches = math.ceil(W / 1024) * math.ceil(H / 1024)

        with Progress(
            SpinnerColumn(),
            TextColumn("[progress.description]{task.description}"),
            BarColumn(),
            TextColumn("{task.completed}/{task.total}"),
            TimeElapsedColumn(),
        ) as progress:

            task = progress.add_task("Running inference...", total=total_patches or None)

            for x, y, w, h, sub_img in self.cnn.inference_generator(self.img_dict, self.feature_order):
                if self._stop:
                    self.log.info("Inference stopped by user")
                    break

                self.patch_ready.emit(x, y, w, h, sub_img)
                progress.advance(task)

        self.finished.emit()
