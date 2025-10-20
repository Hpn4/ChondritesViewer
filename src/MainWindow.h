#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QWidget *central_;
    QGridLayout *grid_;
};

