#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
