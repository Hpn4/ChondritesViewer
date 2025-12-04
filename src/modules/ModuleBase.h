#pragma once

#include "../SharedGLResources.h"

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QString>

class ModuleBase : protected QOpenGLFunctions_3_3_Core {
public:
    virtual ~ModuleBase() = default;

    virtual void initializeGL() { }
    virtual void paintGL(const QMatrix4x4& transform) { }

    virtual void mousePressEvent(float x, float y) { }
    virtual void mouseMoveEvent(float x, float y) { }
    virtual void mouseReleaseEvent() { }

    void initShaders(QString vertPath, QString fragPath);

protected:
    ModuleBase(SharedGLResources *sharedRes)
        : sharedRes_(sharedRes) {}

    SharedGLResources *sharedRes_;
    QOpenGLShaderProgram program_;
};
