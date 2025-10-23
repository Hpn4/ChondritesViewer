#include <QApplication>
#include <QFile>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication app(argc, argv);

    QFile f("resources/style/style.css");
    if (f.open(QFile::ReadOnly)) {
        qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
    }

    MainWindow w;
    w.show();
    return app.exec();
}
