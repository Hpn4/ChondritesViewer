from utils import *

# We load files
def load(display):
	x = 4950
	y = 1700
	width = 1500
	height = 1000

	files = ["BSE"]
	imgs = load_images(x, y, width, height, files)

	if display:
		plot_single(imgs["BSE"], "Input BSE map");

	return imgs

def preprocess(bse, display):
	blurred = skimage.filters.gaussian(bse, sigma=1)

	filtered = skimage.restoration.denoise_bilateral(bse, sigma_color=0.05, sigma_spatial=5)
	filtered = skimage.restoration.denoise_bilateral(filtered, sigma_color=0.05, sigma_spatial=5)
	filtered = skimage.restoration.denoise_bilateral(filtered, sigma_color=0.05, sigma_spatial=5)
	filtered = skimage.restoration.denoise_bilateral(filtered, sigma_color=0.05, sigma_spatial=5)

	if display:
		plot_single(filtered, 'Filtered BSE', cmap="gist_grey")

	return filtered

def build_mask(img, display, distance=30):
	# Discard the first part of the histogram since it's only noise
	pruned = np.array(img)
	pruned[img < 70 / 255] = 0

	if display:
		show_some_histogram(pruned, -1)

	# Extract the peaks
	hist, _ = np.histogram(pruned, bins=256, range=(0, 1))

	# Ajuster 'distance' pour éviter les pics trop proches
	peaks, _ = scipy.signal.find_peaks(hist, distance=distance)

	print(peaks)
	bseC = np.full(img.shape, 1)

	for i, peak in enumerate(peaks):
	    minPeak = peak - distance / 2.0
	    maxPeak = peak + distance / 2.0
	    mask = (img > (minPeak / 255.0)) & (img < (maxPeak / 255.0))

	    bseC[mask] = i

	if display:
		plt.figure(figsize=(10,6))
		plt.imshow(bseC, cmap='tab10')
		plt.colorbar(label='0 = type I, 1 = Matrice, 2 = Type II, 3 = ?, 4 = Dense')
		plt.show()

	return bseC

def lissage(display=False):
	imgs = load(display)

	bse = imgs["BSE"].astype("float") / 255

	filtered = preprocess(bse, display)

	mask = build_mask(filtered, display)

	mask = skimage.morphology.remove_small_objects(mask == 0, 50)

	# Display only type I
	if display:
		plot_single(mask * imgs["BSE"], "Mask type I applied over BSE map")
		plot_single((1 - mask) * imgs["BSE"], "BSE map excluded by the mask")

lissage(True)