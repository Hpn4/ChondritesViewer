import json
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TimeElapsedColumn
from importlib import resources
import numpy as np
import pyvips

from viewer.logger import Logger

class ImageLoader:
    def __init__(self, path):
        self.path = path
        self.width = 0
        self.height = 0
        self.data = {}
        self.image_map = {}
        self.log = Logger("ImageLoader").logger
        self.load()

    def load_single(self, path):
        # Chargement avec pyvips
        img = pyvips.Image.new_from_file(path, access="random")

        self.width = img.width
        self.height = img.height

        r = img[0]
        g = img[1]
        b = img[2]

        gray = r.ifthenelse(r, g.ifthenelse(g, b))

        # Conversion en numpy et passage en niveau de gris
        arr = np.ndarray(buffer=gray.write_to_memory(),
                         dtype=np.uint8,
                         shape=(gray.height, gray.width))

        return arr

    def load(self):
        # Chargement du JSON
        try:
            with open(self.path, "r") as f:
                self.data = json.load(f)
        except Exception as e:
            self.log.error(f"Failed to open JSON file: {e}")
            return

        imgs = self.data.get("imgs", {})
        if not isinstance(imgs, dict):
            self.log.warning('"imgs" section missing or invalid in JSON')
            return

        with Progress(
            SpinnerColumn(),
            TextColumn("[progress.description]{task.description}"),
            BarColumn(),
            TextColumn("{task.completed}/{task.total}"),
            TimeElapsedColumn(),
        ) as progress:
            task = progress.add_task("Loading images...   ", total=len(imgs))

            for key, path in imgs.items():
                try:
                    img_path = resources.files("resources.data") / path
                    img = self.load_single(img_path)
                    self.image_map[key] = img
                    self.height, self.width = img.shape[:2]
                    self.log.info(f"Loaded image '{key}' from {path}")
                except Exception as e:
                    self.log.error(f"Failed to load image '{key}': {e}")
                finally:
                    progress.advance(task)

        self.image_map["0"] = np.zeros((self.height, self.width), dtype=np.uint8)

    def prepare_views(self):
        views = []

        views_data = self.data.get("views", {}).get("datas", [])
        if not views_data:
            self.log.warning("No views found in JSON")
            return views

        # Progress bar pour les vues
        with Progress(
            SpinnerColumn(),
            TextColumn("[progress.description]{task.description}"),
            BarColumn(),
            TextColumn("{task.completed}/{task.total}"),
            TimeElapsedColumn(),
        ) as progress:
            task = progress.add_task("Preparing views...  ", total=len(views_data))

            for view in views_data:
                row = view.get("row", 0)
                col = view.get("col", 0)
                grayscale = view.get("grayscale", True)

                if grayscale:
                    chan = view.get("r", "0")
                    img = self.image_map.get(chan, self.image_map["0"])
                else:
                    r_chan = view.get("r", "0")
                    g_chan = view.get("g", "0")
                    b_chan = view.get("b", "0")
                    # Assemblage RGB
                    img = np.stack([
                        self.image_map.get(r_chan, self.image_map["0"]),
                        self.image_map.get(g_chan, self.image_map["0"]),
                        self.image_map.get(b_chan, self.image_map["0"])
                    ], axis=2)

                views.append((row, col, img, grayscale))
                progress.advance(task)

        return views
