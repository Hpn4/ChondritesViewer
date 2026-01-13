import os
import skimage.io
import matplotlib.pyplot as plt
import matplotlib
import cv2
import tifffile as tiff
import numpy as np
import scipy
import sklearn

IMAGE_DIR = "data/elements/"

def plot_imgs(rows, cols, imgs, labels=None):
    fig, axes = plt.subplots(rows, cols, figsize=(5 * cols, 5 * rows))
    ax = axes.flatten()

    for i in range(len(imgs)):
        ax[i].imshow(imgs[i], cmap="gray")
        ax[i].set_axis_off()
        if labels:
            ax[i].set_title(labels[i])

    fig.tight_layout()
    #plt.savefig("graphique_sans_fond.png", transparent=True, dpi=900)
    plt.show()

def show_some_histogram(img, threshold=-1):
    mi, ma = img.min(), img.max()
    
    img_scaled = ((img - mi) / (ma - mi)) * 255
    img = img_scaled.astype(np.uint8)

    hist, bins = np.histogram(img, bins=256, range=(0, 255))
    hist[0] = 0

    bin_centers = (bins[:-1] + bins[1:]) / 2

    plt.figure(figsize=(8, 5))
    plt.bar(bin_centers, hist, width=1.0, color='gray', edgecolor='black')
    
    if threshold >= 0:
        plt.axvline(x=threshold, color='red', linestyle='--', linewidth=2, label=f'Threshold = {threshold}')
        plt.legend()

    plt.xlabel("Intensité des pixels")
    plt.ylabel("Fréquence")
    plt.title("Histogramme de l'image")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.show()

def load_images(x, y, width, height, images):
    tif_files = []
    for img in images:
        tif_files.append(IMAGE_DIR + "Mosaic element_" + img + ".tif")

    roi = (slice(y, y + height), slice(x, x + width))
    imgs = dict()

    # Loop over files and load only the region
    for i, tif_file in enumerate(tif_files):
        with tiff.TiffFile(tif_file) as tif:
            sub_image = tif.asarray()[roi]
            img = np.array(sub_image).astype(np.float32)
            img = np.max(img, axis=-1)

            imgs[images[i]] = img

    return imgs

def plot_rgb(imgs, red, green, blue, mask):
    plt.figure(figsize=(10,6))
    plt.imshow(np.array([mask * imgs[red] / 255, mask * imgs[green] / 255, mask * imgs[blue] / 255]).swapaxes(0, 2).swapaxes(0,1))
    plt.title(f"{red} (r), {green} (g), {blue} (b)")
    plt.axis('off')
    plt.savefig(f"{red}_{green}_{blue}.png", transparent=False, dpi=220, bbox_inches="tight")
    plt.show()

def plot_single(img, label, cmap="grey", name=None):
    plt.figure(figsize=(12,8))
    if cmap == "grey":
        plt.imshow(img, cmap=cmap, vmin=0, vmax=255)
    else:
        plt.imshow(img, cmap=cmap)
    plt.title(label)
    if name:
        plt.savefig(name, transparent=True, dpi=700, bbox_inches="tight")
    plt.show()

def dice_index(mask_pred, mask_true):
    mask_pred = mask_pred.flatten()
    mask_true = mask_true.flatten()
    
    intersection = np.sum(mask_pred * mask_true)
    total = np.sum(mask_pred) + np.sum(mask_true)

    dice = 2 * intersection / total
    return dice

def metrics(pred, ref):
    pred = pred.flatten()
    ref = ref.flatten()

    # Build the confusion matrix
    TP = np.sum(pred * ref)
    TN = np.sum((1 - pred) * (1 - ref))
    FP = np.sum(pred) - TP
    FN = np.sum(ref) - TP

    return {
        "precision": TP / (TP + FP),
        "recall": TP / (TP + FN),
        "accuracy": (TP + TN) / (TP + TN + FP + FN),
        "dice": 2 * TP / (2 * TP + FP + FN),
        "IoU": TP / (TP + FP + FN),
    }

from sklearn.metrics import precision_score, recall_score, accuracy_score, f1_score, jaccard_score

def metrics2(pred, ref):
    pred = pred.flatten()
    ref = ref.flatten()

    return {
        "precision": precision_score(ref, pred, zero_division=0),
        "recall": recall_score(ref, pred, zero_division=0),
        "accuracy": accuracy_score(ref, pred),
        "dice": f1_score(ref, pred, zero_division=0),
        "IoU": jaccard_score(ref, pred, zero_division=0),
    }

def compute_formula_images(images: dict[str, np.ndarray], formulae: list[str]) -> dict[str, np.ndarray]:
    results = {}
    
    safe_locals = {k: v.astype(np.float32) for k, v in images.items()}

    for formula in formulae:
        try:
            print(formula)
            result = eval(formula, safe_locals)
            print(result.shape)
            results[formula] = result
        except Exception as e:
            print(f"Failed to compute '{formula}': {e}")
            results[formula] = None
    return results

def compute_gradient(img):
    Gx, Gy = np.gradient(img)
    
    G_int = np.sqrt(Gx**2 + Gy**2)
    G_or = np.arctan2(Gx, Gy)

    return G_int, G_or

def compute_features(images, descr):
    basic_imgs = []
    formulae_imgs = []
    gradient_imgs = []

    for s in descr:
        if s[0] == '=':
            formulae_imgs.append(s[2:])
        elif s[0] == 'g':
            gradient_imgs += compute_gradient(images[s[2:]])
        else:
            basic_imgs.append(images[s])

    formulae_imgs = compute_formula_images(images, formulae_imgs)
    if len(formulae_imgs) > 0:
        formulae_imgs = np.stack(list(formulae_imgs.values()), axis=0)

    arrays = [basic_imgs, formulae_imgs, gradient_imgs]
    non_empty_arrays = [arr for arr in arrays if len(arr) > 0]

    stacked = np.concatenate(non_empty_arrays)
    stacked = np.moveaxis(stacked, 0, -1)

    return stacked
