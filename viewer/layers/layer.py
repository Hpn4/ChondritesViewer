from abc import ABC
from OpenGL.GL import GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
from OpenGL.GL.shaders import compileProgram, compileShader
from importlib import resources

from viewer.sharedGLResources import SharedGLResources

class Layer(ABC):
    """
    Base classe for a Layer.

    Gaves methods to override to handle OpenGL context and mouse events
    """

    def __init__(self, name: str, shared_res: SharedGLResources):
        self.shared_res = shared_res
        self.visible = True
        self.name = name
        self.program = None  # Shader program

    def initialize_gl(self):
        """Where to setup OpenGL stuff (to override)"""
        pass

    def paint_gl(self, transform):
        """
        Where to render stuff (to override)

        :param transform: 4x4 view matrix
        """
        pass

    def mouse_press_event(self, x: float, y: float):
        pass

    def mouse_move_event(self, x: float, y: float):
        pass

    def mouse_release_event(self):
        pass

    def init_shaders(self, vert_name: str, frag_name: str):
        """
        Compile and link shaders with the program
        """
        vert = resources.files("resources.shaders") / vert_name
        frag = resources.files("resources.shaders") / frag_name

        self.program = compileProgram(
            compileShader(vert.read_text(), GL_VERTEX_SHADER),
            compileShader(frag.read_text(), GL_FRAGMENT_SHADER)
        )
