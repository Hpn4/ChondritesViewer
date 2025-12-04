#pragma once

#include "../SharedGLResources.h"
#include "ModuleBase.h"

#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <vips/vips8>

using namespace vips;

class PaintVImage : public ModuleBase {
public:
    explicit PaintVImage(const VImage& image,
                       bool grayscale = false,
                       SharedGLResources *sharedRes = nullptr);
    ~PaintVImage();

    void initializeGL() override;
    void paintGL(const QMatrix4x4& transform) override;

private:
    const VImage image_;

    QOpenGLTexture texture_{QOpenGLTexture::Target2D};
    QOpenGLVertexArrayObject vao_;

    bool grayscale_;

    void uploadTexture();
    void initGeometry();
};
