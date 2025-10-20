#include "ImageView.h"

#include <QDebug>
#include <QFile>

ImageView::ImageView(const VImage& image,
                     bool grayscale,
                     std::shared_ptr<ImageTransform> sharedTransform,
                     QWidget *parent)
    : QOpenGLWidget(parent),
      image_(image),
      grayscale_(grayscale),
      sharedTransform_(sharedTransform)
{
    zoom_ = 1.0f;
    transform_.setToIdentity();

    if (sharedTransform_) {
        connect(sharedTransform_.get(), &ImageTransform::transformChanged, this, [this]() {
            this->transform_ = sharedTransform_->data.transform;
            this->zoom_ = sharedTransform_->data.zoom;
            this->update();
        });
    }
}

void ImageView::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.2f,0.2f,0.2f,1.0f);

    GLfloat vertices[] = {
        -1.f,-1.f, 0.f,0.f,
         1.f,-1.f, 1.f,0.f,
         1.f, 1.f, 1.f,1.f,
        -1.f, 1.f, 0.f,1.f
    };
    GLuint indices[] = {0,1,2, 2,3,0};

    glGenVertexArrays(1,&vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1,&vbo_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    glGenBuffers(1,&ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    QString vertPath = "resources/shaders/image.vert";
    QString fragPath = grayscale_ ? "resources/shaders/gray.frag" : "resources/shaders/image.frag";

    QFile vertFile(vertPath);
    QFile fragFile(fragPath);
    if(!vertFile.open(QFile::ReadOnly) || !fragFile.open(QFile::ReadOnly)) {
        qDebug() << "Failed to open shader files";
        return;
    }

    QString vertSrc = vertFile.readAll();
    QString fragSrc = fragFile.readAll();

    program_.addShaderFromSourceCode(QOpenGLShader::Vertex, vertSrc);
    program_.addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc);
    program_.link();
    if(!program_.isLinked())
        qDebug() << "Shader link failed:" << program_.log();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)(2*sizeof(float)));

    uploadTexture();
}

void ImageView::uploadTexture() {
    if(texture_) delete texture_;

    texture_ = new QOpenGLTexture(QOpenGLTexture::Target2D);
    texture_->setFormat(grayscale_ ? QOpenGLTexture::R8_UNorm : QOpenGLTexture::RGB8_UNorm);
    texture_->setSize(image_.width(), image_.height());
    texture_->allocateStorage();

    // Load the image
    void* data = image_.write_to_memory(nullptr);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture_->setData(grayscale_ ? QOpenGLTexture::Red : QOpenGLTexture::RGB, QOpenGLTexture::UInt8, data);

    g_free(data); // Free CPU side

    texture_->setMinificationFilter(QOpenGLTexture::Nearest);
    texture_->setMagnificationFilter(QOpenGLTexture::Nearest);
    texture_->setWrapMode(QOpenGLTexture::ClampToEdge);
}

void ImageView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    program_.bind();
    texture_->bind(0);
    program_.setUniformValue("tex", 0);
    program_.setUniformValue("transform", transform_);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ImageView::resizeGL(int w,int h) {
    glViewport(0,0,w,h);
}

void ImageView::wheelEvent(QWheelEvent *e)
{
    float delta = e->angleDelta().y() / 800.0f;
    float factor = 1.0f + delta;

    // Position de la souris en pixels
    QPointF mousePos = e->position();

    // Convertir en coordonnées normalisées [-1,1]
    float nx = 2.0f * mousePos.x() / width() - 1.0f;
    float ny = 1.0f - 2.0f * mousePos.y() / height();

    // Extraire la translation actuelle
    float ox = transform_(0,3);
    float oy = transform_(1,3);

    // Calculer la nouvelle translation pour garder le point sous la souris fixe
    float nx_ = nx - (nx - ox) * factor;
    float ny_ = ny - (ny - oy) * factor;

    QMatrix4x4 t;
    t.setToIdentity();
    t.translate(nx_, ny_, 0);
    t.scale(zoom_ * factor, zoom_ * factor, 1.0f);

    float newZoom = zoom_ * factor;

    if (sharedTransform_) {
        sharedTransform_->setTransform(t, newZoom);
    } else {
        transform_ = t;
        zoom_ = newZoom;
        update();
    }
}

void ImageView::mousePressEvent(QMouseEvent *e)
{
    lastMouse_ = e->pos();
}

void ImageView::mouseMoveEvent(QMouseEvent *e)
{
    QPointF d = e->pos() - lastMouse_;
    lastMouse_ = e->pos();

    // Convertir le delta écran en delta image
    QVector3D deltaScene = transform_.inverted().mapVector(QVector3D(d.x(), d.y(), 0));

    QMatrix4x4 t = transform_;
    t.translate(deltaScene.x() / 300, -deltaScene.y() / 300, 0);

    if (sharedTransform_) {
        sharedTransform_->setTransform(t, zoom_);
    } else {
        transform_ = t;
        update();
    }
}