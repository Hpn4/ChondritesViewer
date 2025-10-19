#include "ImageView.h"
#include <QDebug>
#include <QFile>

ImageView::ImageView(const std::string &imagePath, QWidget *parent)
    : QOpenGLWidget(parent),
      loader_(std::make_unique<ImageTileLoader>(imagePath))
{
    zoom_ = 1.0f;
    transform_.setToIdentity();
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
    QString fragPath = "resources/shaders/image.frag";

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
    texture_->setFormat(QOpenGLTexture::RGB8_UNorm);
    texture_->setSize(loader_->width(), loader_->height());
    texture_->allocateStorage();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    texture_->setData(QOpenGLTexture::RGB, QOpenGLTexture::UInt8, loader_->data());

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
    if (!loader_) return;

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

    transform_.setToIdentity();
    transform_.translate(nx_, ny_, 0);
    transform_.scale(zoom_ * factor, zoom_ * factor, 1.0f);

    zoom_ *= factor;

    update();
}

void ImageView::mousePressEvent(QMouseEvent *e)
{
    lastMouse_ = e->pos();
}

void ImageView::mouseMoveEvent(QMouseEvent *e)
{
    if (!loader_) return;

    QPointF d = e->pos() - lastMouse_;
    lastMouse_ = e->pos();

    // Convertir le delta écran en delta image
    QVector3D deltaScene = transform_.inverted().mapVector(QVector3D(d.x(), d.y(), 0));

    transform_.translate(deltaScene.x() / 300, -deltaScene.y() / 300, 0);
    update();
}