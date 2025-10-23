#pragma once

#include "SharedGLResources.h"

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <memory>

class PaintLabel : protected QOpenGLFunctions_3_3_Core {
public:
    PaintLabel(int width, int height, SharedGLResources* sharedRes);
    ~PaintLabel();

    void initGL(); // à appeler après contexte OpenGL actif

    void setBrushLabel(uint8_t label) { brushLabel_ = label; }
    void setBrushRadius(float r) { brushRadius_ = r; }

    void mousePressEvent(float x, float y);
    void mouseMoveEvent(float x, float y);
    void mouseReleaseEvent();

private:
    void paintCircle(float x, float y);

    SharedGLResources* sharedRes_;

    int texW_, texH_;
    bool strokeActive_ = false;

    float brushRadius_ = 100.0f;
    uint8_t brushLabel_ = 1;

    QOpenGLShaderProgram paintProg_;
    QOpenGLVertexArrayObject vao_;
};
