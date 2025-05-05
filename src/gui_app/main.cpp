#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);  // << 1. Musí byť úplne hore

    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();    
}
