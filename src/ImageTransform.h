#pragma once

#include <QObject>
#include <QMatrix4x4>

struct TransformData {
    QMatrix4x4 transform;
    float zoom = 1.0f;
};

class ImageTransform : public QObject {
    Q_OBJECT
public:
    TransformData data;

    // Met à jour la transformation et notifie les viewers
    void setTransform(const QMatrix4x4 &t, float z) {
        data.transform = t;
        data.zoom = z;
        emit transformChanged();
    }

signals:
    void transformChanged();
};

