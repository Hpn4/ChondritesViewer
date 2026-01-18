from skimage.morphology import h_maxima, remove_small_objects, remove_small_holes, closing, disk
from skimage.segmentation import watershed, find_boundaries
from skimage import measure
from scipy.ndimage import distance_transform_edt, gaussian_filter
import numpy as np

labels = [
    ("BG", "#000000"),
    ("Chondre I", "#577277"),
    ("Chondre II", "#468232"),
    ("Matrix", "#c65197"),
    ("CAIs", "#73bed3"),
    ("Soufre", "#e7d5b3"),
    ("Fe Oxydé", "#7a367b"),
    ("Fe", "#a53030"),
    ("Epoxy", "#da863e"),
    ("Boundaries", "#FFFFFF")
]

def hex_to_rgb(hex_str):
    hex_str = hex_str.lstrip('#')
    return tuple(int(hex_str[i:i+2], 16) for i in (0, 2, 4))

palette = np.array([hex_to_rgb(c[1]) for c in labels], dtype=np.uint8)

BOUNDARIES_ID = len(labels) - 1

class Watershed:

    def __init__(self, min_obj=100, hole_thresh_small=500, hole_thresh_large=1000,
                 closing_disk=4, distance_sigma=4, h_maxima_h=2):
        self.min_obj = min_obj
        self.hole_thresh_small = hole_thresh_small
        self.hole_thresh_large = hole_thresh_large
        self.closing_disk = closing_disk
        self.distance_sigma = distance_sigma
        self.h_maxima_h = h_maxima_h

    def cleanup(self, img):
        cleaned = remove_small_objects(img.astype(bool), min_size=self.min_obj)
        filled = remove_small_holes(cleaned, area_threshold=self.hole_thresh_small)
        closed = closing(filled, disk(self.closing_disk))
        closed = remove_small_holes(closed, area_threshold=self.hole_thresh_large)

        return closed

    def apply_watershed(self, img):
        distance = distance_transform_edt(img)
        distance = gaussian_filter(distance, sigma=self.distance_sigma)
        markers = measure.label(h_maxima(distance, h=self.h_maxima_h))
        labels_ws = watershed(-distance, markers, mask=img)

        return labels_ws

    def add_boundaries(self, labels, class_id):
        out = np.zeros(labels.shape, dtype=np.uint8)
        out[labels > 0] = class_id + 1
        boundaries = find_boundaries(labels, mode='outer')
        out[boundaries] = BOUNDARIES_ID

        return out

    def forward_generator(self, img, classes_to_process, patch_size=1024, stride=1000):
        H, W = img.shape[:2]

        for y in range(0, H, stride):
            for x in range(0, W, stride):
                y_end = min(y + patch_size, H)
                x_end = min(x + patch_size, W)
                patch = img[y:y_end, x:x_end]

                patch_out = np.zeros(patch.shape, dtype=np.uint8)

                for class_id in classes_to_process:
                    masked = (patch == class_id)
                    mask_cleaned = self.cleanup(masked)
                    if np.any(mask_cleaned):
                        segments = self.apply_watershed(mask_cleaned)
                        obj_img = self.add_boundaries(segments, class_id)

                        patch_out = np.maximum(patch_out, obj_img)

                patch_out = palette[patch_out] # Colorized

                yield (x, y, patch_out.shape[1], patch_out.shape[0], patch_out)
