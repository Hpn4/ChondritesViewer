#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include "SyncController.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QWidget *central_;
    QGridLayout *grid_;
    SyncController sync_;
};

