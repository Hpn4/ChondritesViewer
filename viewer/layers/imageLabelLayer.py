from OpenGL.GL import *
from viewer.layers.layer import Layer

class ImageLabelLayer(Layer):
    def __init__(self, name, shared_res):
        super().__init__(name, shared_res)
        self.vao = None
        self.label_tex = None
        self.alpha = 1.0
        self.enabled = True

    # ---------------- OpenGL lifecycle ----------------
    def initialize_gl(self):
        # Compiler les shaders
        self.init_shaders("paint/overlay.vert", "paint/overlay.frag")

        # Créer et configurer le VAO
        self.vao = glGenVertexArrays(1)
        glBindVertexArray(self.vao)

        # Bind des buffers partagés
        glBindBuffer(GL_ARRAY_BUFFER, self.shared_res.vbo)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.shared_res.ebo)

        # Attributs vertex : pos (0), texcoord (1)
        stride = 4 * 4  # 4 floats par vertex, 4 bytes par float
        glEnableVertexAttribArray(0)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(0))
        glEnableVertexAttribArray(1)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, ctypes.c_void_p(2 * 4))

        glBindVertexArray(0)

        # Récupérer la texture du FBO partagé
        self.label_tex = self.shared_res.fbo_texture

    def paint_gl(self, transform):
        if not self.enabled or self.label_tex is None:
            return

        glUseProgram(self.program)

        # Bind de la texture
        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, self.label_tex)

        # uniforms
        loc_tex = glGetUniformLocation(self.program, "u_labelTex")
        glUniform1i(loc_tex, 0)

        loc_alpha = glGetUniformLocation(self.program, "u_alpha")
        glUniform1f(loc_alpha, self.alpha)

        # Si transform est un numpy array 4x4
        loc_transform = glGetUniformLocation(self.program, "u_transform")
        glUniformMatrix4fv(loc_transform, 1, GL_FALSE, transform.data())

        # Dessin
        glBindVertexArray(self.vao)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, None)
        glBindVertexArray(0)

        glUseProgram(0)

    # ---------------- Cleanup ----------------
    def destroy(self):
        if self.vao:
            glDeleteVertexArrays(1, [self.vao])
            self.vao = None
