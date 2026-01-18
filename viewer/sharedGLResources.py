from PyQt5.QtWidgets import QOpenGLWidget
from OpenGL.GL import *
from enum import Enum

import numpy as np
import cv2

from viewer.logger import Logger

class SharedTexture(Enum):
    PREDICTION = "pred"
    SEGMENTATION = "segment"

class SharedGLResources(QOpenGLWidget):
    def __init__(self, width, height, parent=None, data=None):
        super().__init__(parent)
        self.width_ = width
        self.height_ = height
        self.vbo = None
        self.ebo = None
        self.fbo = None
        self.fbo_texture = None
        self.pred_texture = None
        self.init = False
        self.data = data
        self.setFixedSize(width, height)

        self.log = Logger("SharedGLResources").logger

    def initializeGL(self):
        if self.init:
            return

        self.vbo = self.create_vbo()
        self.ebo = self.create_ebo()
        self.fbo, self.fbo_texture = self.create_fbo(self.width_, self.height_)
        self.pred_texture = self.create_texture_2d(self.width_, self.height_)
        self.segment_texture = self.create_texture_2d(self.width_, self.height_)

        self.init = True

    def get_texture(self, texture: SharedTexture):
        if texture == SharedTexture.PREDICTION:
            return self.pred_texture

        return self.segment_texture

    def create_vbo(self):
        vertices = np.array([
            # Position     # TexCoords
            -1.0, -1.0,    0.0, 0.0,
             1.0, -1.0,    1.0, 0.0,
             1.0,  1.0,    1.0, 1.0,
            -1.0,  1.0,    0.0, 1.0
        ], dtype=np.float32)

        vbo = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, vbo)
        glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)
        glBindBuffer(GL_ARRAY_BUFFER, 0)
        return vbo

    def check_gl_error(self):
        err = glGetError()
        if err != GL_NO_ERROR:
            self.log.error(f"OpenGL error {err}")

    def create_ebo(self):
        indices = np.array([0, 1, 2, 2, 3, 0], dtype=np.uint32)
        ebo = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.nbytes, indices, GL_STATIC_DRAW)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)
        return ebo

    def create_fbo(self, width, height):
        self.makeCurrent()

        fbo = glGenFramebuffers(1)
        glBindFramebuffer(GL_FRAMEBUFFER, fbo)

        tex = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D, tex)

        label_path = self.label_path
        img_data = None

        if label_path is not None:
            img = cv2.imread(label_path, cv2.IMREAD_GRAYSCALE)
            if img is None:
                self.log.error(f"Can't read labels from {label_path}")
            else:
                img_data = np.ascontiguousarray(img, dtype=np.uint8)
                img_data = np.flipud(img_data)
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, img_data)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0)

        if img_data is None:
            glClearColor(0.0, 0.0, 0.0, 1.0)
            glClear(GL_COLOR_BUFFER_BIT)
        else:
            self.log.info(f"FBO texture loaded from {label_path}")

        glBindFramebuffer(GL_FRAMEBUFFER, 0)
        glBindTexture(GL_TEXTURE_2D, 0)
        glFlush()

        self.doneCurrent()

        self.log.info("FBO texture created")

        return fbo, tex

    def create_texture_2d(self, width, height):
        tex = glGenTextures(1)

        glBindTexture(GL_TEXTURE_2D, tex)
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, None)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        glBindTexture(GL_TEXTURE_2D, 0)

        return tex

    def update_texture_region(self, x, y, w, h, sub_img: np.ndarray, texture: SharedTexture):
        if sub_img.dtype != np.uint8 or sub_img.ndim != 3 or sub_img.shape[2] != 3:
            raise ValueError("sub_img doit être uint8 et [h, w, 3]")

        self.makeCurrent()

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)

        tex = self.get_texture(texture)

        glBindTexture(GL_TEXTURE_2D, tex)
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
                        GL_RGB, GL_UNSIGNED_BYTE, sub_img)
        glBindTexture(GL_TEXTURE_2D, 0)

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4)

        self.doneCurrent()

    def save_fbo_texture_to_file(self, filename):
        self.makeCurrent()

        glBindFramebuffer(GL_FRAMEBUFFER, self.fbo)
        glReadBuffer(GL_COLOR_ATTACHMENT0)

        pixels = glReadPixels(0, 0, self.width_, self.height_, GL_RED, GL_UNSIGNED_BYTE)
        img_data = np.frombuffer(pixels, dtype=np.uint8).reshape(self.height_, self.width_)
        img_data = np.flipud(img_data)

        cv2.imwrite(filename, img_data)

        glBindFramebuffer(GL_FRAMEBUFFER, 0)
        self.doneCurrent()
        self.log.info(f"FBO texture saved to {filename}")
