#pragma once

#include "ImageTransform.h"
#include "PaintLabel.h"
#include "ImageLabel.h"
#include "SharedGLResources.h"

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <vips/vips8>
#include <memory>

using namespace vips;

class ImageView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit ImageView(const VImage& image,
                       bool grayscale = false,
                       std::shared_ptr<ImageTransform> sharedTransform = nullptr,
                       QWidget *parent = nullptr,
                       SharedGLResources *sharedRes = nullptr);
    ~ImageView() override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void wheelEvent(QWheelEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

public slots:
    void onLabelSelected(int index);

private:
    SharedGLResources *sharedRes_;

    const VImage image_;
    QOpenGLShaderProgram program_;
    QOpenGLTexture texture_{QOpenGLTexture::Target2D};
    QOpenGLVertexArrayObject vao_;

    bool grayscale_;
    QMatrix4x4 transform_;
    QMatrix4x4 projection_;

    QPoint lastMouse_;
    float zoom_ = 1.0f;
    std::shared_ptr<ImageTransform> sharedTransform_;

    PaintLabel paintLabel_;
    ImageLabel imageLabel_;

    void uploadTexture();
    void initShaders();
    void initGeometry();
};
