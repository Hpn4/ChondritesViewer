#include "SharedGLResources.h"

#include <QDebug>

SharedGLResources::SharedGLResources(int width, int height, QWidget *parent)
    : QOpenGLWidget(parent), width_(width), height_(height) {}

SharedGLResources::~SharedGLResources() {
    makeCurrent();
        
    if (vbo) {
        vbo->destroy();
        delete vbo;
        vbo = nullptr;
    }

    if (ebo) {
        ebo->destroy();
        delete ebo;
        ebo = nullptr;
    }

    if (fbo) {
        delete ebo;
        fbo = nullptr;
    }
        
    doneCurrent();
}

void SharedGLResources::initializeGL() {
    initializeOpenGLFunctions();

    // 4. VBO / EBO partagés
    GLfloat vertices[] = {
        // Position    // TexCoords
        -1.f, -1.f,    0.f, 0.f,
         1.f, -1.f,    1.f, 0.f,
         1.f,  1.f,    1.f, 1.f,
        -1.f,  1.f,    0.f, 1.f
    };
    
    GLuint indices[] = {0, 1, 2, 2, 3, 0};
    
    vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    if (!vbo->create()) {
        qDebug() << "Failed to create VBO";
        doneCurrent();
        return;
    }
    vbo->bind();
    vbo->allocate(vertices, sizeof(vertices));
    vbo->release();
    
    ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    if (!ebo->create()) {
        qDebug() << "Failed to create EBO";
        doneCurrent();
        return;
    }
    ebo->bind();
    ebo->allocate(indices, sizeof(indices));
    ebo->release();

    QOpenGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    fmt.setTextureTarget(GL_TEXTURE_2D);
    fmt.setInternalTextureFormat(GL_R8);
    fbo = new QOpenGLFramebufferObject(width_, height_, fmt);

    // clear
    fbo->bind();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    fbo->release();

    glFlush();
    
    qDebug() << "SharedGLResources initialized successfully";
}
