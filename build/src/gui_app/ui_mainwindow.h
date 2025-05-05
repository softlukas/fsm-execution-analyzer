/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "gui_app/GraphicsView.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    GraphicsView *graphicsView;
    QPushButton *addOutputButton;
    QGroupBox *trackingOutputsGroupBox;
    QPushButton *mainMenuButton;
    QPushButton *editFieldsButton;
    QGroupBox *editInGroupBox;
    QPushButton *addInputButton;
    QGroupBox *editVarGroupBox;
    QPushButton *addVariableButton;
    QGroupBox *menuGroupBox;
    QPushButton *addStateButton;
    QPushButton *addTransitionButton;
    QPushButton *saveJsonButton;
    QPushButton *loadJsonButton;
    QPushButton *runAutomatButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        graphicsView = new GraphicsView(centralwidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(290, 20, 471, 311));
        addOutputButton = new QPushButton(centralwidget);
        addOutputButton->setObjectName(QString::fromUtf8("addOutputButton"));
        addOutputButton->setGeometry(QRect(720, 350, 41, 41));
        trackingOutputsGroupBox = new QGroupBox(centralwidget);
        trackingOutputsGroupBox->setObjectName(QString::fromUtf8("trackingOutputsGroupBox"));
        trackingOutputsGroupBox->setGeometry(QRect(290, 390, 471, 161));
        mainMenuButton = new QPushButton(centralwidget);
        mainMenuButton->setObjectName(QString::fromUtf8("mainMenuButton"));
        mainMenuButton->setGeometry(QRect(20, 20, 111, 41));
        editFieldsButton = new QPushButton(centralwidget);
        editFieldsButton->setObjectName(QString::fromUtf8("editFieldsButton"));
        editFieldsButton->setGeometry(QRect(150, 20, 101, 41));
        editInGroupBox = new QGroupBox(centralwidget);
        editInGroupBox->setObjectName(QString::fromUtf8("editInGroupBox"));
        editInGroupBox->setGeometry(QRect(20, 70, 261, 241));
        addInputButton = new QPushButton(editInGroupBox);
        addInputButton->setObjectName(QString::fromUtf8("addInputButton"));
        addInputButton->setGeometry(QRect(210, 20, 41, 41));
        editVarGroupBox = new QGroupBox(centralwidget);
        editVarGroupBox->setObjectName(QString::fromUtf8("editVarGroupBox"));
        editVarGroupBox->setGeometry(QRect(20, 340, 261, 191));
        addVariableButton = new QPushButton(editVarGroupBox);
        addVariableButton->setObjectName(QString::fromUtf8("addVariableButton"));
        addVariableButton->setGeometry(QRect(210, 20, 41, 41));
        menuGroupBox = new QGroupBox(centralwidget);
        menuGroupBox->setObjectName(QString::fromUtf8("menuGroupBox"));
        menuGroupBox->setGeometry(QRect(10, 80, 261, 1091));
        addStateButton = new QPushButton(menuGroupBox);
        addStateButton->setObjectName(QString::fromUtf8("addStateButton"));
        addStateButton->setGeometry(QRect(10, 70, 241, 41));
        addTransitionButton = new QPushButton(menuGroupBox);
        addTransitionButton->setObjectName(QString::fromUtf8("addTransitionButton"));
        addTransitionButton->setGeometry(QRect(10, 130, 241, 41));
        saveJsonButton = new QPushButton(menuGroupBox);
        saveJsonButton->setObjectName(QString::fromUtf8("saveJsonButton"));
        saveJsonButton->setGeometry(QRect(10, 250, 241, 41));
        loadJsonButton = new QPushButton(menuGroupBox);
        loadJsonButton->setObjectName(QString::fromUtf8("loadJsonButton"));
        loadJsonButton->setGeometry(QRect(10, 310, 241, 41));
        runAutomatButton = new QPushButton(centralwidget);
        runAutomatButton->setObjectName(QString::fromUtf8("runAutomatButton"));
        runAutomatButton->setGeometry(QRect(310, 340, 111, 41));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        addOutputButton->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        trackingOutputsGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Tracking outputs", nullptr));
        mainMenuButton->setText(QCoreApplication::translate("MainWindow", "Main menu", nullptr));
        editFieldsButton->setText(QCoreApplication::translate("MainWindow", "Edit Fields", nullptr));
        editInGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Edit Inputs", nullptr));
        addInputButton->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        editVarGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Edit variables", nullptr));
        addVariableButton->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        menuGroupBox->setTitle(QCoreApplication::translate("MainWindow", "Menu", nullptr));
        addStateButton->setText(QCoreApplication::translate("MainWindow", "Add State", nullptr));
        addTransitionButton->setText(QCoreApplication::translate("MainWindow", "Add Transition", nullptr));
        saveJsonButton->setText(QCoreApplication::translate("MainWindow", "Save json", nullptr));
        loadJsonButton->setText(QCoreApplication::translate("MainWindow", "Load json", nullptr));
        runAutomatButton->setText(QCoreApplication::translate("MainWindow", "Run Automat", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
