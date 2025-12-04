#include "PaintLabel.h"

#include <QDebug>

PaintLabel::PaintLabel(SharedGLResources* sharedRes)
    : ModuleBase(sharedRes)
{}

PaintLabel::~PaintLabel() {
    vao_.destroy();
}

void PaintLabel::initializeGL() {
    initializeOpenGLFunctions();

    initShaders("resources/shaders/paint/brush.vert", "resources/shaders/paint/brush.frag");
    
    vao_.create();
    vao_.bind();

    sharedRes_->getVBO()->bind();
    sharedRes_->getEBO()->bind();

    program_.enableAttributeArray(0);
    program_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(float));

    vao_.release();
}

void PaintLabel::mousePressEvent(float x, float y) {
    strokeActive_ = true;
    paintCircle(x, y);
}

void PaintLabel::mouseMoveEvent(float x, float y) {
    if(strokeActive_)
        paintCircle(x, y);
}

void PaintLabel::mouseReleaseEvent() {
    strokeActive_ = false;
}

void PaintLabel::paintCircle(float x, float y) {
    program_.bind();
    sharedRes_->getFBO()->bind();

    int height = sharedRes_->texHeight();

    glViewport(0, 0, sharedRes_->texWidth(), height);

    program_.setUniformValue("center", QVector2D(x, height - y));
    program_.setUniformValue("radius", brushRadius_);
    program_.setUniformValue("label", float(brushLabel_));
    
    vao_.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    vao_.release();

    sharedRes_->getFBO()->release();
    program_.release();
}