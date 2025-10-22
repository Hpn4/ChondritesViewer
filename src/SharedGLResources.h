#pragma once

#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>

class SharedGLResources : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit SharedGLResources(QWidget *parent = nullptr);
    ~SharedGLResources();

    // Accès aux ressources
    QOpenGLBuffer* getVBO() const { return vbo; }
    QOpenGLBuffer* getEBO() const { return ebo; }
    
protected:
    void initializeGL() override;
    
private:
    QOpenGLBuffer* vbo = nullptr;
    QOpenGLBuffer* ebo = nullptr;
};
