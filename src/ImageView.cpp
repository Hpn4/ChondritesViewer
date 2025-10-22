#include "ImageView.h"

#include <QDebug>
#include <QFile>

ImageView::ImageView(const VImage& image,
                     bool grayscale,
                     std::shared_ptr<ImageTransform> sharedTransform,
                     QWidget *parent,
                     SharedGLResources *sharedRes)
    : QOpenGLWidget(parent),
      image_(image),
      grayscale_(grayscale),
      sharedTransform_(sharedTransform),
      sharedRes_(sharedRes),
      imageLabel_(sharedRes)
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

    // IMPORTANT : Partager le contexte OpenGL
    QSurfaceFormat fmt;
    fmt.setVersion(4, 6);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);
}

ImageView::~ImageView() {
    makeCurrent();

    texture_.destroy();
    vao_.destroy();

    doneCurrent();
}

void ImageView::initializeGL() {
    initializeOpenGLFunctions();

    imageLabel_.initialize();

    if (paintLabel_)
    {
        paintLabel_->initGL();
        imageLabel_.setLabelTexture(paintLabel_->labelTexture());
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    initShaders();
    initGeometry();
    uploadTexture();
}

void ImageView::initShaders() {
    QString vertPath = "resources/shaders/image.vert";
    QString fragPath = grayscale_ ? "resources/shaders/gray.frag" : "resources/shaders/image.frag";

    bool read = program_.addShaderFromSourceFile(QOpenGLShader::Vertex, vertPath);
    read &= program_.addShaderFromSourceFile(QOpenGLShader::Fragment, fragPath);

    if (!read) {
        qDebug() << "Failed to open shader files";
        return;
    }

    program_.link();
    if (!program_.isLinked())
        qDebug() << "Shader link failed:" << program_.log();
}

void ImageView::initGeometry() {
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

void ImageView::uploadTexture() {
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

void ImageView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program_.bind();
    texture_.bind(0);
    program_.setUniformValue("tex", 0);
    program_.setUniformValue("transform", transform_);

    vao_.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    vao_.release();

    imageLabel_.draw(transform_);

    //glDisable(GL_BLEND);
}

void ImageView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void ImageView::wheelEvent(QWheelEvent *e) {
    float delta = e->angleDelta().y() / 800.0f;
    float factor = 1.0f + delta;

    QPointF mousePos = e->position();
    float nx = 2.0f * mousePos.x() / width() - 1.0f;
    float ny = 1.0f - 2.0f * mousePos.y() / height();

    float ox = transform_(0, 3);
    float oy = transform_(1, 3);

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

void ImageView::mousePressEvent(QMouseEvent *e) {
    if (paintLabel_)
    {
        // Coord souris [0,1]
        float u = float(e->pos().x()) / width();
        float v = float(e->pos().y()) / height();

        // NDC [-1,1]
        QVector4D mouseNDC(2*u-1, 1-2*v, 0.f, 1.f);

        // Coord texture pixels
        QVector4D texNDC = transform_.inverted() * mouseNDC;
        float texX = (texNDC.x() + 1)*0.5f * image_.width();
        float texY = (1.f - (texNDC.y() + 1)*0.5f) * image_.height();

        makeCurrent();
        paintLabel_->mousePressEvent(texX, texY, vao_);
        doneCurrent();

        sharedTransform_->setTransform(transform_, zoom_);
    }

    lastMouse_ = e->pos();
}

void ImageView::mouseMoveEvent(QMouseEvent *e) {
    if (paintLabel_)
    {
        makeCurrent();
        paintLabel_->mouseMoveEvent(e->pos().x(), e->pos().y(), vao_);
        doneCurrent();

        // Force redraw of all views
        sharedTransform_->setTransform(transform_, zoom_);
    }

    QPointF d = e->pos() - lastMouse_;
    lastMouse_ = e->pos();

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

void ImageView::mouseReleaseEvent(QMouseEvent *e) {
    if (paintLabel_)
        paintLabel_->mouseReleaseEvent();
}
