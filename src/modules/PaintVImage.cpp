#include "PaintVImage.h"

#include <QDebug>
#include <QFile>

PaintVImage::PaintVImage(const VImage& image,
                     bool grayscale,
                     SharedGLResources *sharedRes)
    : ModuleBase(sharedRes),
      image_(image),
      grayscale_(grayscale)
{ }

PaintVImage::~PaintVImage() {
    texture_.destroy();
    vao_.destroy();
}

void PaintVImage::initializeGL() {
    initializeOpenGLFunctions();

    // Init shaders
    QString fragPath = grayscale_ ? "resources/shaders/gray.frag" : "resources/shaders/image.frag";

    initShaders("resources/shaders/image.vert", fragPath);

    initGeometry();
    uploadTexture();
}

void PaintVImage::initGeometry() {
    vao_.create();
    vao_.bind();

    sharedRes_->getVBO()->bind();
    sharedRes_->getEBO()->bind();

    program_.enableAttributeArray(0);
    program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(float));
    program_.enableAttributeArray(1);
    program_.setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    vao_.release();
}

void PaintVImage::uploadTexture() {
    texture_.destroy();
    texture_.create();
    texture_.setFormat(grayscale_ ? QOpenGLTexture::R8_UNorm : QOpenGLTexture::RGB8_UNorm);
    texture_.setSize(image_.width(), image_.height());
    texture_.allocateStorage();

    void* data = image_.write_to_memory(nullptr);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    texture_.setData(grayscale_ ? QOpenGLTexture::Red : QOpenGLTexture::RGB, QOpenGLTexture::UInt8, data);
    g_free(data);

    texture_.setMinificationFilter(QOpenGLTexture::Nearest);
    texture_.setMagnificationFilter(QOpenGLTexture::Nearest);
    texture_.setWrapMode(QOpenGLTexture::ClampToEdge);
}

void PaintVImage::paintGL(const QMatrix4x4& transform) {
    program_.bind();
    texture_.bind(0);

    program_.setUniformValue("tex", 0);
    program_.setUniformValue("transform", transform);

    vao_.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    vao_.release();

    program_.release();
}
