#include "ImageView.h"

#include <QDebug>

ImageView::ImageView(std::shared_ptr<ImageTransform> sharedTransform,
                     QWidget *parent,
                     SharedGLResources *sharedRes)
    : QOpenGLWidget(parent),
      sharedTransform_(sharedTransform),
      sharedRes_(sharedRes)
{
    zoom_ = 1.0f;
    transform_.setToIdentity();

    width_ = sharedRes->texWidth();
    height_ = sharedRes->texHeight();

    if (sharedTransform_) {
        connect(sharedTransform_.get(), &ImageTransform::transformChanged, this, [this]() {
            this->transform_ = sharedTransform_->data.transform;
            this->zoom_ = sharedTransform_->data.zoom;
            this->update();
        });
    }
}

ImageView::~ImageView() {
    makeCurrent();
    doneCurrent();
}

void ImageView::onLabelSelected(int index) {
    // paintLabel_.setBrushLabel(index);
}

void ImageView::initializeGL() {
    initializeOpenGLFunctions();

    for (auto& m : modules)
        m->initializeGL();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    qDebug() << "ImageView init";
}

void ImageView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    QMatrix4x4 finalTransform = projection_ * transform_;

    for (auto& m : modules)
        m->paintGL(finalTransform);
}

void ImageView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    float widgetAspect = float(w) / float(h);
    float imageAspect = float(width_) / float(height_);

    projection_.setToIdentity();
    if (widgetAspect > imageAspect) {
        projection_.ortho(-widgetAspect / imageAspect, widgetAspect / imageAspect, -1, 1, -1, 1);
    } else {
        projection_.ortho(-1, 1, -imageAspect / widgetAspect, imageAspect / widgetAspect, -1, 1);
    }
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
    // Coord souris [0,1]
    float u = float(e->pos().x()) / width();
    float v = float(e->pos().y()) / height();

    // NDC [-1,1]
    QVector4D mouseNDC(2*u-1, 1-2*v, 0.f, 1.f);

    // Coord texture pixels
    QMatrix4x4 finalTransform = projection_ * transform_;
    
    QVector4D texNDC = finalTransform.inverted() * mouseNDC;
    float texX = (texNDC.x() + 1)*0.5f * width_;
    float texY = (1.f - (texNDC.y() + 1)*0.5f) * height_;

    makeCurrent();

    for (auto& m : modules)
        m->mousePressEvent(texX, texY);

    doneCurrent();

    sharedTransform_->setTransform(transform_, zoom_);

    lastMouse_ = e->pos();
}

void ImageView::mouseMoveEvent(QMouseEvent *e) {
    makeCurrent();
    //paintLabel_.mouseMoveEvent(e->pos().x(), e->pos().y());
    doneCurrent();

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
    for (auto& m : modules)
        m->mouseReleaseEvent();
}
