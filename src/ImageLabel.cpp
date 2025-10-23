#include "ImageLabel.h"
#include <QVector>

ImageLabel::ImageLabel(SharedGLResources *sharedRes) 
    : sharedRes_(sharedRes)
{ }

ImageLabel::~ImageLabel() {
    vao_.destroy();
}

void ImageLabel::initialize() {
    initializeOpenGLFunctions();

    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shaders/paint/overlay.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shaders/paint/overlay.frag");
    program_.link();
    if (!program_.isLinked())
        qDebug() << "Shader link failed:" << program_.log();

    // VAO
    vao_.create();
    vao_.bind();

    sharedRes_->getVBO()->bind();
    sharedRes_->getEBO()->bind();

    program_.enableAttributeArray(0);
    program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(float));
    program_.enableAttributeArray(1);
    program_.setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    vao_.release();

    labelTex_ = sharedRes_->getFBO()->texture();
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

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    vao_.release();
    program_.release();
}
