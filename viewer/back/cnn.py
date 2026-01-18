import torch
import json
import numpy as np
import torch.nn as nn
import torch.nn.functional as F
import matplotlib.pyplot as plt

from viewer.back.models import model_factory

labels = [
    ("Chondre I", "#577277"),
    ("Chondre II", "#468232"),
    ("Matrix", "#c65197"),
    #("Chondre I", "#577277"), # mesostasis will be deleted
    #("Chondre II", "#468232"),
    ("CAIs", "#73bed3"),
    #("Carbonate", "#a8ca58"),
    ("Soufre", "#e7d5b3"),
    ("Fe Oxydé", "#7a367b"),
    ("Fe", "#a53030"),
    ("Epoxy", "#da863e")
]

def hex_to_rgb(hex_str):
    hex_str = hex_str.lstrip('#')
    return tuple(int(hex_str[i:i+2], 16) for i in (0, 2, 4))

palette = np.array([hex_to_rgb(c[1]) for c in labels], dtype=np.uint8)

class CNN:
    def __init__(self, model_path, hp_file, device='cuda'):
        self.device = device

        self.model = model_factory(hp_file)
        self.model.load_state_dict(torch.load(model_path, map_location=self.device))
        self.model.to(self.device)
        self.model.eval()

    def center_labels(self, tensor, labels):
        _, _, img_h, img_w = tensor.shape
        seg_h, seg_w = labels.shape
            
        start_y = (seg_h - img_h) // 2
        start_x = (seg_w - img_w) // 2
            
        return labels[start_y:start_y+img_h, start_x:start_x+img_w]

    def inference_generator(self, img_dict, feature_order, patch_size=1024, stride=1000):
        H, W = next(iter(img_dict.values())).shape

        self.output_img = np.zeros((H, W), dtype=np.uint8)

        for y in range(0, H, stride):
            for x in range(0, W, stride):
                y_end = min(y + patch_size, H)
                x_end = min(x + patch_size, W)

                patch_channels = [img_dict[f][y:y_end, x:x_end] for f in feature_order]

                patch_stack = np.stack(patch_channels, axis=0)
                patch_tensor = torch.tensor(patch_stack[np.newaxis, ...], dtype=torch.float32).to(self.device)

                # Inference
                with torch.inference_mode():
                    logits = self.model(patch_tensor)
                    labels = torch.argmax(logits, dim=1).squeeze().cpu().numpy().astype(np.uint8)

                # Center prediction (conv may upscale with certain HP)
                labels = self.center_labels(patch_tensor, labels)

                out_h, out_w = labels.shape
                self.output_img[y:y+out_h, x:x+out_w] = labels
                colored_img = palette[labels]

                yield (x, y, colored_img.shape[1], colored_img.shape[0], colored_img)
