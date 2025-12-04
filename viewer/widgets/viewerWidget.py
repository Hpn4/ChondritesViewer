import numpy as np

from PyQt5.QtWidgets import QOpenGLWidget
from PyQt5.QtGui import QMatrix4x4, QVector3D
from PyQt5.QtCore import Qt
from PyQt5.QtCore import QPointF
from OpenGL.GL import *

class SharedTransform:
    """
    Transform partagé entre plusieurs vues.
    """
    def __init__(self):
        self.transform = QMatrix4x4()
        self.zoom = 1.0
        self.callbacks = []

    def set_transform(self, transform: QMatrix4x4, zoom: float):
        self.transform = transform
        self.zoom = zoom
        for cb in self.callbacks:
            cb()

    def on_change(self, callback):
        self.callbacks.append(callback)

class ViewerWidget(QOpenGLWidget):
    """
    QOpenGLWidget qui gère plusieurs layers avec un transform partagé.
    """
    def __init__(self, shared_transform: SharedTransform = None, parent=None, width=1, height=1):
        super().__init__(parent)
        self.layers = []
        self.shared_transform = shared_transform
        self.zoom = 1.0
        self.transform = QMatrix4x4()
        self.projection = QMatrix4x4()
        self.width_ = width
        self.height_ = height
        self.last_mouse = QPointF()
        self.pinned = False

        if self.shared_transform:
            self.shared_transform.on_change(self.on_shared_transform_changed)

    def on_shared_transform_changed(self):
        self.transform = self.shared_transform.transform
        self.zoom = self.shared_transform.zoom
        self.update()

    def add_layer(self, layer):
        self.layers.append(layer)

    def initializeGL(self):
        for layer in self.layers:
            layer.initialize_gl()
        glClearColor(0.1, 0.1, 0.1, 1.0)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)
        final_transform = self.projection * self.transform

        for layer in self.layers:
            if not layer.visible:
                continue
            layer.paint_gl(final_transform)

    def resizeGL(self, w: int, h: int):
        glViewport(0, 0, w, h)
        widget_aspect = w / h
        image_aspect = self.width_ / self.height_

        self.projection.setToIdentity()
        if widget_aspect > image_aspect:
            self.projection.ortho(-widget_aspect / image_aspect, widget_aspect / image_aspect,
                                  -1, 1, -1, 1)
        else:
            self.projection.ortho(-1, 1,
                                  -image_aspect / widget_aspect, image_aspect / widget_aspect,
                                  -1, 1)

    def wheelEvent(self, event):
        delta = event.angleDelta().y() / 800.0
        factor = 1.0 + delta

        mouse_pos = event.position()
        nx = 2.0 * mouse_pos.x() / self.width() - 1.0
        ny = 1.0 - 2.0 * mouse_pos.y() / self.height()

        col3 = self.transform.column(3)
        ox = col3.x()
        oy = col3.y()

        nx_ = nx - (nx - ox) * factor
        ny_ = ny - (ny - oy) * factor

        t = QMatrix4x4()
        t.setToIdentity()
        t.translate(nx_, ny_, 0)
        t.scale(self.zoom * factor, self.zoom * factor, 1.0)

        new_zoom = self.zoom * factor

        if self.shared_transform:
            self.shared_transform.set_transform(t, new_zoom)
        else:
            self.transform = t
            self.zoom = new_zoom
            self.update()

    def convertCoord(self, x, y):
        u = x / self.width()
        v = y / self.height()

        mouse_ndc = np.array([2 * u - 1, 1 - 2 * v, 0, 1], dtype=np.float32)

        final_transform = self.projection * self.transform
        inverted, invertible = final_transform.inverted()
        if invertible:
            tex_ndc = inverted.map(QVector3D(mouse_ndc[0], mouse_ndc[1], 0))
            tex_x = (tex_ndc.x() + 1) * 0.5 * self.width_
            tex_y = (1 - (tex_ndc.y() + 1) * 0.5) * self.height_

            return tex_x, tex_y

        return None, None

    def mousePressEvent(self, event):
        self.last_mouse = event.pos()

        if self.window().space_pressed:
            self.pinned = True
            return

        tx, ty = self.convertCoord(event.pos().x(), event.pos().y())

        if tx is not None and ty is not None:
            self.makeCurrent()
            for layer in self.layers:
                layer.mouse_press_event(tx, ty)
            self.doneCurrent()
            self.shared_transform.set_transform(self.transform, self.zoom)

    def mouseMoveEvent(self, event):
        if self.pinned:
            d = event.pos() - self.last_mouse
            self.last_mouse = event.pos()

            inverted, invertible = self.transform.inverted()
            if not invertible:
                return

            delta_scene = inverted.mapVector(QVector3D(d.x(), d.y(), 0))

            t = QMatrix4x4(self.transform)
            t.translate(delta_scene.x() / 200, -delta_scene.y() / 200, 0)

            if self.shared_transform:
                self.shared_transform.set_transform(t, self.zoom)
            else:
                self.transform = t
                self.update()

            return

        tx, ty = self.convertCoord(event.pos().x(), event.pos().y())

        if tx is not None and ty is not None:
            self.makeCurrent()
            for layer in self.layers:
                layer.mouse_move_event(tx, ty)
            self.doneCurrent()
            self.shared_transform.set_transform(self.transform, self.zoom)

    def mouseReleaseEvent(self, event):
        for layer in self.layers:
            layer.mouse_release_event()
        self.pinned = False
