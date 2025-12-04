#include "ModuleBase.h"

#include <QDebug>

void ModuleBase::initShaders(QString vertPath, QString fragPath) {
    bool read = program_.addShaderFromSourceFile(QOpenGLShader::Vertex, vertPath);
    read &= program_.addShaderFromSourceFile(QOpenGLShader::Fragment, fragPath);

    if (!read) {
        qDebug() << "Failed to open shader files";
        return;
    }

    program_.link();
    if (!program_.isLinked())
        qDebug() << "Shader link failed:" << program_.log();
}
