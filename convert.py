
labels = [
    ("Chondre I", "#577277"),
    ("Chondre II", "#468232"),
    ("Matrix", "#c65197"),
    ("CAIs", "#73bed3"),
    ("Carbonate", "#a8ca58"),
    ("Soufre", "#e7d5b3"),
    ("Fe Oxydé", "#7a367b"),
    ("Fe", "#a53030"),
    ("Epoxy", "#da863e")
]

for name,col in labels:
	r = int(col[1:3],16) / 255.0
	g = int(col[3:5],16) / 255.0
	b = int(col[5:7],16) / 255.0

	print(f"\tvec3({r}, {g}, {b}),")