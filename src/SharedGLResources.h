#pragma once

#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>

class SharedGLResources : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit SharedGLResources(int width, int height, QWidget *parent = nullptr);
    ~SharedGLResources();

    QOpenGLBuffer* getVBO() const { return vbo; }
    QOpenGLBuffer* getEBO() const { return ebo; }

    QOpenGLFramebufferObject* getFBO() const { return fbo; }

    int texWidth() { return width_; }
    int texHeight() { return height_; }
    
protected:
    void initializeGL() override;
    
private:
    int width_;
    int height_;

    QOpenGLBuffer* vbo = nullptr;
    QOpenGLBuffer* ebo = nullptr;
    QOpenGLFramebufferObject* fbo = nullptr;
};
