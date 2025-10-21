#include "PaintLabel.h"
#include "ImageTransform.h"
#include <QOpenGLShader>
#include <QDebug>
#include <cmath>

PaintLabel::PaintLabel(int width, int height, std::shared_ptr<ImageTransform> sharedTransform)
    : texW_(width),
      texH_(height),
      sharedTransform_(sharedTransform)
{ }

PaintLabel::~PaintLabel() {
    if (!glInitialized_) return;
    //makeCurrent();

    vbo_.destroy();
    vao_.destroy();
    labelTex_.destroy();
    fbo_.reset();

    //doneCurrent();
}

bool PaintLabel::initGL() {
    if (glInitialized_) return true;

    initializeOpenGLFunctions();

    // Shader program
    if (!paintProg_.addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shaders/paint/paint_brush.vert") ||
        !paintProg_.addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shaders/paint/paint_brush.frag") ||
        !paintProg_.link())
    {
        qWarning() << "PaintLabel shader error:" << paintProg_.log();
        return false;
    }

    // Quad VAO + VBO
    vao_.create();
    vao_.bind();

    vbo_.create();
    vbo_.bind();
    vbo_.allocate(4 * 2 * sizeof(float)); // 4 verts x 2 floats
    int posLoc = paintProg_.attributeLocation("aPos");
    paintProg_.enableAttributeArray(posLoc);
    paintProg_.setAttributeBuffer(posLoc, GL_FLOAT, 0, 2, 2 * sizeof(float));
    vao_.release();

    // Label texture
    labelTex_.create();
    labelTex_.setSize(texW_, texH_);
    labelTex_.setFormat(QOpenGLTexture::R8_UNorm);
    labelTex_.allocateStorage();
    labelTex_.setMinificationFilter(QOpenGLTexture::Nearest);
    labelTex_.setMagnificationFilter(QOpenGLTexture::Nearest);

    // FBO
    QOpenGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    fmt.setTextureTarget(GL_TEXTURE_2D);
    fmt.setInternalTextureFormat(GL_R8);
    fbo_ = std::make_unique<QOpenGLFramebufferObject>(texW_, texH_, fmt);

    // Clear initial FBO
    fbo_->bind();
    const GLuint clearVal[4] = {0,0,0,0};
    glClearBufferuiv(GL_COLOR, 0, clearVal);
    fbo_->release();

    glInitialized_ = true;
    return true;
}

PaintLabel::Point PaintLabel::screenToTexel(const QPointF& screenPos) const {
    if (!sharedTransform_) return {0,0};
    const auto& t = sharedTransform_->data.transform;
    QMatrix4x4 inv = t.inverted();
    QVector4D v(screenPos.x(), screenPos.y(), 0.0f, 1.0f);
    QVector4D img = inv * v;
    int tx = qBound(0, int(std::round(img.x())), texW_-1);
    int ty = qBound(0, int(std::round(img.y())), texH_-1);
    return {tx, ty};
}

void PaintLabel::mousePressEvent(const QPointF& screenPos) {
    if (!glInitialized_) return;
    strokeActive_ = true;
    paintStampToFBO(screenToTexel(screenPos), brush_.label, brush_.radius);
}

void PaintLabel::mouseMoveEvent(const QPointF& screenPos) {
    if (!strokeActive_) return;
    paintStampToFBO(screenToTexel(screenPos), brush_.label, brush_.radius);
}

void PaintLabel::mouseReleaseEvent(const QPointF&) {
    strokeActive_ = false;
}

void PaintLabel::paintStampToFBO(const Point& center, unsigned int label, float radius) {
    if (!labelTex_.isCreated()) return;

    int minx = qBound(0, int(center.x - radius), texW_ - 1);
    int maxx = qBound(0, int(center.x + radius), texW_ - 1);
    int miny = qBound(0, int(center.y - radius), texH_ - 1);
    int maxy = qBound(0, int(center.y + radius), texH_ - 1);

    GLfloat quad[8] = {
        (GLfloat)minx, (GLfloat)miny,
        (GLfloat)maxx, (GLfloat)miny,
        (GLfloat)minx, (GLfloat)maxy,
        (GLfloat)maxx, (GLfloat)maxy
    };

    fbo_->bind();
    glViewport(0, 0, texW_, texH_);

    vao_.bind();
    vbo_.bind();
    vbo_.write(0, quad, sizeof(quad));

    paintProg_.bind();
    paintProg_.setUniformValue("uResolution", QVector2D((float)texW_, (float)texH_));
    paintProg_.setUniformValue("uCenter", QVector2D((float)center.x, (float)center.y));
    paintProg_.setUniformValue("uRadius", radius);
    paintProg_.setUniformValue("uLabel", static_cast<int>(label));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    paintProg_.release();

    fbo_->release();
}
