// Needed includes:
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "core/Machine.h"
#include "core/State.h"
#include "core/Transition.h" // Needed for creating Transition
#include "mainWindowUtils.h" // Needed for utility functions
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
#include <QPainterPath>       // Required for paths
#include <QGraphicsPathItem>  // Required for displaying paths
#include <cmath>              // For std::atan2, std::sin, std::cos, std::sqrt
#include <QtMath>             // For qDegreesToRadians, qRadiansToDegrees (optional, can use M_PI)
#include <string>       // Pre std::string
#include <memory>       // Pre std::unique_ptr, std::make_unique, std::move
#include <QDebug>       // Pre qDebug()
#include <QString>      // Pre QString
#include <QLabel>       // << Potrebný pre zobrazenie textu
#include <QVBoxLayout>  // << Potrebný pre usporiadanie pod seba
#include <QGroupBox>    // << Potrebný pre prístup ku groupboxu (ak pristupuješ z kódu)
#include <QCoreApplication> // Potrebný include
#include <QDir>
#include "persistence/JsonPersistance.h" // Alebo JsonCreator.h
#include "codegen/CodeGenerator.h"
#include <fstream>
#include <QProcess>



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

    std::string machineName = MainWindowUtils::ProccessOneArgumentDialog("Set automat name");


    machine = new Machine(machineName);

    
    qDebug() << "Machine object created successfully.";
    // Set trackingOutputsGroupBox as invisible
    
    ui->editVarGroupBox->setVisible(false);  
   
    ui->editInGroupBox->setVisible(false);

    std::string initialStateName;
    std::string initialStateAction;;

    std::string portGUI;
    std::string portAutomat;


    ProccessMultipleArgsInputEditDialog("ADD PORTS", "Add port GUI", "Add port AUTOMAT",
        initialStateName, initialStateAction, &this->portGUI, &this->portAutomat);

    
}

MainWindow::~MainWindow() {
    delete ui;
}



void MainWindow::on_runAutomatButton_clicked() {

    qDebug() << "Run Automaton button clicked.";

    
    if (!machine) {
        QMessageBox::critical(this, "Error", "No automaton model loaded or created.");
        return;
    }
    std::string automatonNameStd = machine->getName();

    /*
    if (automatonNameStd.empty()) {
        automatonNameStd = MainWindowUtils::ProccessOneArgumentDialog("Enter automaton name:");
        if (automatonNameStd.empty()) {
             QMessageBox::warning(this, "Cancelled", "Automaton name is required.");
             return;
        }
        // Tu by si mal pravdepodobne aktualizovať aj názov v `machine` objekte, ak má metódu setName()
        // machine->setName(automatonNameStd);
        qDebug() << "Using entered automaton name:" << QString::fromStdString(automatonNameStd);
    }*/

    QString automatonName = QString::fromStdString(automatonNameStd);
    QString safeDirName = automatonName;
    safeDirName.replace(QRegExp("[^a-zA-Z0-9_.-]"), "_"); // Vytvor bezpečný názov pre adresár

    QString appDirPath = QCoreApplication::applicationDirPath();
    qDebug() << "Application directory path:" << appDirPath;
    QDir buildSubDir(appDirPath);
    buildSubDir.cdUp();
    QString runtimeLibDir = buildSubDir.absolutePath() + "/runtime";
    buildSubDir.cdUp();
    buildSubDir.cdUp();
    QString projectPath = buildSubDir.absolutePath();
    QString targetBaseDir = projectPath + "/generated_automatons";
    QString targetAutomatonDir = targetBaseDir + "/" + safeDirName;
    qDebug() << "Target automaton directory:" << targetAutomatonDir;

    // --- 3. Vytvorenie Cieľového Adresára ---
    QDir dir(targetAutomatonDir);
    if (!dir.exists()) {
        qDebug() << "Creating directory for automaton:" << targetAutomatonDir;
        if (!dir.mkpath(".")) {
             QMessageBox::critical(this, "Error", "Could not create directory: " + targetAutomatonDir);
             return;
        }
    } else {
         qDebug() << "Directory already exists:" << targetAutomatonDir;
    }

    // --- 4. Uloženie JSON Definície do Cieľového Adresára ---
    QString jsonPath = targetAutomatonDir + "/" + safeDirName + ".json";
    qDebug() << "Saving current model to JSON:" << jsonPath;
    if (!JsonPersistence::saveToFile(*machine, jsonPath.toStdString())) { // Použi správnu triedu na ukladanie
         QMessageBox::critical(this, "Error", "Failed to save automaton model to JSON file.");
         return;
    }

    // --- 5. Generovanie C++ Kódu do Cieľového Adresára ---
    QString generatedCppPath = targetAutomatonDir + "/" + safeDirName + "_generated.cpp";
    QString templatePath = projectPath + "/templates/automation_template.tpl"; // Cesta k šablóne

    qDebug() << "Generating C++ code from template:" << templatePath << "to:" << generatedCppPath;
    try {
        CodeGenerator generator(templatePath.toStdString());
        // Predpokladáme, že generate berie Machine objekt a interné to_json ho konvertuje
        // Alebo ak CodeGenerator číta JSON, tak by čítal jsonPath
        std::string cpp_code = generator.generate(*machine, jsonPath.toStdString());

        std::ofstream cppFile(generatedCppPath.toStdString());
        if (!cppFile) {
             throw CodeGenerator::GenerationError("Failed to open output C++ file for writing: " + generatedCppPath.toStdString());
        }
        cppFile << cpp_code;
        cppFile.close();
         if (!cppFile) { // Skontroluj aj po zatvorení
             throw CodeGenerator::GenerationError("Failed to write to output C++ file: " + generatedCppPath.toStdString());
         }
         qInfo() << "C++ code generated successfully.";

         QMessageBox::information(this, "Success", "Automaton JSON and C++ code generated successfully in:\n" + targetAutomatonDir);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Code Generation Error", QString("Failed to generate C++ code: %1").arg(e.what()));
        // Voliteľne zmaž nekompletné súbory, ak nastala chyba
        QFile::remove(generatedCppPath);
        // QFile::remove(jsonPath); // JSON už bol úspešne uložený
        return;
    }







    // --- 6. Kompilácia C++ Kódu pomocou g++ ---
    QString executablePath = targetAutomatonDir + "/" + safeDirName + "_automaton"; // Výstupný súbor bez prípony pre Linux

    qDebug() << "Compiling" << generatedCppPath << "to" << executablePath;

    QString runtimeDir = projectPath + "/src/runtime";
    QString asioInclude = projectPath + "/third_party/asio/include";

    QString compiler = "g++"; // Predpokladáme, že g++ je v PATH
    QStringList arguments;
    arguments << "-std=c++17";
    arguments << "-I" + runtimeDir;         // Cesta k ifa_runtime hlavičkám
    arguments << "-I" + asioInclude;        // Cesta k Asio hlavičkám
    arguments << generatedCppPath;          // Vstupný generovaný súbor
    // Výstupný spustiteľný súbor
    arguments << "-o" << executablePath;
    arguments << "-L" + runtimeLibDir;
    arguments << "-lifa_runtime";
    // Linker flagy pre Linux
    arguments << "-lpthread";
    arguments << "-lstdc++fs"; // Potrebné pre std::filesystem

    QProcess compilerProcess;
    qDebug() << "Compiler command:" << compiler << arguments.join(" ");
    compilerProcess.setWorkingDirectory(projectPath); // Nastav pracovný adresár na koreň projektu
    compilerProcess.start(compiler, arguments);

    // Čakanie na dokončenie kompilácie a kontrola chýb
    if (!compilerProcess.waitForStarted(-1)) {
         QMessageBox::critical(this, "Compilation Error", "Could not start the compiler process: " + compilerProcess.errorString());
         return;
    }
    qDebug() << "Compiler process started, waiting for finish...";
    if (!compilerProcess.waitForFinished(-1)) {
         QMessageBox::critical(this, "Compilation Error", "Compiler process timed out or failed to finish: " + compilerProcess.errorString());
         return;
    }

    qDebug() << "Compiler process finished with exit code:" << compilerProcess.exitCode();
    if (compilerProcess.exitCode() != 0) {
        QString errorOutput = compilerProcess.readAllStandardError();
        QString stdOutput = compilerProcess.readAllStandardOutput();
        qDebug() << "Compilation Error Output:\n" << errorOutput;
        qDebug() << "Compilation Standard Output:\n" << stdOutput;
        // Zobraz chybu v okne
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Compilation Failed");
        msgBox.setText(QString("Compiler exited with code %1.").arg(compilerProcess.exitCode()));
        msgBox.setDetailedText("Standard Output:\n" + stdOutput + "\n\nError Output:\n" + errorOutput);
        msgBox.exec();
        return; // Chyba kompilácie
    }

    qInfo() << "Compilation successful: " << executablePath;
    QMessageBox::information(this, "Compilation Successful", "Automaton compiled successfully:\n" + executablePath);

    // --- 7. Spustenie skompilovaného automatu (Nezávisle) ---
    qDebug() << "portAutomat:" << QString::fromStdString(portAutomat);
    qDebug() << "portGUI:" << QString::fromStdString(portGUI);

    QStringList automatonArgs;
    automatonArgs << QString::number(std::stoi(portAutomat));

    automatonArgs << QString::number(std::stoi(portGUI));


    qDebug() << "Starting automaton process detached:" << executablePath << automatonArgs.join(" ");

    qint64 pid = 0; // Premenná na uloženie PID
    qDebug() << "Arguments passed to automaton:" << automatonArgs;

    // Použi QProcess::startDetached, ktorá vracia PID
    if (QProcess::startDetached(executablePath, automatonArgs, targetAutomatonDir, &pid)) { // Štvrtý argument prijme PID
        qInfo() << "Automaton process" << automatonName << "started successfully (detached) with PID:" << pid;
        QMessageBox::information(this, "Started", "Automaton '" + automatonName + "' started (PID: " + QString::number(pid) + ").\nListen Port: " + QString::number(std::stoi(portAutomat)) + "\nGUI Port: " + QString::number(std::stoi(portGUI)));

        // Ulož PID do členskej premennej (ak ju máš)
        //currentAutomatonPid_ = pid; // Predpokladá, že máš premennú `qint64 currentAutomatonPid_ = 0;` v mainwindow.h

        // TODO: Aktualizuj stav GUI (napr. zobraz PID, zmeň text tlačidla)
        // ui->statusbar->showMessage("Automaton '" + automatonName + "' running (PID: " + QString::number(pid) + ")", 0);
        // ui->runAutomatButton->setText("Running (Stop?)"); // Alebo samostatné Stop tlačidlo
        // ui->pidLabel->setText(QString::number(pid)); // Ak máš label pre PID

    } else {
        qCritical() << "Failed to start detached automaton process.";
        QMessageBox::critical(this, "Process Error", "Could not start the automaton process.");
        // ui->statusbar->showMessage("Failed to start automaton!", 0);
    }

    qDebug() << "Starting automaton on PID:" << pid;

    
}



///////////////////////////////////////////////////////////////////////////////////


void MainWindow::on_mainMenuButton_clicked() {
    ui->menuGroupBox->setVisible(true);
    ui->editVarGroupBox->setVisible(false);  
    ui->editInGroupBox->setVisible(false);
   
    
}

void MainWindow::on_editFieldsButton_clicked() {
    ui->menuGroupBox->setVisible(false);
    ui->editVarGroupBox->setVisible(true);
    ui->editInGroupBox->setVisible(true);
}



void MainWindow::on_saveJsonButton_clicked() {
    qDebug() << "Save JSON button clicked.";

   
}


void MainWindow::on_loadJsonButton_clicked() {
    qDebug() << "Load JSON button clicked.";

    
}


void MainWindow::CreateVarFromUserInput(const std::string& userInput) {
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

        addVarRowToGUI(name, type, value);

    } catch (const std::exception& e) {
        qCritical() << "Error adding variable to automaton:" << e.what();
        QMessageBox::critical(this, "Model Error", QString("Failed to add variable to model: %1").arg(e.what()));
        return;
    }
}



void MainWindow::on_addVariableButton_clicked() {
   
    std::string input = MainWindowUtils::ProccessOneArgumentDialog("Enter variable name:");
    if (input.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }    
    CreateVarFromUserInput(input);
}






/**
 * @brief Adds a new row representing a variable to the editVarGroupBox UI.
 * @param name The name of the variable.
 * @param type The type hint string of the variable.
 * @param value The current value string of the variable.
 */
void MainWindow::addVarRowToGUI(const std::string& name, const std::string& type, const std::string& value) {
    // Ensure the target GroupBox exists in the UI
    if (!ui->editVarGroupBox) {
        qWarning() << "Cannot add variable row: ui->editVarGroupBox is null. Check objectName in Designer.";
        return;
    }

    // Get the layout of the GroupBox, assuming/creating QVBoxLayout
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->editVarGroupBox->layout());
    if (!mainLayout) {
        qDebug() << "No existing layout found in editVarGroupBox, creating new QVBoxLayout.";
        mainLayout = new QVBoxLayout();
        ui->editVarGroupBox->setLayout(mainLayout);
        // Optional: Add stretch at the end to push items to the top
        // mainLayout->addStretch(1);
    }

    // --- Create Widgets for the new row ---
    // Create a horizontal layout for the row elements
    QHBoxLayout* rowLayout = new QHBoxLayout();

    // Create labels for type and name (read-only)
    QLabel* typeLabel = new QLabel(QString::fromStdString(type));
    QLabel* nameLabel = new QLabel(QString::fromStdString(name));
    nameLabel->setToolTip(QString::fromStdString(name)); // Show full name on hover if truncated

    // Create an editable line edit for the value
    QLineEdit* valueEdit = new QLineEdit(QString::fromStdString(value));
    connect(valueEdit, &QLineEdit::editingFinished, this, &MainWindow::handleVariableValueEdited);
    valueEdit->setToolTip("Edit variable value");

    
    // --- Set Object Names for later identification ---
    // Use a prefix and the variable name to create unique object names
    QString baseObjectName = "var_" + QString::fromStdString(name);
    typeLabel->setObjectName(baseObjectName + "_typeLabel");
    nameLabel->setObjectName(baseObjectName + "_nameLabel");
    valueEdit->setObjectName(QString::fromStdString(name));


    // Create a delete button for the row
    QPushButton* deleteButton = new QPushButton("x");
    deleteButton->setToolTip("Delete this variable");
    deleteButton->setObjectName(baseObjectName + "_deleteBtn");

    // Connect the delete button's clicked signal to a lambda function
    connect(deleteButton, &QPushButton::clicked, this, [this, rowLayout, name]() {
        // Remove the row from the layout
        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->editVarGroupBox->layout());
        if (mainLayout) {
            QLayoutItem* item = nullptr;
            for (int i = 0; i < mainLayout->count(); ++i) {
                item = mainLayout->itemAt(i);
                if (item && item->layout() == rowLayout) {
                    // Remove the layout and its widgets
                    while (QLayoutItem* child = rowLayout->takeAt(0)) {
                        delete child->widget();
                        delete child;
                    }
                    mainLayout->removeItem(item);
                    delete rowLayout;
                    break;
                }
            }
        }

        // Remove the variable from the machine
        machine->removeVariable(name);
        qDebug() << "Variable removed from UI and machine:" << QString::fromStdString(name);
    });

    // Add the delete button to the row layout
    rowLayout->addWidget(deleteButton);

    //deleteButton->setObjectName(baseObjectName + "_deleteBtn");
    // Set object name for the layout itself to find the whole row later
    rowLayout->setObjectName(baseObjectName + "_layout");


    // --- Add widgets to the row layout ---
    rowLayout->addWidget(typeLabel);    // Add type label
    rowLayout->addWidget(nameLabel);    // Add name label
    rowLayout->addWidget(valueEdit, 1); // Add value edit, give it stretch factor 1
    //rowLayout->addWidget(deleteButton); // Add delete button

    // --- Add the row layout to the main vertical layout ---
    // Insert before the last item if it's a stretch, otherwise at the end
    int insertPos = mainLayout->count();
    if (insertPos > 0 && mainLayout->itemAt(insertPos - 1)->spacerItem()) {
        insertPos--; // Insert before the stretch
    }
    mainLayout->insertLayout(insertPos, rowLayout);

    qDebug() << "Added variable row to UI for:" << QString::fromStdString(name);

    // Make the group box visible if it wasn't already
    if (!ui->editVarGroupBox->isVisible()) {
        ui->editVarGroupBox->setVisible(true);
    }
     // Ensure the group box is enabled
     if (!ui->editVarGroupBox->isEnabled()) {
        ui->editVarGroupBox->setEnabled(true);
    }
}

void MainWindow::handleVariableValueEdited() {
    // Get the sender object (the QLineEdit that emitted the signal)
    QLineEdit* senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) {
        qWarning() << "Sender is not a QLineEdit.";
        return;
    }

    // Get the object name to identify the variable
    QString varName = senderEdit->objectName();

    Variable* variable = machine->getVariable(varName.toStdString());
    if (!variable) {
        qWarning() << "Variable not found in machine:" << varName;
        return;
    }
    variable->setValue(senderEdit->text().toStdString());
    qDebug() << "Variable value edited:" << varName << "New value:" << QString::fromStdString(variable->getValueAsString());
   
    
}




void MainWindow::addInputRowToGUI(const std::string& name) {
    // Ensure the target GroupBox exists in the UI
    if (!ui->editVarGroupBox) {
        qWarning() << "Cannot add variable row: ui->editVarGroupBox is null. Check objectName in Designer.";
        return;
    }

    // Get the layout of the GroupBox, assuming/creating QVBoxLayout
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->editInGroupBox->layout());
    if (!mainLayout) {
        qDebug() << "No existing layout found in editVarGroupBox, creating new QVBoxLayout.";
        mainLayout = new QVBoxLayout();
        ui->editInGroupBox->setLayout(mainLayout);
        // Optional: Add stretch at the end to push items to the top
        // mainLayout->addStretch(1);
    }

    // --- Create Widgets for the new row ---
    // Create a horizontal layout for the row elements
    QHBoxLayout* rowLayout = new QHBoxLayout();

    // Create labels for type and name (read-only)
   
    QLabel* nameLabel = new QLabel(QString::fromStdString(name));
    nameLabel->setToolTip(QString::fromStdString(name)); // Show full name on hover if truncated

    // Create an editable line edit for the value
    QLineEdit* valueEdit = new QLineEdit(QString::fromStdString(""));
    connect(valueEdit, &QLineEdit::editingFinished, this, &MainWindow::handleInputValueEdited);
    
    valueEdit->setToolTip("Edit variable value");

    

    // --- Set Object Names for later identification ---
    // Use a prefix and the variable name to create unique object names
    QString baseObjectName = "var_" + QString::fromStdString(name);
    
    nameLabel->setObjectName(baseObjectName + "_nameLabel");
    valueEdit->setObjectName(QString::fromStdString(name));

    // Create a delete button for the row
    QPushButton* deleteButton = new QPushButton("x");
    deleteButton->setToolTip("Delete this input");
    deleteButton->setObjectName(baseObjectName + "_deleteBtn");

    // Connect the delete button's clicked signal to a lambda function
    connect(deleteButton, &QPushButton::clicked, this, [this, rowLayout, name]() {
        // Remove the row from the layout
        QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->editInGroupBox->layout());
        if (mainLayout) {
            QLayoutItem* item = nullptr;
            for (int i = 0; i < mainLayout->count(); ++i) {
                item = mainLayout->itemAt(i);
                if (item && item->layout() == rowLayout) {
                    // Remove the layout and its widgets
                    while (QLayoutItem* child = rowLayout->takeAt(0)) {
                        delete child->widget();
                        delete child;
                    }
                    mainLayout->removeItem(item);
                    delete rowLayout;
                    break;
                }
            }
        }

        // Remove the input from the machine
        machine->removeInput(name);
        qDebug() << "Input removed from UI and machine:" << QString::fromStdString(name);
    });

    // Add the delete button to the row layout
    rowLayout->addWidget(deleteButton);
    //deleteButton->setObjectName(baseObjectName + "_deleteBtn");
    // Set object name for the layout itself to find the whole row later
    rowLayout->setObjectName(baseObjectName + "_layout");


    // --- Add widgets to the row layout ---
    // Add type label
    rowLayout->addWidget(nameLabel);    // Add name label
    rowLayout->addWidget(valueEdit, 1); // Add value edit, give it stretch factor 1
    //rowLayout->addWidget(deleteButton); // Add delete button

    // --- Add the row layout to the main vertical layout ---
    // Insert before the last item if it's a stretch, otherwise at the end
    int insertPos = mainLayout->count();
    if (insertPos > 0 && mainLayout->itemAt(insertPos - 1)->spacerItem()) {
        insertPos--; // Insert before the stretch
    }
    mainLayout->insertLayout(insertPos, rowLayout);

    

    qDebug() << "Added variable row to UI for:" << QString::fromStdString(name);

    
}

void MainWindow::handleInputValueEdited() {
    // Get the sender object (the QLineEdit that emitted the signal)
    QLineEdit* senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) {
        qWarning() << "Sender is not a QLineEdit.";
        return;
    }

    // Get the object name to identify the variable
    QString varName = senderEdit->objectName();

    Input* input = machine->getInput(varName.toStdString());
    if (!input) {
        qWarning() << "Input not found in machine:" << varName;
        return;
    }
    input->updateValue(senderEdit->text().toStdString());
    qDebug() << "Input value edited:" << varName << "New value:" << QString::fromStdString(input->getLastValue().value_or("No value"));
   
}

void MainWindow::on_addInputButton_clicked() {
    qDebug() << "Input button clicked.";

    std::string inputName = MainWindowUtils::ProccessOneArgumentDialog("Enter variable name:");
    if (inputName.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }
    std::unique_ptr<Input> newInput = std::make_unique<Input>(inputName);
    machine->addInput(std::move(newInput));

    addInputRowToGUI(inputName);

    


}

void MainWindow::on_addOutputButton_clicked() {
    
    qDebug() << "Input button clicked.";

    std::string outputName = MainWindowUtils::ProccessOneArgumentDialog("Enter variable name:");
    if (outputName.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }

    std::unique_ptr<Output> newOutput = std::make_unique<Output>(outputName);
    machine->addOutput(std::move(newOutput));
    qDebug() << "Input name:" << QString::fromStdString(outputName);



    // 1. Získaj ukazateľ na QGroupBox (nahraď 'trackingOutputsGroupBox' skutočným menom!)
    QGroupBox *groupBox = ui->trackingOutputsGroupBox;
    if (!groupBox) {
        qWarning() << "ERROR: Could not find the QGroupBox named 'trackingOutputsGroupBox' in the UI.";
        return;
    }

    // 2. Získaj layout z groupboxu. Ak tam ešte nie je, vytvor ho a nastav parametre.
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(groupBox->layout());
    if (!layout) {
        qDebug() << "GroupBox has no layout, creating and configuring a new QVBoxLayout.";
        layout = new QVBoxLayout();

        // --- NASTAVENIE MENŠÍCH MEDZIER A OKRAJOV ---
        layout->setSpacing(2); // Zmenší vertikálnu medzeru MEDZI widgetmi (napr. na 2 pixely)
        layout->setContentsMargins(4, 4, 4, 4); // Zmenší okraje OKOLO layoutu v groupboxe (L,T,R,B)

        groupBox->setLayout(layout);

        // --- PRIDANIE "PRUŽINY" PRE ZAROVNANIE HORE ---
        // Pridaj addStretch iba raz, na koniec layoutu. Toto zabezpečí,
        // že všetky widgety pridané PRED túto pružinu budú tlačené nahor.
        layout->addStretch();
    }
    outputName = outputName + " = value";
    // 3. Vytvor nový QLabel pre zobrazenie mena vstupu
    QLabel *newLabel = new QLabel(QString::fromStdString(outputName), groupBox); // Použi qInputName

    // Create a horizontal layout for the label and delete button
    QHBoxLayout *rowLayout = new QHBoxLayout();

    // Add the label to the row layout
    rowLayout->addWidget(newLabel);

    // Create a delete button for the label
    QPushButton *deleteButton = new QPushButton("x", groupBox);
    deleteButton->setToolTip("Delete this output");

    // Add the delete button to the row layout
    rowLayout->addWidget(deleteButton);

    // Connect the delete button's clicked signal to a lambda function
    connect(deleteButton, &QPushButton::clicked, this, [this, layout, rowLayout, newLabel, deleteButton, outputName]() {
        // Remove the row layout and its widgets
        layout->removeItem(rowLayout);
        delete newLabel;
        delete deleteButton;
        delete rowLayout;

        // Remove the output from the machine
        machine->removeOutput(outputName);
        qDebug() << "Output removed from UI and machine:" << QString::fromStdString(outputName);
    });

    // Add the row layout to the main layout
    layout->insertLayout(0, rowLayout);


    // --- PRIDANIE NOVÉHO LABELU NA ZAČIATOK (HORE) ---
    // Použi insertWidget s indexom 0. Tým sa nový label vloží ako prvý prvok,
    // PRED všetky ostatné widgety (a pred addStretch, ak tam je).
    layout->insertWidget(0, newLabel);

    // /qDebug() << "Inserted QLabel for" << qInputName << "at the top of the layout.";

    // groupBox->update(); // Zvyčajne nie je potrebné volať explicitne, layout sa aktualizuje sám

    // ... (voliteľné scrollovanie) ...
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


    




    // Create a custom event filter and install it on the group
    //QObject* eventFilter = new QObject(this);
    //group->installSceneEventFilter();

    // Connect the position change signal to a custom slot
    connect(scene, &QGraphicsScene::changed, this, [this, group]() {
        if (group->isSelected()) {
            qDebug() << "Group moved. New position:" << group->pos();
            qDebug() << "Moved state ID:" << group->data(0).toInt();
            
            //StateGraphicItem* stateGraphicItem = machine->getStateGraphicItem(group->data(0).toInt());
            //stateGraphicItem->updateTransitions(scene);

            State *state = machine->getState(group->data(0).toInt());
            state->updateTransitionPositions(scene);
        }
    });
   
    //connect(group, &QGraphicsItemGroup::yChanged, this, [this, group]() {
        //qDebug() << "Group moved. New position:" << group->pos();
        // Handle the movement event here
    //});

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


    if(!addingTransitionMode) {
        qDebug() << "handleStateClick called. Not in adding transition mode.";
        return;
    }
    

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
           

            QVariant actualStartPos;
            QVariant actualEndPos;

            QGraphicsItemGroup *transitionGroup = MainWindow::drawArrow(startItemForTransition->sceneBoundingRect().center(), endItemForTransition->sceneBoundingRect().center(), QString::fromStdString(condition.toStdString()), objectTransitionId, scene, &actualStartPos, &actualEndPos);
            
            this->startStateForTransition->addIncomingTransitionGroup(transitionGroup, actualStartPos, actualEndPos);
            this->endStateForTransition->addOutgoingTransitionGroup(transitionGroup, actualStartPos, actualEndPos);
            
            
            
            //scene->addItem(transitionGroup);


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
    

    ProccessMultipleArgsInputEditDialog("Edit State", "Enter new state name:", "Enter new action:",
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

void MainWindow::ProccessMultipleArgsInputEditDialog(const std::string& title, const std::string& label1Text, const std::string& label2Text, 
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
            qDebug() << "Dialog accepted. Values entered:";
            qDebug() << "newInput1 =" << QString::fromStdString(*newInput1);
            qDebug() << "newInput2 =" << QString::fromStdString(*newInput2);
        } else {
            *newInput1 = QString().toStdString();
            *newInput2 = QString().toStdString();
            qDebug() << "Dialog cancelled or closed. Both inputs set to empty strings.";

        }

    // --- Súhrnný výpis na konci ---
    qDebug() << "ProccessMultipleArgsInputEditDialog completed. Final values:";
    qDebug() << "newInput1 =" << QString::fromStdString(*newInput1);
    qDebug() << "newInput2 =" << QString::fromStdString(*newInput2);


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


QGraphicsItemGroup* MainWindow::drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene, QVariant *actualStartPos, QVariant *actualEndPos) {
    // --- Common Arrow Properties ---
    QPen arrowPen(Qt::red, 2); // Pen for the arrow line and head
    qreal arrowSize = 10;      // Size of the arrowhead

    // --- Create a group for the entire transition representation ---
    QGraphicsItemGroup *arrowGroup = new QGraphicsItemGroup();

    // --- Store Transition ID in the group ---
    arrowGroup->setData(0, "Transition"); // Item type identifier
    arrowGroup->setData(1, QVariant(transitionId)); // Store the actual transition ID

    // Check if start and end points are the same (or very close) for self-loop
    if (QLineF(startPos, endPos).length() < 1.0) {
        // --- Draw Self-Loop (Arc) ---
        qDebug() << "Drawing self-loop at" << startPos << "ID:" << transitionId;

        qreal radiusX = 30; // Fixed size loop radius
        qreal radiusY = 30;
        QPointF loopTopCenter(startPos.x(), startPos.y() - radiusY * 1.2);

        QPainterPath loopPath;
        qreal startAngle = 210; qreal spanAngle = -240;
        QRectF arcRect(loopTopCenter.x() - radiusX, loopTopCenter.y() - radiusY, 2 * radiusX, 2 * radiusY);
        loopPath.arcMoveTo(arcRect, startAngle);
        loopPath.arcTo(arcRect, startAngle, spanAngle);
        QGraphicsPathItem* loopItem = new QGraphicsPathItem(loopPath);
        loopItem->setPen(arrowPen);
        arrowGroup->addToGroup(loopItem);

        QPointF loopEnd = loopPath.currentPosition();
        qreal angle = loopPath.angleAtPercent(1.0);
        QPointF arrowLP1 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) + M_PI / 3) * arrowSize, qCos(qDegreesToRadians(-angle) + M_PI / 3) * arrowSize);
        QPointF arrowLP2 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) + M_PI - M_PI / 3) * arrowSize, qCos(qDegreesToRadians(-angle) + M_PI - M_PI / 3) * arrowSize);
        QPolygonF arrowLHead; arrowLHead << loopEnd << arrowLP1 << arrowLP2;
        QGraphicsPolygonItem *arrowLHeadItem = new QGraphicsPolygonItem(arrowLHead);
        arrowLHeadItem->setPen(arrowPen); arrowLHeadItem->setBrush(Qt::red);
        arrowGroup->addToGroup(arrowLHeadItem);

        QGraphicsTextItem* textL = new QGraphicsTextItem(label);
        QRectF textLRect = textL->boundingRect();
        textL->setPos(loopTopCenter.x() - textLRect.width() / 2, loopTopCenter.y() - radiusY - textLRect.height() - 2);
        arrowGroup->addToGroup(textL);

        *actualStartPos = QVariant(startPos);
        *actualEndPos = QVariant(loopEnd);

    } else {
        // --- Draw Curved Arrow ---
        qDebug() << "Drawing curved arrow from" << startPos << "to" << endPos << "ID:" << transitionId;

        QLineF line(startPos, endPos);

        QPainterPath curvePath;
        curvePath.moveTo(startPos);

        QPointF midPoint = line.pointAt(0.5);
        QPointF delta = endPos - startPos;
        QPointF perp(delta.y(), -delta.x());
        qreal perpLength = QLineF(QPointF(0,0), perp).length();
        QPointF normPerp = (perpLength > 0) ? (perp / perpLength) : QPointF(0, -1);
        qreal curveFactor = (endPos.x() > startPos.x()) ? -1.0 : 1.0;
        qreal curveMagnitude = qMin(line.length() * 0.2, 40.0);
        QPointF controlPoint = midPoint + normPerp * curveMagnitude * curveFactor;

        curvePath.quadTo(controlPoint, endPos);

        QGraphicsPathItem* curveItem = new QGraphicsPathItem(curvePath);
        curveItem->setPen(arrowPen);
        arrowGroup->addToGroup(curveItem);

        QLineF endSegment(controlPoint, endPos);
        qreal angleDeg = endSegment.angle();
        double angleRad = qDegreesToRadians(-angleDeg);

        QPointF arrowP1 = endPos + QPointF(cos(angleRad - M_PI / 6.0) * arrowSize,
                                           sin(angleRad - M_PI / 6.0) * arrowSize);
        QPointF arrowP2 = endPos + QPointF(cos(angleRad + M_PI / 6.0) * arrowSize,
                                           sin(angleRad + M_PI / 6.0) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << endPos << arrowP1 << arrowP2;
        QGraphicsPolygonItem *arrowHeadItem = new QGraphicsPolygonItem(arrowHead);
        arrowHeadItem->setPen(arrowPen);
        arrowHeadItem->setBrush(Qt::red);
        arrowGroup->addToGroup(arrowHeadItem);

        QGraphicsTextItem* text = new QGraphicsTextItem(label);
        QPointF labelPos = curvePath.pointAtPercent(0.5);
        text->setPos(labelPos + normPerp * (curveFactor > 0 ? 10 : -15));
        arrowGroup->addToGroup(text);

        *actualStartPos = QVariant(startPos);
        *actualEndPos = QVariant(endPos);
    }

    arrowGroup->setFlag(QGraphicsItem::ItemIsSelectable);
    scene->addItem(arrowGroup);
    qDebug() << "Arrow/Loop group added to scene for transition ID:" << transitionId;
    return arrowGroup;
}







