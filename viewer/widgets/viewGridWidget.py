from PyQt5.QtWidgets import QWidget, QGridLayout
from viewer.layers import ImageLabelLayer, PaintLabelLayer, PaintImageLayer, ImagePredLayer
from viewer.widgets.viewerWidget import ViewerWidget, SharedTransform
from viewer.sharedGLResources import SharedGLResources, SharedTexture

class ViewGridWidget(QWidget):
    def __init__(self, loader, shared_res=None, parent=None):
        super().__init__(parent)

        grid_layout = QGridLayout(self)
        grid_layout.setContentsMargins(0, 0, 0, 0)
        grid_layout.setSpacing(5)

        shared_transform = SharedTransform()

        self.viewers = []

        views = loader.prepare_views()
        if not views:
            raise RuntimeError("Aucune vue à afficher !")

        for x, y, img, grayscale in views:
            viewer = ViewerWidget(shared_transform=shared_transform, width=img.shape[1], height=img.shape[0])
            
            viewer.add_layer(PaintImageLayer(img, grayscale, "EDS", shared_res=shared_res))
            viewer.add_layer(PaintLabelLayer("Painting", shared_res=shared_res))
            viewer.add_layer(ImageLabelLayer("Labeling", shared_res=shared_res))
            viewer.add_layer(ImagePredLayer("Prediction", SharedTexture.PREDICTION, shared_res=shared_res))
            viewer.add_layer(ImagePredLayer("Segmentation", SharedTexture.SEGMENTATION, shared_res=shared_res))
            
            grid_layout.addWidget(viewer, y, x)
            self.viewers.append(viewer)

    def update_view(self):
        for viewer in self.viewers:
            viewer.update()