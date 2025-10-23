#pragma once

#include "SharedGLResources.h"

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

class ImageLabel : protected QOpenGLFunctions_3_3_Core {
public:
    ImageLabel(SharedGLResources *sharedRes);
    ~ImageLabel();

    void initialize();
    void draw(const QMatrix4x4& transform);

    void setAlpha(float a) { alpha_ = a; }
    void setEnabled(bool e) { enabled_ = e; }

private:
    SharedGLResources* sharedRes_;

    GLuint labelTex_ = 0;

    QOpenGLVertexArrayObject vao_;
    QOpenGLShaderProgram program_;

    float alpha_ = 0.5f;
    bool enabled_ = true;
};
