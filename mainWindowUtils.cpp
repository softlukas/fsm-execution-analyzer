#include <string>
#include <QString>
#include <QInputDialog>
#include <QDebug>

#include "mainWindowUtils.h"

std::string MainWindowUtils::ProccessOneArgumentDialog(const std::string& textToDisplay) {

    
    bool okClicked;
    QString input = QInputDialog::getText(nullptr, "Enter " + QString::fromStdString(textToDisplay),
                                          QString::fromStdString(textToDisplay), QLineEdit::Normal,
                                          "", &okClicked);
    if (!okClicked || input.trimmed().isEmpty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return nullptr; // Return an empty string if cancelled or no input
    }
    return input.trimmed().toStdString();
}




