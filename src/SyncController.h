#pragma once

#include <QObject>
#include <vector>
#include "ImageView.h"

class SyncController : public QObject {
    Q_OBJECT
public:
    void registerView(ImageView *v);
private slots:
    void onTransformChanged(const QTransform &t);
private:
    std::vector<ImageView*> views_;
};

