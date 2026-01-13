import sys
import os

# Add parent directory to sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from utils import *

"""
x = 4950
y = 1700
width = 1500
height = 1000

x = 9515
y = 1555
width = 1473
height = 1023
"""

# We load files
def load(x, y, width, height, display=False):
	print("Loading files...")

	files = ["Ca", "BSE", "Mg", "Fe"]
	imgs = load_images(x, y, width, height, files)

	if display:
		plot_imgs(2, 2, list(imgs.values()), files)

	return imgs
	
def remove_object(mask, pixels):
	"""
	mask: 2D numpy array (0 and 255)
	pixel: tuple (row, column) of the selected pixel
	"""
	binary_mask = (mask == 255).astype(np.uint8)
	
	labeled_mask, num_features = scipy.ndimage.label(binary_mask)
	new_mask = mask.copy()

	for x, y in pixels:
		clicked_label = labeled_mask[y, x]
	
		if clicked_label == 0:
			continue
	
		mask_to_remove = (labeled_mask == clicked_label)
		new_mask[mask_to_remove] = 0  # or 0 if background is 0

	return new_mask

def load_mask(display=False):
	print("Loading mask...")

	with tiff.TiffFile(IMAGE_DIR + "mask.tiff") as tif:
		imgMask = tif.asarray()

	img = remove_object(imgMask, [(500, 1250), (1600, 750), (2100, 1600)])

	if display:
		plot_imgs(1, 2, [imgMask, img], ["Mask", "Mask \\ T2"])

	return img

# Function to experiment with thresholding
def threshold_experiment(quotient, imgs, t=10):
	show_some_histogram(quotient, t)
	
	th = quotient >= t

	plot_imgs(1, 2, [th, th * imgs["BSE"]], ["Mask", "BSE Masked"])

def threshold(quotient, imgs, low, high, display=False):
	print("Thresholding...")

	mi, ma = quotient.min(), quotient.max()
	normalized = ((quotient - mi) / (ma - mi)) * 255

	thImg = skimage.filters.apply_hysteresis_threshold(normalized, low, high).astype("float")

	print("Morphological ops...")
	thImg = skimage.morphology.closing(thImg, skimage.morphology.disk(2))
	thImg = skimage.morphology.area_closing(thImg, 50000)

	if display:
		plot_imgs(1, 2, [thImg, thImg * imgs["BSE"]], ["Mask", "BSE Masked"])

	return thImg

def compute_quotient(imgs, display=False):
	print("Compute quotient...")

	output = imgs["Mg"].astype(float) / (imgs["Mg"].astype(float) + imgs["Fe"].astype(float) + imgs["Ca"].astype(float))

	if display:
		plot_single(output, "Mg / (Mg + Fe + Ca)", "cool")

	return output

def pipeline(display=False, low=210, high=240):
	# We load cropped images
	x = 4542
	y = 1615
	width = 2290
	height = 2210

	imgs = load(x, y, width, height, display)
	mask = load_mask(display)

	quotient = compute_quotient(imgs, display)

	thImg = threshold(quotient, imgs, low, high, display)

	# Show metrics
	print(metrics2(thImg == 1, mask == 255))

if __name__ == '__main__':
	pipeline(True)
