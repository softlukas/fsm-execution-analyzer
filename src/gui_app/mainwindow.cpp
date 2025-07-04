/**
 * @file mainwindow.cpp
 * @brief Implementation file for the MainWindow class.
 * @details This file contains the implementation of the MainWindow class, which manages the main GUI application.
 * It includes functionality for handling user interactions, managing the automaton model, and updating the graphics scene.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */

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
#include <string>       
#include <memory>       
#include <QDebug>       
#include <QString>      
#include <QLabel>       
#include <QVBoxLayout>  
#include <QGroupBox>   
#include <QCoreApplication> 
#include <QDir>
#include "persistence/JsonPersistance.h" 
#include "codegen/CodeGenerator.h"
#include <fstream>
#include <QProcess>
#include <QUdpSocket> 
#include <QNetworkDatagram> 
#include <QFileDialog> 
#include <QStandardPaths> 
#include <QLayout>
#include <chrono> 
#include <thread> 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Automat Creator");
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 2000, 1500);

    
    GraphicsView* gView = ui->graphicsView; 

    if(gView) {
        gView->setScene(scene);
        
        qDebug() << "Graphics scene created and assigned to graphicsView.";
        // --- Connect custom view's signal to our slot ---
        connect(gView, &GraphicsView::stateItemClicked, this, &MainWindow::handleStateClick);
        connect(gView, &GraphicsView::stateItemRightClicked, this, &MainWindow::handleRightClick);
        qDebug() << "Connected graphicsView stateItemClicked signal.";
    } else {
        qWarning() << "WARNING: QGraphicsView named 'graphicsView' not found or not promoted to GraphicsView in UI file.";
    }
    if (gView) {
        gView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); // Default
        gView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);   // Default

        
    }
    // Create a Machine object using the Machine constructor
    std::string machineName = "";
    while (machineName.empty()) { 
        machineName = MainWindowUtils::ProccessOneArgumentDialog("Set automat name");
        if (machineName.empty()) {
            QMessageBox::warning(this, "Name Required", "Automaton name cannot be empty. Please provide a name.");
        }
    }
    
    setInputFieldsEnabled(false); // start state is inactive


    machine = new Machine(machineName);

    
    qDebug() << "Machine object created successfully.";


    // --- OBTAINING PORTS WITH NUMBER VALIDATION ---
    bool portsValid = false;
    std::string guiPortStr = "9000"; // Default value
    std::string automatonPortStr = "9001"; // Default value

    while (!portsValid) {
        std::string tempGuiPort = "";
        std::string tempAutomatonPort = "";

        // Display dialog (pre-fill with last valid or default values)
        ProccessMultipleArgsInputEditDialog("ADD PORTS",
                                            "Add port GUI (1024-65535):",
                                            "Add port AUTOMAT (1024-65535):",
                                            guiPortStr,         // Pre-fill last/default value
                                            automatonPortStr,   // Pre-fill last/default value
                                            &tempGuiPort,       // Output for new GUI port
                                            &tempAutomatonPort);// Output for new Automat port

        // Check if the user canceled the dialog (both values would be empty according to ProccessMultipleArgs...)
         if (tempGuiPort.empty() && tempAutomatonPort.empty()) {
             // If we want to terminate the application when ports are canceled, we can do it here
             // e.g., qApp->quit(); or throw an exception, or set an error state
              QMessageBox::critical(this, "Ports Required", "Valid port numbers are required to continue.");
             // In this example, we will continue the loop and ask again
             continue; // Back to the start of the while loop
         }


        // GUI Port Validation
        bool guiPortOk = false;
        int guiPortInt = -1;
        try {
            size_t processedChars = 0;
            guiPortInt = std::stoi(tempGuiPort, &processedChars);
            // Check if the entire number was processed and if it is within range
            if (processedChars == tempGuiPort.length() && guiPortInt >= 1024 && guiPortInt <= 65535) {
                guiPortOk = true;
            }
        } catch (const std::invalid_argument& e) {
            // std::stoi throws if it's not a number
        } catch (const std::out_of_range& e) {
            // std::stoi throws if the number is out of int range
        }

        // Automaton Port Validation
        bool automatonPortOk = false;
        int automatonPortInt = -1;
         try {
            size_t processedChars = 0;
            automatonPortInt = std::stoi(tempAutomatonPort, &processedChars);
            if (processedChars == tempAutomatonPort.length() && automatonPortInt >= 1024 && automatonPortInt <= 65535) {
                 // Additional check: Ports must not be the same
                 if (automatonPortInt != guiPortInt) {
                     automatonPortOk = true;
                 }
            }
        } catch (const std::invalid_argument& e) {
        } catch (const std::out_of_range& e) {
        }

        // Check validation results
        if (guiPortOk && automatonPortOk) {
            portsValid = true; // Everything is fine, exit the loop
            // Save valid values to member variables
            this->portGUI = tempGuiPort;
            this->portAutomat = tempAutomatonPort;
            qInfo() << "Valid ports entered: GUI=" << QString::fromStdString(this->portGUI)
                    << ", Automaton=" << QString::fromStdString(this->portAutomat);
        } else {
            // Display error message
            QString errorMsg = "Invalid input:\n";
            if (!guiPortOk) {
                errorMsg += "- GUI Port must be a number between 1024 and 65535.\n";
            }
            if (!automatonPortOk) {
                 if (automatonPortInt == guiPortInt && guiPortOk) { // Special case of identical ports
                      errorMsg += "- Automaton Port must be different from GUI Port.\n";
                 } else {
                      errorMsg += "- Automaton Port must be a number between 1024 and 65535.\n";
                 }
            }
            QMessageBox::warning(this, "Invalid Ports", errorMsg);
            // Keep the last valid/default values in guiPortStr and automatonPortStr for the next iteration
            guiPortStr = tempGuiPort; // Show the user what they entered incorrectly
            automatonPortStr = tempAutomatonPort;
        }
    } // End of while (!portsValid)

    guiSocket_ = new QUdpSocket(this);
    connect(guiSocket_, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
    bindGuiSocket(); // Ensure the socket is listening
    
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_setInitialStateButton_clicked() {    
    qDebug() << "Run Automaton button clicked.";

    if (!machine) {
        QMessageBox::critical(this, "Error", "No automaton model loaded or created.");
        return;
    }

    std::string automatonInitialStateName; // Renamed for clarity
    bool initialStateSet = false;

    while (!initialStateSet) { // Repeat until successfully set
        automatonInitialStateName = MainWindowUtils::ProccessOneArgumentDialog("Enter automaton starting state name:");

        if (automatonInitialStateName.empty()) {
             // User pressed Cancel in the name dialog
             QMessageBox::warning(this, "Cancelled", "Starting state selection cancelled.");
             return; // Exit the function if the user cancels state selection
        }

        // Attempt to set the initial state in the model
        try {
            machine->setInitialState(automatonInitialStateName);
            qInfo() << "Initial state set to:" << QString::fromStdString(automatonInitialStateName);
            initialStateSet = true; // Success, exit the loop

        } catch (const std::runtime_error& e) {
            // Catch specific exception from Machine::setInitialState
            qWarning() << "Error setting initial state:" << e.what();
            QMessageBox::warning(this, "Invalid State", QString("Could not set initial state:\n%1\nPlease enter a valid state name.").arg(e.what()));
            // The while loop will run again and prompt the user
        } catch (const std::exception& e) {
             // Catch other unexpected errors
             qCritical() << "Unexpected error setting initial state:" << e.what();
             QMessageBox::critical(this, "Error", QString("An unexpected error occurred: %1").arg(e.what()));
             return;
        }
    }

}


void MainWindow::on_runAutomatButton_clicked() {

    qDebug() << "Run Automaton button clicked.";

    
    if (!machine) {
        QMessageBox::critical(this, "Error", "No automaton model loaded or created.");
        return;
    }
    const State* automatonInitialState = machine->getInitialState();

    if (automatonInitialState == nullptr) {
        QMessageBox::warning(this, "Cancelled", "Automaton Initial State Is Not Set.");
        return;
    }
    

    QString automatonName = QString::fromStdString(machine->getName());
    
    QString safeDirName = automatonName;
    safeDirName.replace(QRegExp("[^a-zA-Z0-9_.-]"), "_"); // Create a safe name for the directory

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

    // --- Create Target Directory ---
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

    // --- Save JSON Definition to Target Directory ---
    QString jsonPath = targetAutomatonDir + "/" + safeDirName + ".json";
    qDebug() << "Saving current model to JSON:" << jsonPath;
    if (!JsonPersistence::saveToFile(*machine, jsonPath.toStdString())) { // Use the correct class for saving
         QMessageBox::critical(this, "Error", "Failed to save automaton model to JSON file.");
         return;
    }

    // --- Generate C++ Code to Target Directory ---
    QString generatedCppPath = targetAutomatonDir + "/" + safeDirName + "_generated.cpp";
    QString templatePath = projectPath + "/templates/automation_template.tpl"; // Path to the template

    qDebug() << "Generating C++ code from template:" << templatePath << "to:" << generatedCppPath;
    try {
        CodeGenerator generator(templatePath.toStdString());
        // Assuming generate takes a Machine object and internally converts it to JSON
        // Or if CodeGenerator reads JSON, it would read jsonPath
        std::string cpp_code = generator.generate(*machine, jsonPath.toStdString());

        std::ofstream cppFile(generatedCppPath.toStdString());
        if (!cppFile) {
             throw CodeGenerator::GenerationError("Failed to open output C++ file for writing: " + generatedCppPath.toStdString());
        }
        cppFile << cpp_code;
        cppFile.close();
         if (!cppFile) { // Check after closing
             throw CodeGenerator::GenerationError("Failed to write to output C++ file: " + generatedCppPath.toStdString());
         }
         qInfo() << "C++ code generated successfully.";

         QMessageBox::information(this, "Success", "Automaton JSON and C++ code generated successfully in:\n" + targetAutomatonDir);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Code Generation Error", QString("Failed to generate C++ code: %1").arg(e.what()));
        // Optionally delete incomplete files if an error occurred
        QFile::remove(generatedCppPath);
        // QFile::remove(jsonPath); // JSON was already successfully saved
        return;
    }







    // --- Compile C++ Code using g++ ---
    QString executablePath = targetAutomatonDir + "/" + safeDirName + "_automaton"; // Output file without extension for Linux

    qDebug() << "Compiling" << generatedCppPath << "to" << executablePath;

    QString runtimeDir = projectPath + "/src/runtime";
    QString asioInclude = projectPath + "/third_party/asio/include";

    QString compiler = "g++"; // Assuming g++ is in PATH
    QStringList arguments;
    arguments << "-std=c++17";
    arguments << "-I" + runtimeDir;         // Path to ifa_runtime headers
    arguments << "-I" + asioInclude;        // Path to Asio headers
    arguments << generatedCppPath;          // Input generated file
    // Output executable file
    arguments << "-o" << executablePath;
    arguments << "-L" + runtimeLibDir;
    arguments << "-lifa_runtime";
    // Linker flags for Linux
    arguments << "-lpthread";
    arguments << "-lstdc++fs"; // Required for std::filesystem

    QProcess compilerProcess;
    qDebug() << "Compiler command:" << compiler << arguments.join(" ");
    compilerProcess.setWorkingDirectory(projectPath); // Set working directory to project root
    compilerProcess.start(compiler, arguments);

    // Wait for compilation to complete and check for errors
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
        // Display error in a window
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Compilation Failed");
        msgBox.setText(QString("Compiler exited with code %1.").arg(compilerProcess.exitCode()));
        msgBox.setDetailedText("Standard Output:\n" + stdOutput + "\n\nError Output:\n" + errorOutput);
        msgBox.exec();
        return; // Compilation error
    }

    qInfo() << "Compilation successful: " << executablePath;
    QMessageBox::information(this, "Compilation Successful", "Automaton compiled successfully:\n" + executablePath);

    // --- Run the compiled automaton (Detached) ---
    qDebug() << "portAutomat:" << QString::fromStdString(portAutomat);
    qDebug() << "portGUI:" << QString::fromStdString(portGUI);

    QStringList automatonArgs;
    automatonArgs << QString::number(std::stoi(portAutomat));

    automatonArgs << QString::number(std::stoi(portGUI));


    qDebug() << "Starting automaton process detached:" << executablePath << automatonArgs.join(" ");

    qint64 pid = 0; // Variable to store PID
    qDebug() << "Arguments passed to automaton:" << automatonArgs;

    // Use QProcess::startDetached, which returns PID
    if (QProcess::startDetached(executablePath, automatonArgs, targetAutomatonDir, &pid)) { // Fourth argument receives PID
        qInfo() << "Automaton process" << automatonName << "started successfully (detached) with PID:" << pid;
        QMessageBox::information(this, "Started", "Automaton '" + automatonName + "' started (PID: " + QString::number(pid) + ").\nListen Port: " + QString::number(std::stoi(portAutomat)) + "\nGUI Port: " + QString::number(std::stoi(portGUI)));
        setInputFieldsEnabled(true);
        

    } else {
        qCritical() << "Failed to start detached automaton process.";
        QMessageBox::critical(this, "Process Error", "Could not start the automaton process.");
        setInputFieldsEnabled(false);
        
    }

    qDebug() << "Starting automaton on PID:" << pid;

    
}

void MainWindow::processPendingDatagrams()
{
    while (guiSocket_->hasPendingDatagrams()) {
        // Modern way of reading (recommended)
        QNetworkDatagram datagram = guiSocket_->receiveDatagram();
        if (!datagram.isValid()) {
            continue; // Skip invalid datagram
        }
        QString message = QString::fromUtf8(datagram.data());
        QHostAddress senderAddress = datagram.senderAddress();
        quint16 senderPort = datagram.senderPort();

        // Log received message
        qDebug() << "Received from" << senderAddress.toString() << ":" << senderPort << "->" << message;

        // --- Parsing and Processing Message ---

        if (message.startsWith("NAME ")) { // <<< HANDLE NEW MESSAGE
            QString receivedName = message.mid(5).trimmed();
            qInfo() << "Received NAME:" << receivedName;

            if (waitingForAutomatonInfo) {
                // Optional: Stop timeout timer if used
                // if (connectionTimeoutTimer) connectionTimeoutTimer->stop();

                connectedAutomatonName = receivedName; // Store the name
                waitingForAutomatonInfo = false; // No longer waiting for initial info

                // --- Construct JSON Path ---
                QString safeDirName = connectedAutomatonName;
                safeDirName.replace(QRegExp("[^a-zA-Z0-9_.-]"), "_");
                QString appDirPath = QCoreApplication::applicationDirPath();
                QDir buildSubDir(appDirPath);
                buildSubDir.cdUp(); // Assumes build dir is one level down
                buildSubDir.cdUp(); // Navigate up twice to reach project root (adjust if needed)
                buildSubDir.cdUp(); // Navigate up twice to reach project root (adjust if needed)

                QString projectPath = buildSubDir.absolutePath();
                QString jsonPath = projectPath + "/generated_automatons/" + safeDirName + "/" + safeDirName + ".json";
                qDebug() << "Looking for JSON definition at:" << jsonPath;

                // --- Check and Load JSON ---
                if (!QFile::exists(jsonPath)) {
                    qCritical() << "Automaton definition file not found:" << jsonPath;
                    QMessageBox::critical(this, "Error", "Connected to automaton '" + connectedAutomatonName + "', but its definition file was not found:\n" + jsonPath);
                    connectedAutomatonName = ""; // Reset connection state
                    return; // Stop processing this datagram stream for now
                }

                std::unique_ptr<Machine> loadedMachine = JsonPersistence::loadFromFile(jsonPath.toStdString());

                // Check if loading was successful
                if (!loadedMachine) {
                    QMessageBox::critical(this, "Load Failed", "Could not load or parse the automaton model from the specified file.\nCheck console output for details.");
                    qCritical() << "Failed to load machine from:" << jsonPath;
                    return; // Error during loading
                }

                qInfo() << "Successfully loaded machine '" << QString::fromStdString(loadedMachine->getName()) << "' from JSON.";

                // --- START OF GUI AND MODEL RESET ---
                qDebug() << "Preparing to switch models. Clearing old GUI elements and model...";

                // Disconnect old scene 'changed' listeners and clear Graphics Scene
                for (const auto& conn : stateMoveConnections) {
                    QObject::disconnect(conn);
                }
                stateMoveConnections.clear();
                qDebug() << "Disconnected old state movement scene listeners.";

                scene->clear();
                qDebug() << "Graphics scene cleared.";

                // Clear UI Lists (Variables, Inputs, Outputs) by resetting their QGroupBox layouts
                clearVariableList();
                clearInputList();
                clearOutputList();
                qDebug() << "UI lists (variables, inputs, outputs) cleared by resetting group box layouts.";

                // Delete the old Machine object, if it exists
                if (machine) {
                    delete machine;
                    machine = nullptr;
                    qDebug() << "Old machine model deleted.";
                }

                // Take ownership of the new Machine object
                machine = loadedMachine.release();
                qDebug() << "Took ownership of the newly loaded machine object: " << QString::fromStdString(machine->getName());
                // --- END OF GUI AND MODEL RESET ---

                // --- Populate GUI with new model ---
                qDebug() << "Redrawing automaton and populating UI from the new model...";
                redrawAutomatonFromModel(); // Redraws states and transitions on the scene
                populateUIFromModel();      // Populates QGroupBoxes for vars, ins, outs

                setInputFieldsEnabled(true); // Default for a newly loaded, non-connected automaton
                
                // Update global ID counters for NEW items to be added via GUI
                int maxStateIdEncountered = -1;
                if (machine) {
                    for (const auto& state_pair : machine->getStates()) {
                        if (state_pair.second && state_pair.second->getStateId() > maxStateIdEncountered) {
                            maxStateIdEncountered = state_pair.second->getStateId();
                        }
                    }
                    this->objectStateId = maxStateIdEncountered + 1;

                    int maxTransIdEncountered = -1;
                    for (const auto& trans_ptr : machine->getTransitions()) {
                        if (trans_ptr && trans_ptr->getTransitionId() > maxTransIdEncountered) {
                            maxTransIdEncountered = trans_ptr->getTransitionId();
                        }
                    }
                    this->objectTransitionId = maxTransIdEncountered + 1;
                    qDebug() << "MainWindow ID counters updated: nextStateId=" << this->objectStateId
                            << ", nextTransitionId=" << this->objectTransitionId;
                }

            } else {
                // Received NAME unexpectedly (already connected?) - maybe just log
                 qWarning() << "Received NAME message while not expecting it (already connected?). Name:" << receivedName;
            }
        }else if (message.startsWith("READY ")) {
            QString automatonName = message.mid(6);
            qInfo() << "Automaton" << automatonName << "is READY.";
            

        } else if (message.startsWith("STATE ")) {
            QString activeStateName = message.mid(6).trimmed();
            qInfo() << "Automaton entered state:" << activeStateName;
           
            // Reset color of ALL states to normal
            GraphicsView* gView = ui->graphicsView;
            QGraphicsScene* scene = gView->scene();

            // Iterate through all items in the scene
            for (QGraphicsItem* item : scene->items()) {
                QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(item);
                if (ellipse) {
                    QString ellipseName = ellipse->data(0).toString();  // Read state name
                    if (ellipseName == activeStateName) {
                        ellipse->setBrush(QBrush(activeStateColor));
                    } else {
                        ellipse->setBrush(QBrush(normalStateColor));
                    }
                }
            }

        } else if (message.startsWith("OUTPUT ")) {
            // Parsing: OUTPUT output_name="value"
            QString dataPart = message.mid(7); // Remove "OUTPUT "
            int assignmentPos = dataPart.indexOf('=');
            if (assignmentPos != -1) {
                QString outputName = dataPart.left(assignmentPos).trimmed();
                QString outputValue = dataPart.mid(assignmentPos + 1).trimmed();
                // Remove potential quotes around the value
                if (outputValue.length() >= 2 && outputValue.startsWith('"') && outputValue.endsWith('"')) {
                    outputValue = outputValue.mid(1, outputValue.length() - 2);
                }
                qInfo() << "Received OUTPUT:" << outputName << "=" << outputValue;
                
                updateOutputDisplay(outputName.toStdString(), outputValue.toStdString());



            } else {
                qWarning() << "Malformed OUTPUT message:" << message;
            }

        } else if (message.startsWith("VAR ")) {
            // Parsing: VAR var_name="value" (same as OUTPUT)
             QString dataPart = message.mid(4); // Remove "VAR "
            int assignmentPos = dataPart.indexOf('=');
            if (assignmentPos != -1) {
                QString varName = dataPart.left(assignmentPos).trimmed();
                QString varValue = dataPart.mid(assignmentPos + 1).trimmed();
                if (varValue.length() >= 2 && varValue.startsWith('"') && varValue.endsWith('"')) {
                    varValue = varValue.mid(1, varValue.length() - 2);
                }
                qInfo() << "Received VAR update:" << varName << "=" << varValue;
                Variable* variable = machine->getVariable(varName.toStdString());
                if (variable) {
                    variable->setValue(varValue.toStdString());
                }

                // NOW SAVE TO GUI:
                updateVariableDisplay(varName.toStdString(), varValue.toStdString());

            } else {
                qWarning() << "Malformed VAR message:" << message;
            }

        } else if (message.startsWith("LOG ")) {
            QString logMsg = message.mid(4); // Get text after "LOG "
            qInfo() << "[Automaton LOG]" << logMsg;

            // Special handling for DEFINITION_PATH (if using this solution)
            if (logMsg.startsWith("DEFINITION_PATH:")) {
                QString jsonPath = logMsg.mid(16); // Remove "DEFINITION_PATH:"
                qInfo() << "Received automaton definition path:" << jsonPath;
                
            } else {
                 
            }

        } else if (message.startsWith("ERROR ")) {
            QString errorMsg = message.mid(6); // Get text after "ERROR "
            qWarning() << "[Automaton ERROR]" << errorMsg;
            
            QMessageBox::warning(this, "Automaton Error", errorMsg);


        } else if (message == "TERMINATING") {
            qInfo() << "Automaton is TERMINATING.";
           
            QMessageBox::information(this, "Automaton", "Automaton terminating...");
            setInputFieldsEnabled(false);
            

            for (QGraphicsItem* item : scene->items()) {
                QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(item);
                if (ellipse) {
                    QString ellipseName = ellipse->data(0).toString();  // Read state name
                    ellipse->setBrush(QBrush(normalStateColor));
                }
            }

        } else {
            qWarning() << "Received unknown message format from runtime:" << message;
        }
    }
}

void MainWindow::updateVariableDisplay(const std::string& varName, const std::string& newValue) {
    QLineEdit* valueEdit = ui->editVarGroupBox->findChild<QLineEdit*>(QString::fromStdString(varName));
    if (valueEdit) {
        valueEdit->setText(QString::fromStdString(newValue));
        qDebug() << "Updated GUI value for variable:" << QString::fromStdString(varName);
    } else {
        qWarning() << "QLineEdit not found for variable:" << QString::fromStdString(varName);
    }
}

void MainWindow::updateOutputDisplay(const std::string& outputName, const std::string& newValue) {
    qInfo() << "updateOutputDisplay COOOOOOOOOOOOOOOOOOOOOOOO:";

    QLabel* outputLabel = ui->trackingOutputsGroupBox->findChild<QLabel*>(QString::fromStdString(outputName));
    if (outputLabel) {
        QString newText = QString::fromStdString(outputName) + " = " + QString::fromStdString(newValue);
        outputLabel->setText(newText);
        qDebug() << "Updated OUTPUT label:" << QString::fromStdString(outputName) << "->" << newText;
    } else {
        qWarning() << "QLabel not found for output:" << QString::fromStdString(outputName);
    }
}


void MainWindow::bindGuiSocket() {
    
    int guiListenPort_ = std::stoi(portGUI);

    guiSocket_->close();
    if (guiSocket_->bind(QHostAddress::AnyIPv4, guiListenPort_)) { 
        qInfo() << "GUI Socket bound successfully to port" << guiListenPort_;
    } else {
        qWarning() << "Failed to bind GUI socket to port" << guiListenPort_ << ":" << guiSocket_->errorString();
        QMessageBox::warning(this, "Socket Error", "Could not listen on port " + QString::number(guiListenPort_) + ".\n" + guiSocket_->errorString());
    }
}

void MainWindow::updateStateItemColor(QGraphicsItemGroup* item, const QColor& color) {
    if (!item) return;
    for (QGraphicsItem* child : item->childItems()) {
        QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(child);
        if (ellipse) {
            ellipse->setBrush(QBrush(color));
            return;
        }
    }
    
}

void MainWindow::on_terminateAutomatButton_clicked(){
    if (!guiSocket_) {
        qWarning() << "Cannot send TERMINATE message: guiSocket_ is null.";
        return;
    }

    QString message = "CMD|TERMINATE|";
    QHostAddress runtimeAddr("127.0.0.1");
    quint16 port = static_cast<quint16>(std::stoi(portAutomat)); // portAutomat už máš

    qint64 bytesSent = guiSocket_->writeDatagram(message.toUtf8(), runtimeAddr, port);

    if (bytesSent == -1) {
        qWarning() << "Failed to send TERMINATE datagram:" << guiSocket_->errorString();
        QMessageBox::warning(this, "Network Error", "Failed to send TERMINATE message to automaton:\n" + guiSocket_->errorString());
    } else {
        qDebug() << "Sent TERMINATE command to automaton.";
    }
}

void MainWindow::on_connectButton_clicked(){
    qDebug() << "Connect button clicked.";

    if (scene) { // Ensure the scene exists
        scene->clearSelection();
        qDebug() << "Cleared any existing selection on the scene.";
    }

    // --- Get Ports from User ---
    
    bool ok1, ok2;
    int tempGuiPort = QInputDialog::getInt(this, "GUI Port", "Enter port GUI is listening on:", std::stoi(portGUI), 1024, 65535, 1, &ok1);
    if (!ok1) return; // User cancelled

    int tempAutomatonPort = QInputDialog::getInt(this, "Automaton Port", "Enter port Automaton is listening on:", std::stoi(portAutomat), 1024, 65535, 1, &ok2);
    if (!ok2) return; // User cancelled

    // Update member variables if they changed
    bool portChanged = false;
    if (std::to_string(tempGuiPort) != portGUI) {
        portGUI = std::to_string(tempGuiPort);
        portChanged = true;
        qInfo() << "GUI listen port updated to:" << QString::fromStdString(portGUI);
    }
    if (std::to_string(tempAutomatonPort) != portAutomat) {
        portAutomat = std::to_string(tempAutomatonPort);
        qInfo() << "Target automaton port updated to:" << QString::fromStdString(portAutomat);
        
    }

    // Rebind GUI socket if its port changed
    if (portChanged) {
        bindGuiSocket();
        if (!guiSocket_->isValid()) { // Check if binding failed
             QMessageBox::critical(this, "Error", "Failed to bind GUI socket to new port. Cannot connect.");
             return;
        }
    }

    // --- Clear Current State (Optional: Add confirmation) ---
    if (machine) { // If a machine is already loaded/running visualization
        
        qDebug() << "Clearing existing automaton view and model.";
        scene->clear(); // Clear graphics
        
        delete machine; // Delete the old machine object
        machine = nullptr;
        connectedAutomatonName = "";
        
    }

    // --- Send GET_STATUS Command ---
    if (!guiSocket_) {
        qWarning() << "Cannot send GET_STATUS: guiSocket_ is null.";
        QMessageBox::critical(this, "Error", "Socket not initialized.");
        return;
    }

    QString message = "CMD|GET_STATUS|";
    QHostAddress runtimeAddr("127.0.0.1"); // Assuming localhost
    quint16 targetPort = static_cast<quint16>(std::stoi(portAutomat));

    qInfo() << "Sending GET_STATUS to" << runtimeAddr.toString() << ":" << targetPort;
    qint64 bytesSent = guiSocket_->writeDatagram(message.toUtf8(), runtimeAddr, targetPort);

    if (bytesSent == -1) {
        qWarning() << "Failed to send GET_STATUS datagram:" << guiSocket_->errorString();
        QMessageBox::warning(this, "Network Error", "Failed to send GET_STATUS message:\n" + guiSocket_->errorString());
    } else {
        qDebug() << "Sent GET_STATUS command.";
        waitingForAutomatonInfo = true; // Set flag to wait for NAME response
       
    }
}






void MainWindow::on_saveJsonButton_clicked() {
    qDebug() << "Save JSON button clicked.";

    // Check if there is a model to save
    if (!machine) {
        QMessageBox::warning(this, "No Automaton", "There is no automaton model loaded or created to save.");
        qWarning() << "Save attempted but no machine model exists.";
        return;
    }

    // Prepare the file name and path
    // Use the automaton name as the default file name
    QString defaultFileName = QString::fromStdString(machine->getName()) + ".json";
    // Remove invalid characters for the file name (less strict than for directories)
    defaultFileName.replace(QRegExp("[\\\\/:*?\"<>|]"), "_");

    
    QString safeDirName = QString::fromStdString(machine->getName());
    safeDirName.replace(QRegExp("[^a-zA-Z0-9_.-]"), "_");
    QString appDirPath = QCoreApplication::applicationDirPath();
    QDir buildSubDir(appDirPath);
    buildSubDir.cdUp(); // Assume build directory
    buildSubDir.cdUp(); // To project root
    buildSubDir.cdUp(); // To project root
    QString projectPath = buildSubDir.absolutePath();
    QString targetBaseDir = projectPath + "/generated_automatons";
    QString targetAutomatonDir = targetBaseDir + "/" + safeDirName;
    QString defaultPath = targetAutomatonDir + "/" + defaultFileName;

    

    // Show "Save As..." dialog
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save Automaton As", // Dialog title
                                                    defaultPath,         // Default path and file
                                                    "JSON Files (*.json);;All Files (*)"); // File filter

    // Check if the user selected a file
    if (fileName.isEmpty()) {
        qDebug() << "Save cancelled by user.";
        return; // User pressed Cancel or closed the dialog
    }

    // Ensure the file has a .json extension (optional but recommended)
     if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
         fileName += ".json";
     }
     qInfo() << "Saving automaton model to:" << fileName;

    // Call the save function from JsonPersistence
    bool success = JsonPersistence::saveToFile(*machine, fileName.toStdString());

    // Inform the user about the result
    if (success) {
        QMessageBox::information(this, "Save Successful", "Automaton model saved successfully to:\n" + fileName);
        qInfo() << "Automaton model saved successfully.";
        
    } else {
        QMessageBox::critical(this, "Save Failed", "Could not save the automaton model to the specified file.\nCheck console output for details.");
        qCritical() << "Failed to save automaton model to:" << fileName;
    }
}


void MainWindow::on_loadJsonButton_clicked() {
    qDebug() << "Load JSON button clicked.";
    
    if (scene) { // Ensure the scene exists
        scene->clearSelection();
        qDebug() << "Cleared any existing selection on the scene.";
    }

    GraphicsView* gView = ui->graphicsView; // Use the correct type

    if(gView) {
        gView->ensureVisible(QRectF(0, 0, 1, 1), 0, 0);
    }
    // Prepare default path (e.g., project directory or last used)
    
    QString appDirPath = QCoreApplication::applicationDirPath();
    QDir buildSubDir(appDirPath);
    buildSubDir.cdUp(); // Assume build directory
    buildSubDir.cdUp(); // To project root
    buildSubDir.cdUp(); // To project root
    QString projectPath = buildSubDir.absolutePath();
    QString defaultDir = projectPath; // Or /examples

    

    // Show "Open File..." dialog
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Automaton from JSON", // Dialog title
                                                    defaultDir,                 // Default directory
                                                    "JSON Files (*.json);;All Files (*)"); // File filter

    // Check if the user selected a file
    if (fileName.isEmpty()) {
        qDebug() << "Load cancelled by user.";
        return; // User pressed Cancel or closed the dialog
    }

    qInfo() << "Attempting to load automaton model from:" << fileName;

    // Call the function to load from JsonPersistence
    std::unique_ptr<Machine> loadedMachine = JsonPersistence::loadFromFile(fileName.toStdString());

    // Check if loading was successful
    if (!loadedMachine) {
        QMessageBox::critical(this, "Load Failed", "Could not load or parse the automaton model from the specified file.\nCheck console output for details.");
        qCritical() << "Failed to load machine from:" << fileName;
        return; // Error during loading
    }

    qInfo() << "Successfully loaded machine '" << QString::fromStdString(loadedMachine->getName()) << "' from JSON.";

    // --- START OF GUI AND MODEL RESET ---
    qDebug() << "Preparing to switch models. Clearing old GUI elements and model...";

    // Disconnect old scene 'changed' listeners and clear Graphics Scene
    for (const auto& conn : stateMoveConnections) {
        QObject::disconnect(conn);
    }
    stateMoveConnections.clear();
    qDebug() << "Disconnected old state movement scene listeners.";

    scene->clear();
    qDebug() << "Graphics scene cleared.";

    // Clear UI Lists (Variables, Inputs, Outputs) by resetting their QGroupBox layouts
    clearVariableList();
    clearInputList();
    clearOutputList();
    qDebug() << "UI lists (variables, inputs, outputs) cleared by resetting group box layouts.";

    // Delete the old Machine object, if it exists
    if (machine) {
        delete machine;
        machine = nullptr;
        qDebug() << "Old machine model deleted.";
    }

    // Take ownership of the new Machine object
    machine = loadedMachine.release();
    qDebug() << "Took ownership of the newly loaded machine object: " << QString::fromStdString(machine->getName());
    // --- END OF GUI AND MODEL RESET ---

    // --- Populate GUI with new model ---
    qDebug() << "Redrawing automaton and populating UI from the new model...";
    redrawAutomatonFromModel(); // Redraws states and transitions on the scene
    populateUIFromModel();      // Populates QGroupBoxes for vars, ins, outs

    setInputFieldsEnabled(false); // Default for a newly loaded, non-connected automaton

    // Update global ID counters for NEW items to be added via GUI
    int maxStateIdEncountered = -1;
    if (machine) {
        for (const auto& state_pair : machine->getStates()) {
            if (state_pair.second && state_pair.second->getStateId() > maxStateIdEncountered) {
                maxStateIdEncountered = state_pair.second->getStateId();
            }
        }
        this->objectStateId = maxStateIdEncountered + 1;

        int maxTransIdEncountered = -1;
        for (const auto& trans_ptr : machine->getTransitions()) {
            if (trans_ptr && trans_ptr->getTransitionId() > maxTransIdEncountered) {
                maxTransIdEncountered = trans_ptr->getTransitionId();
            }
        }
        this->objectTransitionId = maxTransIdEncountered + 1;
        qDebug() << "MainWindow ID counters updated: nextStateId=" << this->objectStateId
                 << ", nextTransitionId=" << this->objectTransitionId;
    }

    QMessageBox::information(this, "Load Successful", "Automaton model loaded successfully from:\n" + fileName);
}

void MainWindow::redrawAutomatonFromModel() {

    // Function to find the ellipse and its center in scene coordinates
    auto getVisualCenterOfStateItem = [](QGraphicsItemGroup* stateItemGroup) -> QPointF {
        if (!stateItemGroup) return QPointF(); // If null, return a null point

        for (QGraphicsItem* child : stateItemGroup->childItems()) {
            if (QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(child)) {
                
                return stateItemGroup->mapToScene(ellipse->rect().center());
            }
        }
        // Fallback if the ellipse is not found (should not occur with correct structure)
        qWarning() << "Ellipse not found in state group, using sceneBoundingRect().center() as fallback.";
        return stateItemGroup->sceneBoundingRect().center();
    };

    // Basic checks and cleanup
    if (!machine) {
        qWarning() << "Cannot redraw: machine model is null.";
        return;
    }
    if (!scene) {
         qWarning() << "Cannot redraw: scene is null.";
         return;
    }
    qDebug() << "Redrawing automaton '" << QString::fromStdString(machine->getName()) << "' from loaded model.";
    scene->clear(); // Clear the scene of old items
   

    // Retrieve states and calculate layout
    const auto& statesMap = machine->getStates();
    int stateCount = statesMap.size();
    if (stateCount == 0) {
        qDebug() << "No states to draw.";
        return; // Nothing to draw
    }

    // Calculate grid dimensions
    int itemsPerRow = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(stateCount))));
    if (itemsPerRow == 0) itemsPerRow = 1; // Avoid division by zero if stateCount is 0 (though already checked)
    qDebug() << "Calculated items per row:" << itemsPerRow;

    // Dimensions and spacing between items
    const qreal itemWidth = 60;
    const qreal itemHeight = 60;
    const qreal hSpacing = 100; // Horizontal spacing
    const qreal vSpacing = 100; // Vertical spacing
    const qreal margin = 50;    // Margin from the edge of the scene (increased for better visibility)

    // Map to store created QGraphicsItemGroup for states (keyed by state name)
    std::map<std::string, QGraphicsItemGroup*> stateSceneItems;

    // Draw States (in a grid)
    int currentRow = 0;
    int currentCol = 0;
    for (const auto& pair : statesMap) {
        const std::string& stateName = pair.first;
        State* state = pair.second.get(); // Get non-const State pointer
        if (!state) {
            qWarning() << "Skipping null state pointer for name:" << QString::fromStdString(stateName);
            continue;
        }

        int stateId = state->getStateId(); // Use the ID from the State object itself

        // Calculate position
        qreal x = margin + currentCol * (itemWidth + hSpacing);
        qreal y = margin + currentRow * (itemHeight + vSpacing);

        QGraphicsItemGroup *group = new QGraphicsItemGroup();
        QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, itemWidth, itemHeight, group);

        bool isInitial = (state == machine->getInitialState());
        ellipse->setBrush(QBrush(isInitial ? activeStateColor : normalStateColor));
        ellipse->setPen(QPen(Qt::black));
        ellipse->setData(0, QVariant(QString::fromStdString(stateName))); // Store original name for color updates

        QGraphicsTextItem *text = new QGraphicsTextItem(QString::fromStdString(stateName), group);
        QRectF textRect = text->boundingRect();
        qreal textX = (itemWidth - textRect.width()) / 2;
        qreal textY = (itemHeight - textRect.height()) / 2;
        text->setPos(textX, textY);

        group->setPos(x, y);
        group->setFlag(QGraphicsItem::ItemIsMovable);
        group->setFlag(QGraphicsItem::ItemIsSelectable);
        group->setData(0, QVariant(stateId)); // Store state ID (key 0 for state ID)
        group->setData(1, "state");           // Identifier type "state" (key 1 for type)
        group->setData(2, QVariant(QString::fromStdString(stateName))); // Also store state name (key 2 for name)
        group->setZValue(2);
        scene->addItem(group);
        stateSceneItems[stateName] = group;
        state->currentPos = getVisualCenterOfStateItem(group); // Initialize currentPos for the State object

        // Connect the position change signal for this specific group
        
        QMetaObject::Connection conn = connect(this->scene, &QGraphicsScene::changed, this, [this, group, state]() {
            if (addingTransitionMode) return;
        
            // This check is mostly for robustness during development,
            // proper connection management should make it less necessary.
            if (!group || !group->scene()) {
                // qDebug() << "State move lambda: group no longer valid or not in a scene.";
                return;
            }
        
            if (group->isSelected()) {
                // --- Block signals to prevent re-entrancy ---
                bool oldSignalsBlocked = scene->signalsBlocked();
                scene->blockSignals(true);
        
                QPointF newVisualCenter = MainWindow::getVisualCenterOfStateItem_static(group);
                if (state && state->currentPos != newVisualCenter) {
                    qDebug().nospace() << "LAMBDA DETECTED MOVE for '" << QString::fromStdString(state->getName())
                                       << "': Old currentPos=" << state->currentPos
                                       << ", NewVisualCenter=" << newVisualCenter;
                    state->currentPos = newVisualCenter;
                    state->updateTransitionPositions(this->scene, group, machine);
                }
        
                // --- Restore signal blocking state ---
                scene->blockSignals(oldSignalsBlocked);
            }
        });
        stateMoveConnections.append(conn); // Store the connection

        qDebug() << " Drew state:" << QString::fromStdString(stateName) << "ID:" << stateId << "at (" << x << "," << y << ")";

        currentCol++;
        if (currentCol >= itemsPerRow) {
            currentCol = 0;
            currentRow++;
        }
    }

    // Draw Transitions
    qDebug() << "Drawing transitions...";
    const auto& transitionsList = machine->getTransitions();
    for (const auto& trans_ptr : transitionsList) {
        const Transition* transition_model = trans_ptr.get(); // Using transition_model for clarity
        State* sourceState_model = transition_model->getSourceState();
        State* targetState_model = transition_model->getTargetState();

        if (!sourceState_model || !targetState_model) {
            qWarning() << "REDRAW: Skipping transition, null source/target model state. TransID:" << (transition_model ? transition_model->getTransitionId() : -1);
            continue;
        }

        const std::string& sourceName = sourceState_model->getName();
        const std::string& targetName = targetState_model->getName();
        int transId = transition_model->getTransitionId();

        auto itSource = stateSceneItems.find(sourceName);
        auto itTarget = stateSceneItems.find(targetName);

        if (itSource != stateSceneItems.end() && itTarget != stateSceneItems.end()) {
            QGraphicsItemGroup* startItemGroup_gui = itSource->second;
            QGraphicsItemGroup* endItemGroup_gui = itTarget->second;

           
            QPointF startCenter = getVisualCenterOfStateItem(startItemGroup_gui);
            QPointF endCenter = getVisualCenterOfStateItem(endItemGroup_gui);
            qDebug().nospace() << "REDRAW: For transID " << transId << " (" << QString::fromStdString(sourceName) << " -> " << QString::fromStdString(targetName) << ")";
            qDebug().nospace() << "    Source GUI item at: " << startItemGroup_gui->pos() << ", Calculated Center: " << startCenter;
            qDebug().nospace() << "    Target GUI item at: " << endItemGroup_gui->pos() << ", Calculated Center: " << endCenter;
           

            QString label = QString::fromStdString(transition_model->getCondition());
            QPointF actualStartPos_from_drawArrow, actualEndPos_from_drawArrow; // Renamed for clarity

            QGraphicsItemGroup* transitionGroup_gui = MainWindowUtils::drawArrow(
                startCenter,
                endCenter,
                label,
                transId,
                scene,
                &actualStartPos_from_drawArrow,
                &actualEndPos_from_drawArrow
            );

            
            qDebug().nospace() << "    drawArrow for transID " << transId << " returned: "
                     << "actualStartPos=" << actualStartPos_from_drawArrow
                     << ", actualEndPos=" << actualEndPos_from_drawArrow;
            

            if (transitionGroup_gui) {
                sourceState_model->addOutgoingTransitionGroup(transitionGroup_gui, actualStartPos_from_drawArrow, actualEndPos_from_drawArrow);
                targetState_model->addIncomingTransitionGroup(transitionGroup_gui, actualStartPos_from_drawArrow, actualEndPos_from_drawArrow);
                // Original debug log retained, but we can supplement it with points:
                qDebug().nospace() << "  Drew and registered transition: " << QString::fromStdString(sourceName) << "->" << QString::fromStdString(targetName)
                         << " Label: " << label << " ID: " << transId
                         << " (Points used for registration: " << actualStartPos_from_drawArrow << ", " << actualEndPos_from_drawArrow << ")";
            } else {
                qWarning() << "  REDRAW: Failed to draw arrow for transition ID:" << transId << "from" << QString::fromStdString(sourceName) << "to" << QString::fromStdString(targetName);
            }
        } else {
            qWarning() << " REDRAW: Skipping transition draw/registration: Cannot find scene item for source '" << QString::fromStdString(sourceName) << "' or target '" << QString::fromStdString(targetName) << "'";
        }
    }

    qDebug() << "Finished redrawing automaton.";
    
    GraphicsView* gView = qobject_cast<GraphicsView*>(ui->graphicsView);
    if(gView) {
        // Attempt to fit all items in view, or at least ensure (0,0) is visible.
        if (!scene->items().isEmpty()) {
            gView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
            qDebug() << "Fitted scene contents into view.";
        } else {
            gView->ensureVisible(QRectF(0, 0, 1, 1), 0, 0); // Fallback for empty scene
            qDebug() << "Ensured (0,0) is visible for empty scene.";
        }
    }
}


QPointF MainWindow::getVisualCenterOfStateItem_static(QGraphicsItemGroup* stateItemGroup) {
    if (!stateItemGroup) {
        qWarning() << "MainWindow::getVisualCenterOfStateItem_static: Received null stateItemGroup.";
        return QPointF(); // Return an invalid point or default point
    }

    for (QGraphicsItem* child : stateItemGroup->childItems()) {
        if (QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(child)) {
            
            return stateItemGroup->mapToScene(ellipse->rect().center());
        }
    }

    // Fallback if the ellipse is not found (should not occur with correct group structure)
    qWarning() << "MainWindow::getVisualCenterOfStateItem_static: Ellipse not found in state group for item at scene pos"
               << stateItemGroup->scenePos() << ". Falling back to sceneBoundingRect().center().";
    return stateItemGroup->sceneBoundingRect().center();
}

void MainWindow::setInputFieldsEnabled(bool enabled) {
    qDebug() << "Setting input fields enabled state to:" << enabled;
    // Find the group box
    QGroupBox* groupBox = ui->editInGroupBox;
    if (!groupBox) {
        qWarning() << "setInputFieldsEnabled: editInGroupBox not found.";
        return;
    }

    // Find all QLineEdit inside the group box
    QList<QLineEdit*> inputEdits = groupBox->findChildren<QLineEdit*>();

    for (QLineEdit* edit : inputEdits) {
        edit->setEnabled(enabled);
        
    }
    qDebug() << "Updated enabled state for" << inputEdits.count() << "input fields.";
}


void MainWindow::clearLayout(QLayout* layout) {
    if (!layout) return;
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) { // Item is now ours to delete
        if (QWidget* widget = item->widget()) {
            widget->disconnect(); // Good for safety
            widget->deleteLater();  // Schedule widget for deletion
        } else if (QLayout* subLayout = item->layout()) {
            clearLayout(subLayout); // Recursively clear sub-layout
           
        }
        
        delete item;
    }
    qDebug() << "Layout cleared (all items taken and deleted).";
}



void MainWindow::resetGroupBoxLayout(QGroupBox* groupBox) {
    if (!groupBox) {
        qWarning() << "resetGroupBoxLayout: groupBox is null.";
        return;
    }
    if (QLayout* layout = groupBox->layout()) {
        clearLayout(layout); // Clear items from the layout (widgets, sublayouts, spacers)
        delete layout;       // Delete the layout object itself. groupBox->layout() will become nullptr.
        qDebug() << "Layout for GroupBox" << groupBox->objectName() << "has been deleted.";
    } else {
        qDebug() << "GroupBox" << groupBox->objectName() << "had no layout to reset.";
    }
}

void MainWindow::clearVariableList() {
    qDebug() << "Clearing Variable List UI (resetting layout for editVarGroupBox)";
    resetGroupBoxLayout(ui->editVarGroupBox);
}

void MainWindow::clearInputList() {
    qDebug() << "Clearing Input List UI (resetting layout for editInGroupBox)";
    resetGroupBoxLayout(ui->editInGroupBox);
}

void MainWindow::clearOutputList() {
    qDebug() << "Clearing Output List UI (resetting layout for trackingOutputsGroupBox)";
    resetGroupBoxLayout(ui->trackingOutputsGroupBox);
}


// Corrected populateUIFromModel (remove internal clear calls)
void MainWindow::populateUIFromModel() {
    if (!machine) {
         qWarning() << "Cannot populate UI: machine model is null.";
         return;
    }
    qDebug() << "Populating UI elements from loaded model for machine:" << QString::fromStdString(machine->getName());

    // Clearing is now handled in on_loadJsonButton_clicked BEFORE this function.

    // --- Populate Variables ---
    qDebug() << "Populating Variables...";
    const auto& variablesMap = machine->getVariables();
    for (const auto& pair : variablesMap) {
        const Variable* var = pair.second.get();
        addVarRowToGUI(var->getName(), var->getTypeHint(), var->getValueAsString());
    }

    // --- Populate Inputs ---
    qDebug() << "Populating Inputs...";
    const auto& inputsMap = machine->getInputs();
    for (const auto& pair : inputsMap) {
         const Input* input = pair.second.get();
         addInputRowToGUI(input->getName());
         QLineEdit* le = ui->editInGroupBox->findChild<QLineEdit*>(QString::fromStdString(input->getName()));
         if(le) {
             le->setText(QString::fromStdString(input->getLastValue().value_or("")));
         }
    }

    // --- Populate Outputs ---
    qDebug() << "Populating Outputs...";
    const auto& outputsMap = machine->getOutputs();
    QGroupBox *groupBox = ui->trackingOutputsGroupBox;
    if(groupBox) {
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(groupBox->layout());
        if (!layout) { // This will be true if resetGroupBoxLayout was called
            qDebug() << "Creating new QVBoxLayout for trackingOutputsGroupBox.";
            layout = new QVBoxLayout();
            layout->setSpacing(2);
            layout->setContentsMargins(4, 4, 4, 4);
            groupBox->setLayout(layout);
            layout->addStretch(); // Add stretch ONCE to keep items at the top
        }

        for (const auto& pair : outputsMap) {
            const Output* output = pair.second.get();
            const std::string& outputName = output->getName();
            qDebug() << " Adding output to UI:" << QString::fromStdString(outputName);

            QString labelText = QString::fromStdString(outputName) + " = ?";
            QLabel *outputLabel = new QLabel(labelText, groupBox);
            outputLabel->setObjectName(QString::fromStdString(outputName));

            QHBoxLayout *rowLayout = new QHBoxLayout();
            rowLayout->addWidget(outputLabel);

            QPushButton *deleteButton = new QPushButton("x", groupBox);
            deleteButton->setFixedSize(20, 20);
            deleteButton->setToolTip("Delete this output");
            rowLayout->addWidget(deleteButton);

            connect(deleteButton, &QPushButton::clicked, this, [this, groupBox, rowLayout, outputName]() { // Capture groupBox
                qDebug() << "Delete button clicked for output:" << QString::fromStdString(outputName);
                if (machine) machine->removeOutput(outputName);

                // Get layout from groupBox, not a captured 'layout' variable which might be stale
                QVBoxLayout* currentMainLayout = qobject_cast<QVBoxLayout*>(groupBox->layout());
                if (currentMainLayout) {
                    currentMainLayout->removeItem(rowLayout);
                }
                QLayoutItem *childItem;
                while((childItem = rowLayout->takeAt(0)) != nullptr) {
                    if(childItem->widget()) childItem->widget()->deleteLater();
                    delete childItem;
                }
                delete rowLayout;
                qDebug() << "Output removed from UI and machine:" << QString::fromStdString(outputName);
                
            });
            layout->insertLayout(0, rowLayout); // Insert before the stretch
        }
    } else {
        qWarning() << "Cannot populate outputs: ui->trackingOutputsGroupBox is null.";
    }
    qDebug() << "Finished populateUIFromModel.";
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




void MainWindow::on_renameButton_clicked() {
    qDebug() << "Rename Automaton button clicked.";

    // Check if there is a model to rename
    if (!machine) {
        QMessageBox::warning(this, "No Automaton", "No automaton is currently loaded to rename.");
        qWarning() << "Rename attempted but no machine model exists.";
        return;
    }

    std::string newName = MainWindowUtils::ProccessOneArgumentDialog("Enter new automaton name:");
    if (newName.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }
    
    try {
        
         machine->setName(newName);
    } catch (const std::exception& e) {
         qCritical() << "Error renaming automaton in the model:" << e.what();
         QMessageBox::critical(this, "Rename Error", QString("Failed to set the new name in the model: %1").arg(e.what()));
         return; // Error while changing the name in the model
    }
}



void MainWindow::addVarRowToGUI(const std::string& name, const std::string& type, const std::string& value) {
    // Ensure the target GroupBox exists in the UI
    if (!ui->editVarGroupBox) {
        qWarning() << "Cannot add variable row: ui->editVarGroupBox is null. Check objectName in Designer.";
        
    }

    // Get the layout of the GroupBox, assuming/creating QVBoxLayout
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->editVarGroupBox->layout());
    if (!mainLayout) {
        qDebug() << "No existing layout found in editVarGroupBox, creating new QVBoxLayout.";
        mainLayout = new QVBoxLayout();
        ui->editVarGroupBox->setLayout(mainLayout);
        
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

    connect(deleteButton, &QPushButton::clicked, this, [this, rowLayout, name]() {
        QVBoxLayout* currentMainLayout = qobject_cast<QVBoxLayout*>(ui->editVarGroupBox->layout()); // Get current layout
        if (currentMainLayout) {
            currentMainLayout->removeItem(rowLayout);
        }
         while (QLayoutItem* child = rowLayout->takeAt(0)) {
            if(child->widget()) child->widget()->deleteLater();
            delete child;
        }
        delete rowLayout;

        if(machine) machine->removeVariable(name);
        qDebug() << "Variable removed from UI and machine:" << QString::fromStdString(name);
    });

    // Add the delete button to the row layout
    rowLayout->addWidget(deleteButton);

    
    // Set object name for the layout itself to find the whole row later
    rowLayout->setObjectName(baseObjectName + "_layout");


    // --- Add widgets to the row layout ---
    rowLayout->addWidget(typeLabel);    // Add type label
    rowLayout->addWidget(nameLabel);    // Add name label
    rowLayout->addWidget(valueEdit, 1); // Add value edit, give it stretch factor 1
    

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
    if (!ui->editInGroupBox) {
        qWarning() << "Cannot add variable row: ui->editVarGroupBox is null. Check objectName in Designer.";
        return;
    }

    // Get the layout of the GroupBox, assuming/creating QVBoxLayout
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->editInGroupBox->layout());
    if (!mainLayout) {
        qDebug() << "No existing layout found in editVarGroupBox, creating new QVBoxLayout.";
        mainLayout = new QVBoxLayout();
        ui->editInGroupBox->setLayout(mainLayout);
        
    }

    // --- Create Widgets for the new row ---
    // Create a horizontal layout for the row elements
    QHBoxLayout* rowLayout = new QHBoxLayout();

    // Create labels for type and name (read-only)
   
    QLabel* nameLabel = new QLabel(QString::fromStdString(name));
    nameLabel->setToolTip(QString::fromStdString(name)); // Show full name on hover if truncated

    // Create an editable line edit for the value
    QLineEdit* valueEdit = new QLineEdit(QString::fromStdString(""));
    connect(valueEdit, &QLineEdit::returnPressed, this, &MainWindow::handleInputReturnPressed); 
    
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

    valueEdit->setEnabled(false);

    // Connect the delete button's clicked signal to a lambda function
    connect(deleteButton, &QPushButton::clicked, this, [this, rowLayout, name]() {
        // Remove the row from the layout
        QVBoxLayout* currentMainLayout = qobject_cast<QVBoxLayout*>(ui->editInGroupBox->layout()); // Get current layout
        if (currentMainLayout) {
            
            currentMainLayout->removeItem(rowLayout);
        }
        // Delete widgets within the rowLayout
        while (QLayoutItem* child = rowLayout->takeAt(0)) {
            if(child->widget()) child->widget()->deleteLater();
            delete child;
        }
        delete rowLayout; // Then delete the row layout itself
        
        if(machine) machine->removeInput(name); // Remove the input from the machine
        qDebug() << "Input removed from UI and machine:" << QString::fromStdString(name);
    });

    // Add the delete button to the row layout
    rowLayout->addWidget(deleteButton);
    
    // Set object name for the layout itself to find the whole row later
    rowLayout->setObjectName(baseObjectName + "_layout");


    // --- Add widgets to the row layout ---
    // Add type label
    rowLayout->addWidget(nameLabel);    // Add name label
    rowLayout->addWidget(valueEdit, 1); // Add value edit, give it stretch factor 1
    

    // --- Add the row layout to the main vertical layout ---
    // Insert before the last item if it's a stretch, otherwise at the end
    int insertPos = mainLayout->count();
    if (insertPos > 0 && mainLayout->itemAt(insertPos - 1)->spacerItem()) {
        insertPos--; // Insert before the stretch
    }
    mainLayout->insertLayout(insertPos, rowLayout);

    

    qDebug() << "Added variable row to UI for:" << QString::fromStdString(name);

    
}

void MainWindow::handleInputReturnPressed() {
    // Get the sender object (the QLineEdit that emitted the signal)
    QLineEdit* senderEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderEdit) {
        qWarning() << "Sender is not a QLineEdit.";
        return;
    }

    // Get the object name to identify the variables
    QString varName = senderEdit->objectName();

    Input* input = machine->getInput(varName.toStdString());
    if (!input) {
        qWarning() << "Input not found in machine:" << varName;
        return;
    }
    input->updateValue(senderEdit->text().toStdString());
    qDebug() << "Input value edited:" << varName << "New value:" << QString::fromStdString(input->getLastValue().value_or("No value"));
   //ODOSLANIE
   if (!guiSocket_) {
    qWarning() << "Cannot send INPUT message: guiSocket_ is null.";
    return;
    }

    QString newValue = senderEdit->text();
    QString message = QString("INPUT|%1|%2").arg(varName).arg(newValue);
    QHostAddress runtimeAddr("127.0.0.1");
    quint16 port = static_cast<quint16>(std::stoi(portAutomat));
    qint64 bytesSent = guiSocket_->writeDatagram(message.toUtf8(), runtimeAddr, port);

    if (bytesSent == -1) {
        qWarning() << "Failed to send INPUT datagram:" << guiSocket_->errorString();
        QMessageBox::warning(this, "Network Error", "Failed to send input message to automaton:\n" + guiSocket_->errorString());
    } else {
        qDebug() << "Sent good";
    }
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



    // Get a pointer to QGroupBox (replace 'trackingOutputsGroupBox' with the actual name!)
    QGroupBox *groupBox = ui->trackingOutputsGroupBox;
    if (!groupBox) {
        qWarning() << "ERROR: Could not find the QGroupBox named 'trackingOutputsGroupBox' in the UI.";
        return;
    }

    // Get the layout from the groupbox. If it doesn't exist, create it and set its parameters.
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(groupBox->layout());
    if (!layout) {
        qDebug() << "GroupBox has no layout, creating and configuring a new QVBoxLayout.";
        layout = new QVBoxLayout();

        
        layout->setSpacing(2); // Reduce vertical spacing BETWEEN widgets (e.g., to 2 pixels)
        layout->setContentsMargins(4, 4, 4, 4); // Reduce margins AROUND the layout in the groupbox (L,T,R,B)

        groupBox->setLayout(layout);

        
        
        layout->addStretch();
    }
    QString oldName = QString::fromStdString(outputName);

    outputName = outputName + " = value";

    // Create a new QLabel to display the input name
    QLabel *newLabel = new QLabel(QString::fromStdString(outputName), groupBox); // Use qInputName
    newLabel->setObjectName(oldName);  

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
        while(QLayoutItem *child = rowLayout->takeAt(0)) {
            if (child->widget()) child->widget()->deleteLater();
            else if (child->layout()) child->layout()->deleteLater();
        }
        rowLayout->deleteLater(); // Or let Qt handle it when the parent is destroyed.

        // Remove the output from the machine
        machine->removeOutput(outputName);
        qDebug() << "Output removed from UI and machine:" << QString::fromStdString(outputName);
    });

    // Add the row layout to the main layout
    layout->insertLayout(0, rowLayout);


    
    layout->insertWidget(0, newLabel);

    
}



void MainWindow::on_addStateButton_clicked() {
    
    QDialog dialog(this);
    dialog.setWindowTitle("Enter Transition Details");
    dialog.setMinimumSize(400, 400);
    QFont font = dialog.font();
    font.setPointSize(16);  // Increases font size, including for buttons
    dialog.setFont(font);

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
    

    // --- Create Visual Representation (Ellipse + Name) ---
    qreal width = 60; qreal height = 60;
    int itemCount = scene->items().count();
    qreal x = (itemCount % 5) * (width + 20);
    qreal y = (itemCount / 5) * (height + 20);

    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, width, height);
    ellipse->setBrush(QBrush(Qt::cyan));
    ellipse->setPen(QPen(Qt::black));
    ellipse->setData(0, QVariant(stateName));


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
    group->setZValue(2); // Set Z value to 1 for proper stacking order



    // Connect the position change signal to a custom slot
    connect(scene, &QGraphicsScene::changed, this, [this, group]() {

        if(addingTransitionMode) {
            return;
        }

        if (group->isSelected()) {
            

            State *state = machine->getState(group->data(0).toInt());
            if(state->currentPos != group->sceneBoundingRect().center()) {
                state->currentPos = group->sceneBoundingRect().center();
                state->updateTransitionPositions(scene, group, machine);

            }
            
        }
    });
   
    

    scene->addItem(group);

    std::unique_ptr<State> newState = std::make_unique<State>(stateName.toStdString(), stateAction.toStdString(), objectStateId);
    newState->currentPos = group->sceneBoundingRect().center();
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

    GraphicsView* gView = ui->graphicsView;
    if (gView) {
        
         gView->ensureVisible(group->sceneBoundingRect(), 50, 50); // 50px margin around the element
         qDebug() << "Ensured newly added state is visible.";

        
    }

    qDebug() << "Group (Ellipse + Name) added to scene for state:" << stateName;
}




void MainWindow::on_addTransitionButton_clicked() {
    addingTransitionMode = true;
    startItemForTransition = nullptr; // Reset start item
    qDebug() << "Entered Add Transition mode. Click the start state.";
    
}

void MainWindow::handleRightClick(QGraphicsItemGroup *item, QGraphicsLineItem *lineItem) {
    // Handle right-click events on the state items
    if (item) {
        // edit state
        if(!addingTransitionMode && item->data(1).toString() == "state") {
            editState(item);
            return;   
        }

        if(!addingTransitionMode && item->data(0).toString() == "Transition") {
            editTransition(item);
            return;
        }   
    }
}




void MainWindow::handleStateClick(QGraphicsItemGroup *item, QGraphicsLineItem *lineItem) {
    


    if(!addingTransitionMode) {
        //qDebug() << "handleStateClick called. Not in transition mode.";
        return;
    }
    

    //qDebug() << "handleStateClick called. Adding transition mode:";
    //qDebug() << "State item clicked:" << lineItem->data(0).toString();

    

    //qDebug() << "State item clicked:" << lineItem->data(0).toString();

    // Check if a valid state item was clicked (item is not nullptr)
    

    
    if (item) {
        
        

        
        

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

            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            layout.addWidget(&buttonBox);

            connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

            QString condition;
            int delay = 0;

            if (dialog.exec() == QDialog::Accepted) {
                condition = lineEdit1.text().trimmed();
                

                if (condition.isEmpty()) {
                    qDebug() << "Dialog cancelled or empty input.";
                    return;
                }

                qDebug() << "Transition condition:" << condition;
                qDebug() << "Transition action:" << delay;
                
            } else {
                qDebug() << "Dialog cancelled.";
                return;
            }
           

            QPointF actualStartPos;
            QPointF actualEndPos;

            QGraphicsItemGroup *transitionGroup = MainWindowUtils::drawArrow(
                MainWindow::getVisualCenterOfStateItem_static(startItemForTransition),
                MainWindow::getVisualCenterOfStateItem_static(endItemForTransition),
                QString::fromStdString(condition.toStdString()), objectTransitionId, scene, 
                &actualStartPos, &actualEndPos
            );
            

            
            this->startStateForTransition->addOutgoingTransitionGroup(transitionGroup, actualStartPos, actualEndPos);
            this->endStateForTransition->addIncomingTransitionGroup(transitionGroup, actualStartPos, actualEndPos);
            
            

            qDebug() << "---------------------------------------------------";
            qDebug() << "Transition ID:" << objectTransitionId;

            qDebug() << "Start state name:" << QString::fromStdString(startStateForTransition->getName());
            qDebug() << "End state name:" << QString::fromStdString(endStateForTransition->getName());


            //startStateForTransition->printTransitions();
            //endStateForTransition->printTransitions();

            
            qDebug() << "---------------------------------------------------";
            
            
            
            


            try {
                if(this->startStateForTransition && this->endStateForTransition) {
                    std::unique_ptr<Transition> newTransition = std::make_unique<Transition>(
                        this->startStateForTransition,
                        this->endStateForTransition,
                        objectTransitionId,
                        condition.toStdString(),
                        delay
                        ); 
    
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
    
    else {
        qDebug() << "Clicked item is not a valid state or line item.";
    }
}

void MainWindow::editTransition(QGraphicsItemGroup *item) {

    int transitionId = item->data(1).toInt(); // Retrieve the state ID stored in the item
    Transition* transition = machine->getTransition(transitionId); // Get the state object using the ID
    if (!transition) {
        qDebug() << "State not found for ID:" << transitionId;
        return;
    }
    qDebug() << "Attempting to edit transition: ID=" << transitionId << " Old condition=" << QString::fromStdString(transition->getCondition());
    std::string currentCondition = transition->getCondition();
   
    std::string newCondition = MainWindowUtils::ProccessEditDialogForTransition("Edit transition condition:", currentCondition);
    if (newCondition.empty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return;
    }

    transition->setCondition(newCondition);
    
    // --- Update graphical element ---
    // Find QGraphicsTextItem in the group
    QGraphicsTextItem *textItem = nullptr;
    for (QGraphicsItem *child : item->childItems()) {
        textItem = dynamic_cast<QGraphicsTextItem*>(child);
        if (textItem) {
            break; // Found it
        }
    }

    if (textItem) {
        // Change displayed text
        QString newConditionQt = QString::fromStdString(newCondition); // Use new condition from dialog
        textItem->setPlainText(newConditionQt); // Change text in GUI
    }

    
    
    qWarning() << "Could not find text item within the group to update.";
    
}


void MainWindow::editState(QGraphicsItemGroup *itemGroup) {

    // --- Get ID and State object from Machine ---
    // Assuming convention: 0=ID, 1=Type, 2=Name
    bool idOk;
    int stateId = itemGroup->data(0).toInt(&idOk);
    QString itemType = itemGroup->data(1).toString();

    if (!idOk || itemType != "state") { /* ... warning and return ... */ }
    if (!machine) { /* ... warning and return ... */ }
    State* state = machine->getState(stateId);
    if (!state) { /* ... warning and return ... */ }
    qDebug() << "Attempting to edit state: ID=" << stateId << "Name=" << QString::fromStdString(state->getName());

    // --- Get new data from the user ---
    std::string newName;
    std::string newAction;
    std::string currentName = state->getName(); // Get current name from the model
    std::string currentAction = state->getAction();
    ProccessMultipleArgsInputEditDialog("Edit State", "Enter new state name:", "Enter new action:",
        currentName, currentAction, &newName, &newAction);
    if (newName.empty()) { /* ... warning and return ... */ }
    qDebug() << "New name entered:" << QString::fromStdString(newName);
    qDebug() << "New action entered:" << QString::fromStdString(newAction);

    // --- Update the model (State object) ---
    // ADD DUPLICATE NAME CHECK HERE, IF THE NAME HAS CHANGED
    if (newName != currentName && machine->getStates().count(newName)) {
         QMessageBox::critical(this, "Error", "A state with the name '" + QString::fromStdString(newName) + "' already exists!");
         return;
    }
    state->setName(newName); // Update name in the model
    state->setAction(newAction); // Update action in the model
    qDebug() << "State object in model updated.";

    // --- Update graphical representation (itemGroup) ---
    QGraphicsTextItem* textItem = nullptr;
    QGraphicsEllipseItem* ellipseItem = nullptr;
    for (QGraphicsItem *child : itemGroup->childItems()) {
         if (!textItem) textItem = dynamic_cast<QGraphicsTextItem*>(child);
         if (!ellipseItem) ellipseItem = dynamic_cast<QGraphicsEllipseItem*>(child);
         if (textItem && ellipseItem) break;
    }

    if (textItem && ellipseItem) {
        QString newNameQt = QString::fromStdString(newName);
        textItem->setPlainText(newNameQt); // Change text in GUI

        // --- Update stored data ---
        itemGroup->setData(2, QVariant(newNameQt));      // Key 2 = NAME
        ellipseItem->setData(0, QVariant(newNameQt));    // Key 0 in ellipse = NAME (for highlight?)

        // --- Recalculate text position ---
        QRectF ellipseRect = ellipseItem->rect();
        QRectF textRect = textItem->boundingRect();
        qreal textX = (ellipseRect.width() - textRect.width()) / 2;
        qreal textY = (ellipseRect.height() - textRect.height()) / 2;
        textItem->setPos(textX, textY);

        qDebug() << "State graphics (text, position, data) updated for:" << newNameQt;
    } else {
        qWarning() << "Could not find text/ellipse item within the state group ID:" << stateId;
    }
}


void MainWindow::ProccessMultipleArgsInputEditDialog(const std::string& title, const std::string& label1Text, const std::string& label2Text, 
    std::string& predefinedInput1, std::string& predefinedInput2, 
    std::string* newInput1, std::string* newInput2) {

        QDialog dialog(this);
        dialog.setWindowTitle(QString::fromStdString(title));

        QFont font = dialog.font();
        font.setPointSize(20);  // Veľký font
        dialog.setFont(font);

        dialog.setMinimumSize(400, 400);  // Väčšie okno

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

    
    qDebug() << "ProccessMultipleArgsInputEditDialog completed. Final values:";
    qDebug() << "newInput1 =" << QString::fromStdString(*newInput1);
    qDebug() << "newInput2 =" << QString::fromStdString(*newInput2);


}




void MainWindow::highlightItem(QGraphicsItemGroup* item) {
    if (!item) return;
    
    item->setOpacity(0.2); // Make it slightly transparent
     
}


void MainWindow::unhighlightItem(QGraphicsItemGroup* item) {
     if (!item) return;
     item->setOpacity(1.0); // Restore full opacity
     
}










