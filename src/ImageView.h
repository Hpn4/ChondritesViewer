#pragma once

#include "ImageTransform.h"
#include "PaintLabel.h"
#include "ImageLabel.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
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
                       QWidget *parent = nullptr);
    ~ImageView() override;

    void setPaintLabel(std::shared_ptr<PaintLabel> paintLabel) { paintLabel_ = paintLabel; }

    void setImageLabel(std::shared_ptr<ImageLabel> imageLabel) { imageLabel_ = imageLabel; }

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void wheelEvent(QWheelEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    const VImage image_;
    QOpenGLShaderProgram program_;
    QOpenGLTexture texture_{QOpenGLTexture::Target2D};
    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vbo_{QOpenGLBuffer::VertexBuffer};
    QOpenGLBuffer ebo_{QOpenGLBuffer::IndexBuffer};

    bool grayscale_;
    QMatrix4x4 transform_;
    QPoint lastMouse_;
    float zoom_ = 1.0f;
    std::shared_ptr<ImageTransform> sharedTransform_;

    std::shared_ptr<PaintLabel> paintLabel_;
    std::shared_ptr<ImageLabel> imageLabel_;

    void uploadTexture();
    void initShaders();
    void initGeometry();
};
