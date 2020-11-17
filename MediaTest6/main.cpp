#include "mainwindow.h"
#include <QApplication>

#pragma comment(lib, "user32.lib")
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(media);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
