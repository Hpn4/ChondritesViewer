#pragma once

#include "ImageTransform.h"
#include "modules/PaintVImage.h"
#include "modules/PaintLabel.h"
#include "modules/ImageLabel.h"
#include "SharedGLResources.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <vips/vips8>
#include <memory>
#include <vector>

using namespace vips;

class ImageView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit ImageView(std::shared_ptr<ImageTransform> sharedTransform = nullptr,
                       QWidget *parent = nullptr,
                       SharedGLResources *sharedRes = nullptr);
    ~ImageView() override;

    void addModule(std::unique_ptr<ModuleBase> m) { modules.push_back(std::move(m)); }

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void wheelEvent(QWheelEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

public slots:
    void onLabelSelected(int index);

private:
    int width_;
    int height_;

    SharedGLResources *sharedRes_;

    QMatrix4x4 transform_;
    QMatrix4x4 projection_;

    QPoint lastMouse_;
    float zoom_ = 1.0f;
    std::shared_ptr<ImageTransform> sharedTransform_;

    std::vector<std::unique_ptr<ModuleBase>> modules;
};
