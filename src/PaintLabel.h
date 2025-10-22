#pragma once
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <memory>

class PaintLabel : protected QOpenGLFunctions_3_3_Core {
public:
    PaintLabel(int width, int height);
    ~PaintLabel();

    void initGL(); // à appeler après contexte OpenGL actif

    void setBrushLabel(uint8_t label) { brushLabel_ = label; }
    void setBrushRadius(float r) { brushRadius_ = r; }

    void mousePressEvent(float x, float y, QOpenGLVertexArrayObject& vao);
    void mouseMoveEvent(float x, float y, QOpenGLVertexArrayObject& vao);
    void mouseReleaseEvent();

    GLuint labelTexture() const { return fbo_->texture(); } // texture R8 du FBO

private:
    void paintCircle(float x, float y, QOpenGLVertexArrayObject& vao);

    int texW_, texH_;
    bool strokeActive_ = false;
    bool glInitialized_ = false;

    float brushRadius_ = 100.0f;
    uint8_t brushLabel_ = 1;

    QOpenGLShaderProgram paintProg_;
    QOpenGLBuffer vbo_;
    QOpenGLBuffer ebo_{QOpenGLBuffer::IndexBuffer};
    QOpenGLVertexArrayObject vao_;
    std::unique_ptr<QOpenGLFramebufferObject> fbo_;
};
