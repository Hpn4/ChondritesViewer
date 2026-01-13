from abc import ABC
from OpenGL.GL import GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
from OpenGL.GL.shaders import compileProgram, compileShader
from importlib import resources

class Layer(ABC):
    """
    Classe de base pour un layer OpenGL.
    Fournit les méthodes à surcharger pour gérer le cycle de vie OpenGL
    et les interactions souris.
    """

    def __init__(self, name, shared_res):
        """
        :param shared_res: instance de SharedGLResources
        """
        self.shared_res = shared_res
        self.visible = True
        self.name = name
        self.program = None  # Shader program

    # ---------------- OpenGL lifecycle ----------------
    def initialize_gl(self):
        """Initialisation OpenGL (à surcharger)"""
        pass

    def paint_gl(self, transform):
        """
        Rendu OpenGL (à surcharger)
        :param transform: matrice 4x4
        """
        pass

    # ---------------- Mouse events ----------------
    def mouse_press_event(self, x: float, y: float):
        pass

    def mouse_move_event(self, x: float, y: float):
        pass

    def mouse_release_event(self):
        pass

    # ---------------- Shader helper ----------------
    def init_shaders(self, vert_name: str, frag_name: str):
        """
        Méthode utilitaire pour compiler et lier des shaders.
        """
        vert = resources.files("resources.shaders") / vert_name
        frag = resources.files("resources.shaders") / frag_name

        self.program = compileProgram(
            compileShader(vert.read_text(), GL_VERTEX_SHADER),
            compileShader(frag.read_text(), GL_FRAGMENT_SHADER)
        )
