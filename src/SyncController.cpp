#include "SyncController.h"

void SyncController::registerView(ImageView *v) {
    views_.push_back(v);
    // QObject::connect(v, &ImageView::transformChanged, this, &SyncController::onTransformChanged);
}

void SyncController::onTransformChanged(const QTransform &t) {
    //for (auto *v : views_) v->transformChanged(t);
}

