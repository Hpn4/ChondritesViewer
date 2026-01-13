import numpy as np
from skimage.morphology import dilation, disk
from scipy.ndimage import distance_transform_edt

def merge_labels(img, class_dict):
    """
    Merge label indices into binary masks per merged class.
    img: np.array, labeled image
    class_dict: dict, {class_name: [list_of_labels]}
    Returns: dict {class_name: binary mask}
    """
    merged_masks = {}
    for cname, labels in class_dict.items():
        mask = np.isin(img, labels)
        merged_masks[cname] = mask
    return merged_masks

def iou_score(gt_mask, pred_mask):
    intersection = np.logical_and(gt_mask, pred_mask).sum()
    union = np.logical_or(gt_mask, pred_mask).sum()
    if union == 0:
        return 1.0
    return intersection / union

def dice_score(gt_mask, pred_mask):
    intersection = np.logical_and(gt_mask, pred_mask).sum()
    size_sum = gt_mask.sum() + pred_mask.sum()
    if size_sum == 0:
        return 1.0
    return 2 * intersection / size_sum

def tolerant_iou(gt_mask, pred_mask, radius=2):
    """
    Bidirectional dilation tolerant IoU.
    """
    selem = disk(radius)
    gt_dil = dilation(gt_mask, selem)
    pred_dil = dilation(pred_mask, selem)

    intersection = np.logical_and(pred_mask, gt_dil).sum() + np.logical_and(gt_mask, pred_dil).sum()
    union = gt_mask.sum() + pred_mask.sum()
    if union == 0:
        return 1.0
    return intersection / union

def boundary_f1_score(gt_mask, pred_mask, radius=2):
    """
    Compute Boundary F1 score with tolerance radius.
    """
    selem = disk(radius)

    # Extract boundaries (binary edges)
    gt_boundary = gt_mask ^ dilation(gt_mask, selem)
    pred_boundary = pred_mask ^ dilation(pred_mask, selem)

    if gt_boundary.sum() == 0 and pred_boundary.sum() == 0:
        return 1.0
    if gt_boundary.sum() == 0 or pred_boundary.sum() == 0:
        return 0.0

    # Dilate boundaries to create tolerance regions
    gt_dil = dilation(gt_boundary, selem)
    pred_dil = dilation(pred_boundary, selem)

    # Precision: fraction of predicted boundary pixels near GT boundary
    precision = np.logical_and(pred_boundary, gt_dil).sum() / pred_boundary.sum()
    # Recall: fraction of GT boundary pixels near predicted boundary
    recall = np.logical_and(gt_boundary, pred_dil).sum() / gt_boundary.sum()

    if precision + recall == 0:
        return 0.0
    return 2 * precision * recall / (precision + recall)

def surface_dice(gt_mask, pred_mask, tol=2):
    """
    Compute Surface Dice at tolerance 'tol' pixels.
    Based on the approach used in MONAI and medical imaging literature.

    Steps:
    - Extract boundaries of gt and pred masks.
    - Compute distance transforms of each boundary.
    - Calculate fraction of boundary points within tol distance.

    Returns:
        surface_dice score [0..1]
    """

    def boundary(mask):
        # Boundary = mask - eroded mask
        from skimage.morphology import erosion, disk
        selem = disk(1)
        eroded = erosion(mask, selem)
        return mask ^ eroded

    gt_boundary = boundary(gt_mask)
    pred_boundary = boundary(pred_mask)

    if gt_boundary.sum() == 0 and pred_boundary.sum() == 0:
        return 1.0
    if gt_boundary.sum() == 0 or pred_boundary.sum() == 0:
        return 0.0

    # Distance transform from GT boundary (distance to nearest boundary pixel)
    dt_gt = distance_transform_edt(~gt_boundary)
    dt_pred = distance_transform_edt(~pred_boundary)

    # For each predicted boundary pixel, get distance to closest GT boundary pixel
    pred_to_gt = dt_gt[pred_boundary]
    # For each GT boundary pixel, get distance to closest predicted boundary pixel
    gt_to_pred = dt_pred[gt_boundary]

    # Count boundary pixels within tolerance
    pred_match = np.sum(pred_to_gt <= tol)
    gt_match = np.sum(gt_to_pred <= tol)

    surface_dice_score = (pred_match + gt_match) / (pred_boundary.sum() + gt_boundary.sum())
    return surface_dice_score

def evaluate_segmentation(gt_img, pred_img, gt_classes, pred_classes, radius=2, tol=2):
    """
    Evaluate segmentation masks with multiple metrics per class.
    gt_img, pred_img: labeled images (np.array)
    gt_classes, pred_classes: dict {class_name: [label_indices]}
    radius: dilation radius for tolerant IoU and boundary F1
    tol: tolerance for surface dice (in pixels)
    Returns: dict with per-class metrics and mean metrics
    """

    # Merge labels to masks per class
    gt_masks = merge_labels(gt_img, gt_classes)
    pred_masks = merge_labels(pred_img, pred_classes)

    classes = list(gt_classes.keys())
    results = {}

    metrics_sum = {
        'IoU': 0.0,
        'Dice': 0.0,
        'BoundaryF1': 0.0,
        'TolerantIoU': 0.0,
        'SurfaceDice': 0.0
    }
    valid_class_count = 0

    for cls in classes:
        gt_mask = gt_masks.get(cls, np.zeros_like(gt_img, dtype=bool))
        pred_mask = pred_masks.get(cls, np.zeros_like(pred_img, dtype=bool))

        # Avoid classes with no pixels in both GT and pred
        if gt_mask.sum() == 0 and pred_mask.sum() == 0:
            # Perfect match on empty class
            iou = dice = bf1 = tiou = sdice = 1.0
        else:
            iou = iou_score(gt_mask, pred_mask)
            dice = dice_score(gt_mask, pred_mask)
            bf1 = boundary_f1_score(gt_mask, pred_mask, radius=radius)
            tiou = tolerant_iou(gt_mask, pred_mask, radius=radius)
            sdice = surface_dice(gt_mask, pred_mask, tol=tol)

        results[cls] = {
            'IoU': iou,
            'Dice': dice,
            'BoundaryF1': bf1,
            'TolerantIoU': tiou,
            'SurfaceDice': sdice
        }

        metrics_sum['IoU'] += iou
        metrics_sum['Dice'] += dice
        metrics_sum['BoundaryF1'] += bf1
        metrics_sum['TolerantIoU'] += tiou
        metrics_sum['SurfaceDice'] += sdice

        valid_class_count += 1

    # Compute mean metrics
    results['mean'] = {k: v / valid_class_count for k, v in metrics_sum.items()}

    return results
