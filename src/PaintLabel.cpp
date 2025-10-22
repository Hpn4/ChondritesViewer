#include "PaintLabel.h"
#include <QDebug>

PaintLabel::PaintLabel(int width, int height)
    : texW_(width), texH_(height)
{}

PaintLabel::~PaintLabel() {
    vbo_.destroy();
    ebo_.destroy();
    vao_.destroy();
    fbo_.reset();
}

void PaintLabel::initGL() {
    if(glInitialized_) return;

    initializeOpenGLFunctions();

    // Shader vertex + fragment pour cercle
    const char* vertSrc = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;
        void main() { gl_Position = vec4(aPos,0.0,1.0); }
    )";

    const char* fragSrc = R"(
        #version 330 core
        uniform vec2 center;   // position cercle en pixels
        uniform float radius;
        uniform float label;
        out float FragColor;
        void main() {
            vec2 uv = gl_FragCoord.xy;
            float d = distance(uv, center);
            if(d < radius)
                FragColor = 1.0;
            else
                discard;
        }
    )";

    paintProg_.addShaderFromSourceCode(QOpenGLShader::Vertex, vertSrc);
    paintProg_.addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc);
    paintProg_.link();

    if (!paintProg_.isLinked())
        qDebug() << "Shader link failed:" << paintProg_.log();

    GLfloat vertices[] = {
        -1.f,-1.f, 0.f,0.f,
         1.f,-1.f, 1.f,0.f,
         1.f, 1.f, 1.f,1.f,
        -1.f, 1.f, 0.f,1.f
    };
    GLuint indices[] = {0,1,2, 2,3,0};
    
    vao_.create();
    vao_.bind();
    
    vbo_.create();
    vbo_.bind();
    vbo_.allocate(vertices, sizeof(vertices));
    
    ebo_.create();
    ebo_.bind();
    ebo_.allocate(indices, sizeof(indices));

    paintProg_.enableAttributeArray(0);
    paintProg_.setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(float));
    paintProg_.enableAttributeArray(1);
    paintProg_.setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    vao_.release();

    // FBO R8
    QOpenGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QOpenGLFramebufferObject::NoAttachment);
    fmt.setTextureTarget(GL_TEXTURE_2D);
    fmt.setInternalTextureFormat(GL_R8);
    fbo_ = std::make_unique<QOpenGLFramebufferObject>(texW_, texH_, fmt);

    // clear
    fbo_->bind();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    fbo_->release();

    glInitialized_ = true;
}

void PaintLabel::mousePressEvent(float x, float y, QOpenGLVertexArrayObject& vao) {
    strokeActive_ = true;
    paintCircle(x, y, vao);
}

void PaintLabel::mouseMoveEvent(float x, float y, QOpenGLVertexArrayObject& vao) {
    if(strokeActive_)
        paintCircle(x, y, vao);
}

void PaintLabel::mouseReleaseEvent() {
    strokeActive_ = false;
}

void PaintLabel::paintCircle(float x, float y, QOpenGLVertexArrayObject& vao) {
    if(!glInitialized_) return;
    
    // Vérifications AVANT le draw
    qDebug() << "=== DEBUG DRAW ===";
    qDebug() << "VAO ID:" << vao_.objectId();
    qDebug() << "VBO ID:" << vbo_.bufferId();
    qDebug() << "EBO ID:" << ebo_.bufferId();
    qDebug() << "EBO type:" << ebo_.type();
    qDebug() << "FBO valid:" << fbo_->isValid();
    
    // Vérifier ce qui est bindé
    GLint boundVAO = 0, boundEBO = 0, boundVBO = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundVBO);
    qDebug() << "Before bind - VAO:" << boundVAO << "EBO:" << boundEBO << "VBO:" << boundVBO;
    
    paintProg_.bind();
    fbo_->bind();
    glViewport(0,0,texW_,texH_);
    
    paintProg_.setUniformValue("center", QVector2D(x, texH_-y));
    paintProg_.setUniformValue("radius", brushRadius_);
    paintProg_.setUniformValue("label", float(brushLabel_));
    
    vao.bind();
    
    // Vérifier après bind
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEBO);
    qDebug() << "After VAO bind - VAO:" << boundVAO << "EBO:" << boundEBO;
    
    qDebug() << "About to draw...";
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    qDebug() << "Draw done!";
    
    GLenum err = glGetError();
    qDebug() << "GL Error after draw:" << err;
    
    vao.release();
    paintProg_.release();
    fbo_->release();
}