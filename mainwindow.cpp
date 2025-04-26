// Needed includes:
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Machine.h"
#include "State.h"
#include "Transition.h" // Needed for creating Transition
#include "JsonCreator.h" // Needed for saving to JSON
#include "GraphicsView.h" // Needed for casting ui->graphicsView
#include <memory>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem> // For drawing lines
#include <QGraphicsDropShadowEffect> // For highlighting effect
#include <QBrush>
#include <QPen>
#include <QVariant>
#include <QString>
#include <QVBoxLayout> // Needed for QVBoxLayout
#include <QLabel> // Needed for QLabel
#include <QDialogButtonBox> // Needed for QDialogButtonBox
#include <QDebug>
#include <exception>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("IFA Automaton Tool (Step 3)");
    scene = new QGraphicsScene(this);

    // IMPORTANT: ui->graphicsView is now of type GraphicsView* due to promotion
    GraphicsView* gView = ui->graphicsView; // Use the correct type

    if(gView) {
        gView->setScene(scene);
        qDebug() << "Graphics scene created and assigned to graphicsView.";
        // --- Connect custom view's signal to our slot ---
        connect(gView, &GraphicsView::stateItemClicked, this, &MainWindow::handleStateClick);
        qDebug() << "Connected graphicsView stateItemClicked signal.";
    } else {
        qWarning() << "WARNING: QGraphicsView named 'graphicsView' not found or not promoted to GraphicsView in UI file.";
    }
    // Create a Machine object using the Machine constructor
    machine = new Machine("machine1");
    qDebug() << "Machine object created successfully.";
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_saveJsonButton_clicked() {
    qDebug() << "Save JSON button clicked.";
    // Save JSON file logic here
    // Use JsonCreator::saveToFile(machine, filename) to save the machine to a file
    // Example: JsonCreator::saveToFile(*machine, "machine.json");
    // Handle errors and display messages as needed
}


void MainWindow::on_loadJsonButton_clicked() {
    qDebug() << "Load JSON button clicked.";
    // Load JSON file logic here
    // Use JsonCreator::saveToFile(machine, filename) to save the machine to a file
    // Example: JsonCreator::saveToFile(*machine, "machine.json");
    // Handle errors and display messages as needed
}

void MainWindow::on_addVariableButton_clicked() {
    qDebug() << "Load JSON button clicked.";
    // Load JSON file logic here
    // Use JsonCreator::saveToFile(machine, filename) to save the machine to a file
    // Example: JsonCreator::saveToFile(*machine, "machine.json");
    // Handle errors and display messages as needed
}

void MainWindow::on_addInputButton_clicked() {
    qDebug() << "Load JSON button clicked.";
    // Load JSON file logic here
    // Use JsonCreator::saveToFile(machine, filename) to save the machine to a file
    // Example: JsonCreator::saveToFile(*machine, "machine.json");
    // Handle errors and display messages as needed
}

void MainWindow::on_addOutputButton_clicked() {
    qDebug() << "Load JSON button clicked.";
    // Load JSON file logic here
    // Use JsonCreator::saveToFile(machine, filename) to save the machine to a file
    // Example: JsonCreator::saveToFile(*machine, "machine.json");
    // Handle errors and display messages as needed
}








// ... on_addStateButton_clicked() should be the version that prompts for name/action ...
// (Make sure it uses QGraphicsItemGroup as shown previously)
void MainWindow::on_addStateButton_clicked() {
    
    QDialog dialog(this);
    dialog.setWindowTitle("Enter Transition Details");

    QString stateAction;
    QString stateName;

    QVBoxLayout layout(&dialog);

    QLabel label1("Enter state name:", &dialog);
    QLineEdit lineEdit1(&dialog);
    layout.addWidget(&label1);
    layout.addWidget(&lineEdit1);

    QLabel label2("Enter state action:", &dialog);
    QLineEdit lineEdit2(&dialog);
    layout.addWidget(&label2);
    layout.addWidget(&lineEdit2);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout.addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    

    if (dialog.exec() == QDialog::Accepted) {
        stateName = lineEdit1.text().trimmed();
        stateAction = lineEdit2.text().trimmed().toInt();

        if (stateName.isEmpty()) {
            qDebug() << "Dialog cancelled or empty input.";
            return;
        }

        qDebug() << "Transition condition:" << stateName;
        qDebug() << "Transition action:" << stateAction;
        // Use `condition` and `action` as needed
    } else {
        qDebug() << "Dialog cancelled.";
        return;
    }
    

    // --- 4. Create Visual Representation (Ellipse + Name) ---
    qreal width = 60; qreal height = 60;
    int itemCount = scene->items().count();
    qreal x = (itemCount % 5) * (width + 20);
    qreal y = (itemCount / 5) * (height + 20);

    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, width, height);
    ellipse->setBrush(QBrush(Qt::cyan));
    ellipse->setPen(QPen(Qt::black));

    QGraphicsTextItem *text = new QGraphicsTextItem(stateName);
    QRectF textRect = text->boundingRect();
    qreal textX = (width - textRect.width()) / 2;
    qreal textY = (height - textRect.height()) / 2;
    text->setPos(textX, textY);

    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    group->addToGroup(ellipse);
    group->addToGroup(text);
    group->setPos(x, y);
    group->setFlag(QGraphicsItem::ItemIsMovable);
    group->setFlag(QGraphicsItem::ItemIsSelectable);
    group->setData(0, QVariant(stateName));

    scene->addItem(group);

    std::unique_ptr<State> newState = std::make_unique<State>(stateName.toStdString(), stateAction.toStdString());

    // Add the state to the automaton
    try {
        machine->addState(std::move(newState));
        qDebug() << "State added to Automaton model:" << stateName << "Action:" << stateAction;
    } catch (const std::exception& e) {
        qCritical() << "Error adding state to automaton:" << e.what();
        QMessageBox::critical(this, "Model Error", QString("Failed to add state to model: %1").arg(e.what()));
        scene->removeItem(group); // Remove the visual representation if adding to the model fails
        delete group;
        return;
    }
    qDebug() << "Group (Ellipse + Name) added to scene for state:" << stateName;
}

void MainWindow::on_actionSave_triggered() {
    qDebug() << "Save action triggered.";
}


/**
 * @brief Initiates the 'Add Transition' mode.
 */
void MainWindow::on_addTransitionButton_clicked() {
    addingTransitionMode = true;
    startItemForTransition = nullptr; // Reset start item
    qDebug() << "Entered Add Transition mode. Click the start state.";
    // Optional: Update status bar or change cursor
    // ui->statusbar->showMessage("Click the starting state for the transition.", 0);
    // QApplication::setOverrideCursor(Qt::CrossCursor);
}





/**
 * @brief Handles clicks on state items, managing the transition creation process.
 */
void MainWindow::handleStateClick(QGraphicsItemGroup *item, QGraphicsLineItem *lineItem) {
    // Only handle clicks if we are in the 'Add Transition' mode
    //if (!addingTransitionMode) {
        //return;
    //}

    

    qDebug() << "handleStateClick called. Adding transition mode:";
    qDebug() << "State item clicked:" << lineItem->data(0).toString();

    if(addingTransitionMode == false) {
        return;
    }

    qDebug() << "State item clicked:" << lineItem->data(0).toString();

    // Check if a valid state item was clicked (item is not nullptr)
    
    if (item) {
        
        
        //highlightItem(item); // Highlight the clicked item

        if (startItemForTransition == nullptr) {
            qDebug() << "State item clicked:" << item->data(0).toString();
            // --- First click: Selecting the start state ---
            startItemForTransition = item;
            highlightItem(startItemForTransition); // Highlight the selected start state
            this->startStateForTransition = machine->getState(item->data(0).toString().toStdString());
            if(this->startStateForTransition) {
                qDebug() << "Start state selected:" << QString::fromStdString(this->startStateForTransition->getName());
            } else {
                qDebug() << "Start state could not be determined.";
            }
            //QString startStateName = startItemForTransition->data(0).toString();
            //qDebug() << "Start state selected:" << startStateName << ". Click the target state.";
            // ui->statusbar->showMessage(QString("Start state '%1' selected. Click the target state.").arg(startStateName), 0);
        }

        else if(endItemForTransition == nullptr)
        {
            endItemForTransition = item;
            highlightItem(endItemForTransition); // Highlight the selected end state
            this->endStateForTransition = machine->getState(item->data(0).toString().toStdString());
            if (this->endStateForTransition) {
                qDebug() << "End state selected:" << QString::fromStdString(this->endStateForTransition->getName());
            } else {
                qDebug() << "End state could not be determined.";
            }

            QDialog dialog(this);
            dialog.setWindowTitle("Enter Transition Details");

            QVBoxLayout layout(&dialog);

            QLabel label1("Enter transition condition:", &dialog);
            QLineEdit lineEdit1(&dialog);
            layout.addWidget(&label1);
            layout.addWidget(&lineEdit1);

            QLabel label2("Enter transition delay:", &dialog);
            QLineEdit lineEdit2(&dialog);
            layout.addWidget(&label2);
            layout.addWidget(&lineEdit2);

            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            layout.addWidget(&buttonBox);

            connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            QString condition;
            int delay = 0;;

            if (dialog.exec() == QDialog::Accepted) {
                condition = lineEdit1.text().trimmed();
                delay = lineEdit2.text().trimmed().toInt();

                if (condition.isEmpty()) {
                    qDebug() << "Dialog cancelled or empty input.";
                    return;
                }

                qDebug() << "Transition condition:" << condition;
                qDebug() << "Transition action:" << delay;
                // Use `condition` and `action` as needed
            } else {
                qDebug() << "Dialog cancelled.";
                return;
            }


            

           
            drawArrow(startItemForTransition, endItemForTransition, QString::fromStdString(condition.toStdString()));
            
            try {
                if(this->startStateForTransition && this->endStateForTransition) {
                    std::unique_ptr<Transition> newTransition = std::make_unique<Transition>(
                        *this->startStateForTransition,
                        *this->endStateForTransition,
                        condition.toStdString(),
                        delay
                        ); // Defaults for now
    
                    machine->addTransition(std::move(newTransition));
                    qDebug() << "Transition added to Automaton model:" 
                            << QString::fromStdString(this->startStateForTransition->getName()) 
                            << "->" 
                            << QString::fromStdString(this->endStateForTransition->getName());
                }
               
            } catch (const std::exception& e) {
                qCritical() << "Error adding transition to automaton:" << e.what();
                QMessageBox::critical(this, "Model Error", QString("Failed to add transition to model: %1").arg(e.what()));
                unhighlightItem(startItemForTransition);
                unhighlightItem(endItemForTransition);
                addingTransitionMode = false;
                startItemForTransition = nullptr;
                endItemForTransition = nullptr;
                return;
            }

            JsonCreator::saveToFile(*machine, "automaton.json");

            // --- Reset state ---
            unhighlightItem(startItemForTransition);
            unhighlightItem(endItemForTransition);
            addingTransitionMode = false;
            startItemForTransition = nullptr;
            endItemForTransition = nullptr;
            
            
        }
    
    }
    else if(lineItem) {
        // Handle line item clicks if needed
        qDebug() << "Line item clicked:" << lineItem->data(0).toString();
    }
    else {
        qDebug() << "Clicked item is not a valid state or line item.";
    }
}

std::string MainWindow::displayDialog(const std::string& textToDisplay) {
    bool okClicked;
    QString input = QInputDialog::getText(this, "Enter " + QString::fromStdString(textToDisplay),
                                          QString::fromStdString(textToDisplay), QLineEdit::Normal,
                                          "", &okClicked);
    if (!okClicked || input.trimmed().isEmpty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return nullptr; // Return an empty string if cancelled or no input
    }
    return input.trimmed().toStdString();
}


//void MainWindow::createTransition(const std::string& startStateName, const std::string& endStateName) {
    // Placeholder for creating a transition in the backend model
    //qDebug() << "Creating transition from" << QString::fromStdString(startStateName) << "to" << QString::fromStdString(endStateName);

//}

/**
 * @brief Draws an arrow between two points.
 * @param start The starting point of the arrow.
 * @param end The ending point of the arrow.
 */
void MainWindow::drawArrow(const QGraphicsItemGroup *start, const QGraphicsItemGroup *end, const QString &label) {
    // Calculate the line between start and end
    QPointF startCenter = start->sceneBoundingRect().center();
    QPointF endCenter = end->sceneBoundingRect().center();
    QLineF line(startCenter, endCenter);

    // Draw the main line
    QGraphicsLineItem* mainLine = new QGraphicsLineItem(line);


    mainLine->setPen(QPen(Qt::black, 2));
    mainLine->setData(0, QVariant(1));

    scene->addItem(mainLine);

    // Calculate the arrowhead
    qreal arrowSize = 10;
    QLineF arrowLine1 = line;
    QLineF arrowLine2 = line;

    arrowLine1.setLength(arrowSize);
    arrowLine2.setLength(arrowSize);

    arrowLine1.setAngle(line.angle() - 135);
    arrowLine2.setAngle(line.angle() + 135);

    QGraphicsLineItem* arrowHead1 = new QGraphicsLineItem(arrowLine1);
    QGraphicsLineItem* arrowHead2 = new QGraphicsLineItem(arrowLine2);

    arrowHead1->setPen(QPen(Qt::black, 2));
    arrowHead2->setPen(QPen(Qt::black, 2));

    scene->addItem(arrowHead1);
    scene->addItem(arrowHead2);

    // Add the label next to the arrow
    QGraphicsTextItem* text = new QGraphicsTextItem(label);
    QPointF midPoint = line.pointAt(0.5); // Get the midpoint of the line
    text->setPos(midPoint.x() + 5, midPoint.y() - 15); // Offset the label slightly
    scene->addItem(text);

    qDebug() << "Arrow drawn from" << start << "to" << end << "with label:" << label;
}


/**
 * @brief Applies a visual effect to highlight an item.
 */
void MainWindow::highlightItem(QGraphicsItemGroup* item) {
    if (!item) return;
    // Example using opacity - could also use QGraphicsDropShadowEffect or change color
    item->setOpacity(0.2); // Make it slightly transparent
     // Or add an effect:
     // QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
     // effect->setColor(Qt::yellow);
     // effect->setBlurRadius(15);
     // item->setGraphicsEffect(effect);
}

/**
 * @brief Removes visual highlighting from an item.
 */
void MainWindow::unhighlightItem(QGraphicsItemGroup* item) {
     if (!item) return;
     item->setOpacity(1.0); // Restore full opacity
     // Or remove effect:
     // item->setGraphicsEffect(nullptr);
}




