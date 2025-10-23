#include "PaintLabel.h"
#include <QDebug>

PaintLabel::PaintLabel(int width, int height, SharedGLResources* sharedRes)
    : texW_(width), texH_(height), sharedRes_(sharedRes)
{}

PaintLabel::~PaintLabel() {
    vao_.destroy();
}

void PaintLabel::initGL() {
    initializeOpenGLFunctions();

    paintProg_.addShaderFromSourceFile(QOpenGLShader::Vertex, "resources/shaders/paint/brush.vert");
    paintProg_.addShaderFromSourceFile(QOpenGLShader::Fragment, "resources/shaders/paint/brush.frag");
    paintProg_.link();

    if (!paintProg_.isLinked())
        qDebug() << "Shader link failed:" << paintProg_.log();
    
    vao_.create();
    vao_.bind();

    sharedRes_->getVBO()->bind();
    sharedRes_->getEBO()->bind();

    paintProg_.enableAttributeArray(0);
    paintProg_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(float));
    paintProg_.enableAttributeArray(1);
    paintProg_.setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

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
    paintProg_.bind();
    sharedRes_->getFBO()->bind();
    glViewport(0,0,texW_,texH_);
    
    paintProg_.setUniformValue("center", QVector2D(x, texH_-y));
    paintProg_.setUniformValue("radius", brushRadius_);
    paintProg_.setUniformValue("label", float(brushLabel_));
    
    vao_.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    vao_.release();

    paintProg_.release();
    sharedRes_->getFBO()->release();
}