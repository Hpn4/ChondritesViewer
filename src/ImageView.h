#pragma once

#include "ImageTransform.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>
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

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void wheelEvent(QWheelEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:
    const VImage image_;
    QOpenGLShaderProgram program_;
    GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
    QOpenGLTexture* texture_ = nullptr;

    bool grayscale_;

    // Transform related private variables
    QMatrix4x4 transform_;
    QPoint lastMouse_;
    float zoom_ = 1.0f;

    std::shared_ptr<ImageTransform> sharedTransform_;

    void uploadTexture();
};
