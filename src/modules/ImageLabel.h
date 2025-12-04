#pragma once

#include "../SharedGLResources.h"
#include "ModuleBase.h"

#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

class ImageLabel : public ModuleBase {
public:
    ImageLabel(SharedGLResources *sharedRes);
    ~ImageLabel();

    void initializeGL() override;
    void paintGL(const QMatrix4x4& transform) override;

    void setAlpha(float a) { alpha_ = a; }
    void setEnabled(bool e) { enabled_ = e; }

private:
    GLuint labelTex_ = 0;

    QOpenGLVertexArrayObject vao_;

    float alpha_ = 0.5f;
    bool enabled_ = true;
};
