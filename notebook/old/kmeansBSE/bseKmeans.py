import sys
import os

# Add parent directory to sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from utils import *

# We load files
def load(x, y, width, height, display=False):
	print("File loading...")

	files = ["BSE"]
	imgs = load_images(x, y, width, height, files)

	if display:
		plot_single(imgs["BSE"], "BSE")

	return imgs

#from skimage import io, color
def kmeans(bse, display=False, save=False):
	print("Kmeans...")

	N = 5

	image = bse.reshape(-1, 1)

	kmeans = sklearn.cluster.KMeans(n_clusters=N, random_state=42, n_init=10)
	labels = kmeans.fit_predict(image)

	segmented = labels.reshape(bse.shape)

	label_img = []
	for i in range(N):
	    mask = (segmented == i).astype(np.uint8)
	    label_img.append(bse * mask)

	if display:
		plt.figure(figsize=(12, 8))
		plt.imshow(segmented, cmap="Set1")
		plt.title("Label")
		plt.axis("off")
		plt.colorbar()
		plt.show()

	# Saving
	if save:
		colormap = matplotlib.colormaps["Set1"]
		colored_labels = colormap(segmented / segmented.max())
		colored_labels = (colored_labels[:, :, :3] * 255).astype(np.uint8)

		cv2.imwrite("image_grayscale.png", colored_labels)

	return label_img

def cluster1(label_img, display=False):
	print("Processing first cluster...")

	noisy = label_img[1]

	if display:
		show_some_histogram(noisy)
	result = np.array(noisy)
	result = result > skimage.filters.threshold_otsu(result)

	result = skimage.morphology.closing(result, skimage.morphology.disk(1))
	result = skimage.morphology.area_opening(result, 10)
	result = skimage.morphology.area_closing(result, 1000)
	result = skimage.morphology.area_opening(result, 300)

	if display:
		plot_imgs(1, 2, [noisy, result], ["Before", "After"])

	return result

def cluster2(label_img, display=False):
	print("Processing second cluster...")

	noisy = label_img[0]

	if display:
		show_some_histogram(noisy)

	mask = noisy > 80

	result = np.zeros_like(noisy)
	#result[mask] = noisy[mask]

	#result[noisy > 100] = 0

	result = noisy > skimage.filters.threshold_otsu(noisy)

	result = skimage.morphology.closing(result, skimage.morphology.disk(1))
	result = skimage.morphology.opening(result, skimage.morphology.disk(2))
	result = skimage.morphology.area_closing(result, 600)
	result = skimage.morphology.area_opening(result, 50)

	if display:
		plot_imgs(1, 2, [noisy, result], ["Before", "After"])

	return result

def cluster3(label_img, display=False):
	print("Processing third cluster...")

	noisy = label_img[2]

	if display:
		show_some_histogram(noisy)

	mask = noisy > 80

	result = np.zeros_like(noisy)

	result = noisy > skimage.filters.threshold_otsu(noisy)

	result = skimage.morphology.closing(result, skimage.morphology.disk(1))
	result = skimage.morphology.area_opening(result, 12)
	result = skimage.morphology.area_closing(result, 300)

	if display:
		plot_imgs(1, 2, [noisy, result], ["Before", "After"])

	return result

def pipeline(display=False):
	x = 4950
	y = 1700
	width = 1500
	height = 1000

	"""
	x = 10200
	y = 1400
	width = 700
	height = 700
	"""

	imgs = load(x, y, width, height, display)

	bse = imgs["BSE"]

	label_img = kmeans(bse, display)

	cluster1_processed = cluster1(label_img, display)
	cluster2_processed = cluster2(label_img, display)
	cluster3_processed = cluster3(label_img, display)

	print("Combining result...")
	output = cluster1_processed + cluster2_processed + cluster3_processed

	plot_imgs(1, 2, [bse, bse * output])

if __name__ == '__main__':
	pipeline()