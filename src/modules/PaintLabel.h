#pragma once

#include "../SharedGLResources.h"
#include "ModuleBase.h"

#include <QOpenGLVertexArrayObject>

class PaintLabel : public ModuleBase {
public:
    PaintLabel(SharedGLResources* sharedRes);
    ~PaintLabel();

    void initializeGL() override;

    void mousePressEvent(float x, float y) override;
    void mouseMoveEvent(float x, float y) override;
    void mouseReleaseEvent() override;

    void setBrushLabel(uint8_t label) { brushLabel_ = label; }
    void setBrushRadius(float r) { brushRadius_ = r; }

private:
    void paintCircle(float x, float y);

    bool strokeActive_ = false;

    float brushRadius_ = .6f;
    uint8_t brushLabel_ = 1;

    QOpenGLVertexArrayObject vao_;
};
