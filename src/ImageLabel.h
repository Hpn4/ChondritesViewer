#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

class ImageLabel : protected QOpenGLFunctions_3_3_Core {
public:
    ImageLabel();
    ~ImageLabel();

    void initialize();
    void draw(const QMatrix4x4& transform, QOpenGLVertexArrayObject& vao);

    void setLabelTexture(GLuint tex) { labelTex_ = tex; }
    void setAlpha(float a) { alpha_ = a; }
    void setEnabled(bool e) { enabled_ = e; }
    void resize(int w, int h) { width_ = w; height_ = h; }

private:
    bool glInitialized_ = false;

    GLuint labelTex_ = 0;

    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vbo_{QOpenGLBuffer::VertexBuffer};
    QOpenGLShaderProgram program_;

    float alpha_ = 0.5f;
    bool enabled_ = true;
    int width_ = 0;
    int height_ = 0;
};
