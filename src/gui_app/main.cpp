/**
 
@file main.cpp
@brief Entry point for the GUI application.
@details This file contains the main function, which initializes the QApplication,
sets up the main window, and starts the event loop.
@authors xsimonl00, xsiaket00
@date Last modified: 2025-05-05*/


#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>

QFile logFile;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(&logFile);
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString typeStr;

    switch (type) {
    case QtDebugMsg:    typeStr = "DEBUG"; break;
    case QtWarningMsg:  typeStr = "WARNING"; break;
    case QtCriticalMsg: typeStr = "CRITICAL"; break;
    case QtFatalMsg:    typeStr = "FATAL"; break;
    }

    out << "[" << time << "] [" << typeStr << "] " << msg << "\n";
    out.flush();

    if (type == QtFatalMsg) abort();
}

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    QDir().mkpath("logs");
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString logFilePath = "logs/debug" + timestamp + ".log";
    logFile.setFileName(logFilePath);
    logFile.open(QIODevice::Append | QIODevice::Text);
    qInstallMessageHandler(customMessageHandler);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}