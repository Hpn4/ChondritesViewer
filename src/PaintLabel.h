#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QPointF>
#include <memory>

class ImageTransform;

class PaintLabel : protected QOpenGLFunctions_3_3_Core {
public:
    struct Point { int x, y; };
    struct Brush { unsigned int label = 1; float radius = 5.0f; };

    explicit PaintLabel(int width, int height, std::shared_ptr<ImageTransform> sharedTransform);
    ~PaintLabel();

    bool initGL();

    void mousePressEvent(const QPointF& screenPos);
    void mouseMoveEvent(const QPointF& screenPos);
    void mouseReleaseEvent(const QPointF& screenPos);

    void setBrushLabel(unsigned int label) { brush_.label = label; }
    void setBrushRadius(float r) { brush_.radius = r; }

    GLuint labelTexture() const { return labelTex_.isCreated() ? labelTex_.textureId() : 0; }

private:
    Point screenToTexel(const QPointF& screenPos) const;
    void paintStampToFBO(const Point& center, unsigned int label, float radius);

private:
    bool glInitialized_ = false;
    int texW_ = 0;
    int texH_ = 0;
    bool strokeActive_ = false;

    Brush brush_;
    std::shared_ptr<ImageTransform> sharedTransform_;

    // Modern OpenGL objects
    std::unique_ptr<QOpenGLFramebufferObject> fbo_;
    QOpenGLShaderProgram paintProg_;
    QOpenGLTexture labelTex_{QOpenGLTexture::Target2D};
    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vbo_{QOpenGLBuffer::VertexBuffer};
};
