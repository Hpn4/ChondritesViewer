#include "ImageLabel.h"
#include <QVector>

ImageLabel::ImageLabel() { }

ImageLabel::~ImageLabel() {
    vao_.destroy();
    vbo_.destroy();
}

void ImageLabel::initialize() {
    if (glInitialized_) return;

    initializeOpenGLFunctions();

    // Quad plein écran (NDC)
    GLfloat vertices[] = {
        -1.f, -1.f,
         1.f, -1.f,
        -1.f,  1.f,
         1.f,  1.f
    };

    // VAO
    vao_.create();
    vao_.bind();

    // VBO
    vbo_.create();
    vbo_.bind();
    vbo_.allocate(vertices, sizeof(vertices));

    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shaders/paint/overlay.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shaders/paint/overlay.frag");
    program_.link();
    if (!program_.isLinked())
        qDebug() << "Shader link failed:" << program_.log();

    program_.enableAttributeArray(0);
    program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 2 * sizeof(GLfloat));

    vao_.release();
    vbo_.release();

    glInitialized_ = true;
}

void ImageLabel::draw(const QMatrix4x4& transform) {
    if (!enabled_ || labelTex_ == 0)
        return;

    program_.bind();
    vao_.bind();

    program_.setUniformValue("u_transform", transform);
    program_.setUniformValue("u_alpha", alpha_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, labelTex_);
    program_.setUniformValue("u_labelTex", 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);
    vao_.release();
    program_.release();
}
