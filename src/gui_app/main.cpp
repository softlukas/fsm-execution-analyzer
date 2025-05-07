/**
 * @file main.cpp
 * @brief Entry point for the GUI application.
 * @details This file contains the main function, which initializes the QApplication,
 * sets up the main window, and starts the event loop.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */


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
