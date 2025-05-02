// Needed includes:
#include "mainwindow.h"
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
#include <sstream> // Needed for std::istringstream
#include <QtMath> // Needed for qDegreesToRadians

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

    //JsonCreator::saveToFile(*machine, "automaton.json");

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
    qDebug() << "Add variable button clicked.";

    std::string input = displayDialog("Enter variable name:");
    if (input.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }
    qDebug() << "Variable name:" << QString::fromStdString(input);
    
    ParseVariableArguments(input);
}

void MainWindow::ParseVariableArguments(const std::string& userInput) {
    // Split the input string by spaces
    std::istringstream iss(userInput);
    std::string type, name, value;
    iss >> type >> name >> value;

    // Check if all parts are present
    if (name.empty() || type.empty() || value.empty()) {
        qDebug() << "Invalid input format. Expected: <name> <type> <value>";
        return;
    }

    // Create a new Variable object
    std::unique_ptr<Variable> newVariable = std::make_unique<Variable>(name, value, type);

    // Add the variable to the machine
    try {
        machine->addVariable(std::move(newVariable));
        qDebug() << "Variable added to Automaton model:" << QString::fromStdString(name);
    } catch (const std::exception& e) {
        qCritical() << "Error adding variable to automaton:" << e.what();
        QMessageBox::critical(this, "Model Error", QString("Failed to add variable to model: %1").arg(e.what()));
        return;
    }
}

void MainWindow::on_addInputButton_clicked() {
    qDebug() << "Input button clicked.";

    std::string inputName = displayDialog("Enter variable name:");
    if (inputName.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }
    std::unique_ptr<Input> newInput = std::make_unique<Input>(inputName);
    machine->addInput(std::move(newInput));
    qDebug() << "Input name:" << QString::fromStdString(inputName);
}

void MainWindow::on_addOutputButton_clicked() {
    
    qDebug() << "Input button clicked.";

    std::string outputName = displayDialog("Enter variable name:");
    if (outputName.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }

    std::unique_ptr<Output> newOutput = std::make_unique<Output>(outputName);
    machine->addOutput(std::move(newOutput));
    qDebug() << "Input name:" << QString::fromStdString(outputName);
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
        stateAction = lineEdit2.text().trimmed();

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

    setElipseText(ellipse, stateName.toStdString());

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
    group->setData(0, QVariant(objectStateId)); // Store the state ID in the group
    group->setData(1, "state"); // Store the state name in the group

    scene->addItem(group);

    std::unique_ptr<State> newState = std::make_unique<State>(stateName.toStdString(), stateAction.toStdString(), objectStateId);
    objectStateId++;
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

    

    qDebug() << "State item clicked:" << lineItem->data(0).toString();

    // Check if a valid state item was clicked (item is not nullptr)
    

    
    if (item) {
        
        // edit state
        if(!addingTransitionMode && item->data(1).toString() == "state") {
            editState(item);
            return;
        }

        
        //highlightItem(item); // Highlight the clicked item

        if (startItemForTransition == nullptr) {
            qDebug() << "State item clicked:" << item->data(0).toString();
            // --- First click: Selecting the start state ---
            startItemForTransition = item;
            highlightItem(startItemForTransition); // Highlight the selected start state
            this->startStateForTransition = machine->getState(item->data(0).toInt());
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
            this->endStateForTransition = machine->getState(item->data(0).toInt());
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
           
            drawArrow(startItemForTransition, endItemForTransition, QString::fromStdString(condition.toStdString()), objectTransitionId);
            
            try {
                if(this->startStateForTransition && this->endStateForTransition) {
                    std::unique_ptr<Transition> newTransition = std::make_unique<Transition>(
                        *this->startStateForTransition,
                        *this->endStateForTransition,
                        objectTransitionId,
                        condition.toStdString(),
                        delay
                         // Increment the transition ID for each new transition
                        ); // Defaults for now
    
                    machine->addTransition(std::move(newTransition));
                    objectTransitionId++;
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

            

            // --- Reset state ---
            unhighlightItem(startItemForTransition);
            unhighlightItem(endItemForTransition);
            addingTransitionMode = false;
            startItemForTransition = nullptr;
            endItemForTransition = nullptr;
            
            
        }
    
    }
    else if(lineItem) {
        qDebug() << "Line item clicked:";
        // Handle line item clicks if needed
        Transition *trans = machine->getTransition(lineItem->data(0).toInt());
        if (trans) {
            
            qDebug() << QString::fromStdString(trans->DisplayTransition());

            QDialog dialog(this);
            dialog.setWindowTitle("Edit Transition Details");

            QVBoxLayout layout(&dialog);

            QLabel label1("Edit transition condition:", &dialog);
            QLineEdit lineEdit1(&dialog);
            lineEdit1.setText(QString::fromStdString(trans->getCondition())); // Pre-fill with current condition
            layout.addWidget(&label1);
            layout.addWidget(&lineEdit1);

            QLabel label2("Edit transition delay:", &dialog);
            QLineEdit lineEdit2(&dialog);
            lineEdit2.setText(QString::number(trans->getDelayMs())); // Pre-fill with current delay
            layout.addWidget(&label2);
            layout.addWidget(&lineEdit2);

            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            layout.addWidget(&buttonBox);

            connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            QString condition;
            int delay = 0;

            if (dialog.exec() == QDialog::Accepted) {
                condition = lineEdit1.text().trimmed();
                delay = lineEdit2.text().trimmed().toInt();

                if (condition.isEmpty()) {
                    qDebug() << "Dialog cancelled or empty input.";
                    return;
                }

                qDebug() << "Updated transition condition:" << condition;
                qDebug() << "Updated transition delay:" << delay;

                // Update the transition object with new values
                trans->setCondition(condition.toStdString());
                trans->setDelay(delay);
            } else {
                qDebug() << "Dialog cancelled.";
                return;
            }


        } else {
            qDebug() << "Transition not found.";
        }
    }
    else {
        qDebug() << "Clicked item is not a valid state or line item.";
    }
}

void editTransition(int transitionId) {
    // Placeholder for editing transition logic
    qDebug() << "Edit transition function called.";
    // Implement the logic to edit the selected transition
}

void MainWindow::editState(QGraphicsItemGroup *item) {

    int stateId = item->data(0).toInt(); // Retrieve the state ID stored in the item
    State* state = machine->getState(stateId); // Get the state object using the ID
    if (!state) {
        qDebug() << "State not found for ID:" << stateId;
        return;
    }
    


    
    std::string newName;
    std::string newAction;
    
    std::string currentName = state->getName();
    std::string currentAction = state->getAction();
    

    ProccessInputEditDialog("Edit State", "Enter new state name:", "Enter new action:",
        currentName, currentAction, &newName, &newAction);

    qDebug () << "New name:" << QString::fromStdString(newName);

    state->setName(newName);
    state->setAction(newAction);



    // --- Aktualizácia grafického prvku ---
    // 1. Nájdi QGraphicsTextItem v skupine
    QGraphicsTextItem *textItem = nullptr;
    for (QGraphicsItem *child : item->childItems()) {
        textItem = dynamic_cast<QGraphicsTextItem*>(child);
        if (textItem) {
            break; // Našli sme ho
        }
    }

    if (textItem) {
        // 2. Zmeň zobrazený text
        QString newNameQt = QString::fromStdString(newName); // Použi nové meno z dialógu
        textItem->setPlainText(newNameQt);
        qDebug () <<"new name qt" << newNameQt;

        // 3. Aktualizuj uložené dáta v skupine (ak si menil meno)
        //item->setData(0, QVariant(newNameQt)); // Ulož NOVÉ meno pod kľúčom 0

        // 4. Prerátaj pozíciu textu, aby bol vycentrovaný
        //    Potrebujeme nájsť aj elipsu na získanie rozmerov
        QGraphicsEllipseItem* ellipseItem = nullptr;
         for (QGraphicsItem *child : item->childItems()) {
            ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(child);
            if (ellipseItem) {
                break;
            }
        }
        setElipseText(ellipseItem, newName);
        /*
        if(ellipseItem) {
            QRectF ellipseRect = ellipseItem->rect(); // Získaj rozmery elipsy
            QRectF textRect = textItem->boundingRect();
            qreal textX = (ellipseRect.width() - textRect.width()) / 2;
            qreal textY = (ellipseRect.height() - textRect.height()) / 2;
            textItem->setPos(textX, textY); // Nastav novú pozíciu textu
        } else {
             qWarning() << "Could not find ellipse item within the group to recenter text.";
        }
        */

        qDebug() << "State item text updated to:" << newNameQt;
    } else {
        qWarning() << "Could not find text item within the group to update.";
    }

    
    

    
    //newName = QString::fromStdString(updatedName);
    //newAction = QString::fromStdString(updatedAction);
            /*
            
            qDebug() << "State updated: Name =" << newName << ", Action =" << newAction;
        } else {
            qDebug() << "Dialog cancelled.";
        }
    } else {
        //qDebug() << "State not found with name:" << QString::fromStdString(stateId);
    }
        */
        
}

void MainWindow::setElipseText(QGraphicsEllipseItem* ellipseItem, const std::string& text) {
    if (!ellipseItem) {
        qWarning() << "Cannot set text: Invalid ellipse item.";
        return;
    }

    QGraphicsTextItem* textItem = new QGraphicsTextItem(QString::fromStdString(text));
    QRectF textRect = textItem->boundingRect();
    qreal textX = (ellipseItem->rect().width() - textRect.width()) / 2;
    qreal textY = (ellipseItem->rect().height() - textRect.height()) / 2;
    textItem->setPos(textX, textY);
}


void MainWindow::ProccessInputEditDialog(const std::string& title, const std::string& label1Text, const std::string& label2Text, 
    std::string& predefinedInput1, std::string& predefinedInput2, 
    std::string* newInput1, std::string* newInput2) {

        QDialog dialog(this);
        dialog.setWindowTitle(QString::fromStdString(title));

        QVBoxLayout layout(&dialog);

        QLabel label1(QString::fromStdString(label1Text), &dialog);
        QLineEdit lineEdit1(&dialog);
        lineEdit1.setText(QString::fromStdString(predefinedInput1)); // Pre-fill with current name
        layout.addWidget(&label1);
        layout.addWidget(&lineEdit1);

        QLabel label2(QString::fromStdString(label2Text), &dialog);
        QLineEdit lineEdit2(&dialog);
        lineEdit2.setText(QString::fromStdString(predefinedInput2)); // Pre-fill with current action
        layout.addWidget(&label2);
        layout.addWidget(&lineEdit2);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        layout.addWidget(&buttonBox);

        connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            *newInput1 = lineEdit1.text().trimmed().toStdString();
            *newInput2 = lineEdit2.text().trimmed().toStdString();
        } else {
            *newInput1 = QString().toStdString();
            *newInput2 = QString().toStdString();
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
void MainWindow::drawArrow(const QGraphicsItemGroup *start, const QGraphicsItemGroup *end, const QString &label, int transitionId) {
    if (!scene || !start || !end) {
        qWarning() << "Cannot draw arrow: Invalid scene or start/end item.";
        return;
    }

    // --- Common Arrow Properties ---
    QPen arrowPen(Qt::red, 2); // Pen for the arrow line and head
    qreal arrowSize = 10;      // Size of the arrowhead

    // --- Create a group for the entire transition representation ---
    // This group will contain the line/curve, arrowhead(s), and label.
    QGraphicsItemGroup *arrowGroup = new QGraphicsItemGroup();

    // --- Store Transition ID in the group ---
     // Item type identifier
    arrowGroup->setData(0, QVariant(transitionId)); // Store the actual transition ID
    arrowGroup->setData(1, "Transition");

    if (start == end) {
        // --- Draw Self-Loop (Arc) ---
        qDebug() << "Drawing self-loop for state:" << start->data(0).toString() << "ID:" << transitionId;

        QRectF bounds = start->sceneBoundingRect(); // Get bounds of the state group
        qreal radiusX = bounds.width() / 1.5; // Horizontal radius of the loop
        qreal radiusY = bounds.height() / 1.5; // Vertical radius of the loop
        QPointF center = bounds.center();
        // Position the loop slightly above the state
        QPointF loopTopCenter(center.x(), bounds.top() - radiusY * 0.8);

        // Create the path for the arc (ellipse segment)
        QPainterPath loopPath;
        // Start angle and span angle for QPainterPath::arcTo
        // Angles are in degrees, 0 is at 3 o'clock, positive is counter-clockwise
        // We want an arc mostly above the state
        qreal startAngle = 210; // Start slightly left of bottom
        qreal spanAngle = -240; // Go counter-clockwise almost full circle
        QRectF arcRect(loopTopCenter.x() - radiusX, loopTopCenter.y() - radiusY, 2 * radiusX, 2 * radiusY);
        loopPath.arcMoveTo(arcRect, startAngle);
        loopPath.arcTo(arcRect, startAngle, spanAngle);

        // Create a QGraphicsPathItem for the loop
        QGraphicsPathItem* loopItem = new QGraphicsPathItem(loopPath);
        loopItem->setPen(arrowPen);
        arrowGroup->addToGroup(loopItem); // Add arc to the group

        // --- Calculate Arrowhead for the loop ---
        // Get the end point and angle of the arc path
        QPointF loopEnd = loopPath.currentPosition();
        qreal angle = loopPath.angleAtPercent(1.0); // Angle at the end of the path

        

        QPointF arrowP1 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) + M_PI / 3) * arrowSize,
                            qCos(qDegreesToRadians(-angle) + M_PI / 3) * arrowSize);
        QPointF arrowP2 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) + M_PI - M_PI / 3) * arrowSize,
                            qCos(qDegreesToRadians(-angle) + M_PI - M_PI / 3) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << loopEnd << arrowP1 << arrowP2;
        QGraphicsPolygonItem *arrowHeadItem = new QGraphicsPolygonItem(arrowHead);
        arrowHeadItem->setPen(arrowPen);
        arrowHeadItem->setBrush(Qt::red); // Fill the arrowhead
        arrowGroup->addToGroup(arrowHeadItem); // Add arrowhead to the group

        // --- Position Label for the loop ---
        // Place the label above the loop's highest point
        QGraphicsTextItem* text = new QGraphicsTextItem(label);
        QRectF textRect = text->boundingRect();
        text->setPos(loopTopCenter.x() - textRect.width() / 2, loopTopCenter.y() - radiusY - textRect.height() - 2);
        arrowGroup->addToGroup(text); // Add label to the group

    } else {
        // --- Draw Straight Arrow ---
        qDebug() << "Drawing straight arrow from" << start->data(0).toString() << "to" << end->data(0).toString() << "ID:" << transitionId;

        QPointF startCenter = start->sceneBoundingRect().center();
        QPointF endCenter = end->sceneBoundingRect().center();
        QLineF line(startCenter, endCenter);

        // --- Adjust line to connect to edges of circles instead of centers (optional but nicer) ---
        // Create temporary lines from center to edge intersection points
        QLineF startToCenter(startCenter, endCenter);
        QLineF endToCenter(endCenter, startCenter);
        // Assuming states are roughly circular/elliptical of known size (e.g., 60x60)
        qreal stateRadius = 30; // Approximate radius
        startToCenter.setLength(stateRadius); // Shorten line to edge
        endToCenter.setLength(stateRadius);   // Shorten line to edge
        // New line points on the edges
        QPointF adjustedStartPoint = startToCenter.p2();
        QPointF adjustedEndPoint = endToCenter.p2();
        // Update the main line
        line.setPoints(adjustedStartPoint, adjustedEndPoint);


        // Draw the main line
        QGraphicsLineItem* mainLine = new QGraphicsLineItem(line);
        mainLine->setPen(arrowPen);
        arrowGroup->addToGroup(mainLine); // Add line to the group

        // --- Calculate Arrowhead for the line ---
        double angle = std::atan2(-line.dy(), line.dx()); // Angle of the line

        QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize,
                                              cos(angle + M_PI / 3) * arrowSize);
        QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                              cos(angle + M_PI - M_PI / 3) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << line.p2() << arrowP1 << arrowP2; // Points for the triangle
        QGraphicsPolygonItem *arrowHeadItem = new QGraphicsPolygonItem(arrowHead);
        arrowHeadItem->setPen(arrowPen);
        arrowHeadItem->setBrush(Qt::red); // Fill the arrowhead
        arrowGroup->addToGroup(arrowHeadItem); // Add arrowhead to the group

        // --- Position Label for the line ---
        QGraphicsTextItem* text = new QGraphicsTextItem(label);
        QPointF midPoint = line.pointAt(0.5); // Get the midpoint of the line
        // Offset the label slightly perpendicular to the line
        qreal offsetX = 10 * sin(angle);
        qreal offsetY = -10 * cos(angle);
        text->setPos(midPoint.x() + offsetX, midPoint.y() + offsetY);
        arrowGroup->addToGroup(text); // Add label to the group
    }

    // --- Finalize Group ---
    arrowGroup->setFlag(QGraphicsItem::ItemIsSelectable); // Make the whole group selectable
    scene->addItem(arrowGroup); // Add the complete group to the scene

    qDebug() << "Arrow/Loop group added to scene for transition ID:" << transitionId;
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




