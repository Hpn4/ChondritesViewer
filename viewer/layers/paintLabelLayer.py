from OpenGL.GL import *
from viewer.layers.layer import Layer

class PaintLabelLayer(Layer):
    def __init__(self, name, shared_res):
        super().__init__(name, shared_res)
        self.stroke_active = False
        self.vao = None

    # ---------------- OpenGL lifecycle ----------------
    def initialize_gl(self):
        self.shared_res.initializeGL()

        # Compiler les shaders
        self.init_shaders("paint/brush.vert", "paint/brush.frag")

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

    # ---------------- Mouse events ----------------
    def mouse_press_event(self, x: float, y: float):
        self.stroke_active = True
        self.paint_circle(x, y)

    def mouse_move_event(self, x: float, y: float):
        if self.stroke_active:
            self.paint_circle(x, y)

    def mouse_release_event(self):
        self.stroke_active = False

    # ---------------- Painting ----------------
    def paint_circle(self, x: float, y: float):
        glUseProgram(self.program)

        # Binder le FBO partagé
        glBindFramebuffer(GL_FRAMEBUFFER, self.shared_res.fbo)

        height = self.shared_res.height_

        glViewport(0, 0, self.shared_res.width_, height)

        # uniforms
        loc_center = glGetUniformLocation(self.program, "center")
        glUniform2f(loc_center, x, height - y)

        loc_radius = glGetUniformLocation(self.program, "radius")
        glUniform1f(loc_radius, self.shared_res.data["brush_size"])

        loc_label = glGetUniformLocation(self.program, "label")
        glUniform1f(loc_label, float(self.shared_res.data["selected_label"]))

        # Dessin
        glBindVertexArray(self.vao)
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, None)
        glBindVertexArray(0)

        glBindFramebuffer(GL_FRAMEBUFFER, 0)
        glUseProgram(0)

    # ---------------- Cleanup ----------------
    def destroy(self):
        if self.vao:
            glDeleteVertexArrays(1, [self.vao])
            self.vao = None
