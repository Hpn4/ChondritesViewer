import torch
import numpy as np
import torch.nn as nn
import torch.nn.functional as F
import matplotlib.pyplot as plt

labels = [
    ("Chondre I", "#577277"),
    ("Chondre II", "#468232"),
    ("Matrix", "#c65197"),
    ("Chondre I", "#577277"), # mesostasis will be deleted
    ("Chondre II", "#468232"),
    ("CAIs", "#73bed3"),
    ("Carbonate", "#a8ca58"),
    ("Soufre", "#e7d5b3"),
    ("Fe Oxydé", "#7a367b"),
    ("Fe", "#a53030"),
    ("Epoxy", "#da863e")
]

# --- convertir hex -> RGB 0-255 ---
def hex_to_rgb(hex_str):
    hex_str = hex_str.lstrip('#')
    return tuple(int(hex_str[i:i+2], 16) for i in (0, 2, 4))

palette = np.array([hex_to_rgb(c[1]) for c in labels], dtype=np.uint8)

class LCNN(nn.Module):
    def __init__(self, in_channels, num_classes):
        super().__init__()
        self.conv1 = nn.Conv2d(in_channels, 20, kernel_size=3, stride=1, padding=0)
        self.relu1 = nn.ReLU(inplace=True)
        self.conv2 = nn.Conv2d(20, 90, kernel_size=3, stride=1, padding=0)
        self.relu2 = nn.ReLU(inplace=True)
        self.conv3 = nn.Conv2d(90, num_classes, kernel_size=1, stride=1, padding=0)

    def forward(self, x):
        x = self.relu1(self.conv1(x))
        x = self.relu2(self.conv2(x))
        x = self.conv3(x)
        return x

class CNN:
    def __init__(self, model_path, device='cuda'):
        self.device = device

        self.model = LCNN(8, 11)
        self.model.load_state_dict(torch.load(model_path, map_location=device))
        self.model.to(device)
        self.model.eval()

    def inference_generator(self, img_dict, feature_order, patch_size=1024, stride=1000):
        H, W = next(iter(img_dict.values())).shape

        # Préparer la sortie finale
        output_img = np.zeros((H, W), dtype=np.uint8)

        for y in range(0, H, stride):
            for x in range(0, W, stride):
                y_end = min(y + patch_size, H)
                x_end = min(x + patch_size, W)

                # Extraire patchs par feature
                patch_channels = [img_dict[f][y:y_end, x:x_end] for f in feature_order]

                patch_stack = np.stack(patch_channels, axis=0)
                patch_tensor = torch.tensor(patch_stack[np.newaxis, ...], dtype=torch.float32).to(self.device)

                # Inference
                with torch.inference_mode():
                    logits = self.model(patch_tensor)
                    labels = torch.argmax(logits, dim=1).squeeze().cpu().numpy().astype(np.uint8)

                # Copier dans l'image finale
                out_h, out_w = labels.shape
                output_img[y:y+out_h, x:x+out_w] = labels
                colored_img = palette[labels]

                yield (x, y, colored_img.shape[1], colored_img.shape[0], colored_img)
