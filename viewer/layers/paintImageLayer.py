import numpy as np
from OpenGL.GL import *
from viewer.layers.layer import Layer

class PaintImageLayer(Layer):
    """
    Layer responsible to render a numpy image. Either in grayscale or in RGB
    """
    
    def __init__(self, image: np.ndarray, grayscale: bool = True, name : str = "", shared_res=None):
        super().__init__(name, shared_res)
        self.image = image
        self.grayscale = grayscale
        self.texture = None
        self.vao = None

    def initialize_gl(self):
        self.shared_res.initializeGL()

        vert_name = "image.vert"
        frag_name = "gray.frag" if self.grayscale else "image.frag"
        self.init_shaders(vert_name, frag_name)

        self.init_geometry()
        self.upload_texture()

    def init_geometry(self):
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)

        glBindBuffer(GL_ARRAY_BUFFER, self.shared_res.vbo)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.shared_res.ebo)

        FLOAT_SIZE = 4

        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * FLOAT_SIZE, ctypes.c_void_p(0))
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * FLOAT_SIZE, ctypes.c_void_p(2 * FLOAT_SIZE))

        glBindVertexArray(0)

    def upload_texture(self):
        if self.texture is None:
            self.texture = glGenTextures(1)

        glBindTexture(GL_TEXTURE_2D, self.texture)

        img_data = np.ascontiguousarray(self.image, dtype=np.uint8)
        h, w = img_data.shape[:2]
        format_ = GL_RED if self.grayscale else GL_RGB

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1)
        glTexImage2D(GL_TEXTURE_2D, 0, format_, w, h, 0, format_, GL_UNSIGNED_BYTE, img_data)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
        glBindTexture(GL_TEXTURE_2D, 0)

    def paint_gl(self, transform):
        glUseProgram(self.program)
        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.texture)

        tex_loc = glGetUniformLocation(self.program, "tex")
        glUniform1i(tex_loc, 0)

        transform_loc = glGetUniformLocation(self.program, "transform")
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, transform.data())

        glBindVertexArray(self.vao)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, None)
        glBindVertexArray(0)
        glUseProgram(0)
