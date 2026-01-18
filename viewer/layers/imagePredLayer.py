from OpenGL.GL import *
from viewer.layers.layer import Layer
from viewer.sharedGLResources import SharedTexture

class ImagePredLayer(Layer):
    """
    Layer responsible to draw outputs of the prediction and segmentation step.
    """

    def __init__(self, name, texture: SharedTexture, shared_res):
        super().__init__(name, shared_res)
        self.vao = None
        self.label_tex = None
        self.enabled = True
        self.texture = texture

    def initialize_gl(self):
        self.init_shaders("image.vert", "image-trans.frag")

        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)

        glBindBuffer(GL_ARRAY_BUFFER, self.shared_res.vbo)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.shared_res.ebo)

        stride = 4 * 4
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(0))
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(2 * 4))

        glBindVertexArray(0)

        self.label_tex = self.shared_res.get_texture(self.texture)

    def paint_gl(self, transform):
        if not self.enabled or self.label_tex is None:
            return

        glUseProgram(self.program)

        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.label_tex)

        loc_tex = glGetUniformLocation(self.program, "tex")
        glUniform1i(loc_tex, 0)

        loc_transform = glGetUniformLocation(self.program, "transform")
        glUniformMatrix4fv(loc_transform, 1, GL_FALSE, transform.data())

        glBindVertexArray(self.vao)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, None)
        glBindVertexArray(0)

        glUseProgram(0)

    def destroy(self):
        if self.vao:
            glDeleteVertexArrays(1, [self.vao])
            self.vao = None
