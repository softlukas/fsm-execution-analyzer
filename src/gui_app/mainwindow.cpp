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
#include <QUdpSocket> // Potrebný include
#include <QNetworkDatagram> // Alternatívny spôsob čítania od Qt 5.10+
#include <QFileDialog> // Potrebný pre dialóg na výber súboru
#include <QStandardPaths> // Pre získanie predvoleného adresára (napr. Dokumenty)
#include <QLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Automat Creator");
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 2000, 1500); // <<< Nastav veľkú scénu (napr. 2000x1500 pixelov)

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
    if (gView) {
        gView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); // Predvolené
        gView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);   // Predvolené

        
    }
    // Create a Machine object using the Machine constructor
    std::string machineName = "";
    while (machineName.empty()) { // Opakuj, kým meno nie je zadané
        machineName = MainWindowUtils::ProccessOneArgumentDialog("Set automat name");
        if (machineName.empty()) {
            QMessageBox::warning(this, "Name Required", "Automaton name cannot be empty. Please provide a name.");
        }
    }
    
    setInputFieldsEnabled(false); // Východzí stav je neaktívny


    machine = new Machine(machineName);

    
    qDebug() << "Machine object created successfully.";


    // --- ZÍSKANIE PORTOV S VALIDÁCIOU ČÍSIEL ---
    bool portsValid = false;
    std::string guiPortStr = "9000"; // Predvolená hodnota
    std::string automatonPortStr = "9001"; // Predvolená hodnota

    while (!portsValid) {
        std::string tempGuiPort = "";
        std::string tempAutomatonPort = "";

        // Zobraz dialóg (predvyplň posledné platné alebo predvolené hodnoty)
        ProccessMultipleArgsInputEditDialog("ADD PORTS",
                                            "Add port GUI (1024-65535):",
                                            "Add port AUTOMAT (1024-65535):",
                                            guiPortStr,         // Predvyplň poslednú/predvolenú hodnotu
                                            automatonPortStr,   // Predvyplň poslednú/predvolenú hodnotu
                                            &tempGuiPort,       // Výstup pre nový GUI port
                                            &tempAutomatonPort);// Výstup pre nový Automat port

        // Skontroluj, či užívateľ nezrušil dialóg (obe hodnoty by boli prázdne podľa ProccessMultipleArgs...)
         if (tempGuiPort.empty() && tempAutomatonPort.empty()) {
             // Ak chceme ukončiť aplikáciu pri zrušení portov, môžeme to urobiť tu
             // napr. qApp->quit(); alebo vyhodiť výnimku, alebo nastaviť chybový stav
              QMessageBox::critical(this, "Ports Required", "Valid port numbers are required to continue.");
             // V tomto príklade budeme pokračovať v cykle a žiadať znova
             continue; // Späť na začiatok cyklu while
         }


        // Validácia GUI Portu
        bool guiPortOk = false;
        int guiPortInt = -1;
        try {
            size_t processedChars = 0;
            guiPortInt = std::stoi(tempGuiPort, &processedChars);
            // Skontroluj, či bolo spracované celé číslo a či je v rozsahu
            if (processedChars == tempGuiPort.length() && guiPortInt >= 1024 && guiPortInt <= 65535) {
                guiPortOk = true;
            }
        } catch (const std::invalid_argument& e) {
            // std::stoi vyhodí, ak to nie je číslo
        } catch (const std::out_of_range& e) {
            // std::stoi vyhodí, ak je číslo mimo rozsahu int
        }

        // Validácia Automaton Portu
        bool automatonPortOk = false;
        int automatonPortInt = -1;
         try {
            size_t processedChars = 0;
            automatonPortInt = std::stoi(tempAutomatonPort, &processedChars);
            if (processedChars == tempAutomatonPort.length() && automatonPortInt >= 1024 && automatonPortInt <= 65535) {
                 // Dodatočná kontrola: Porty nesmú byť rovnaké
                 if (automatonPortInt != guiPortInt) {
                     automatonPortOk = true;
                 }
            }
        } catch (const std::invalid_argument& e) {
        } catch (const std::out_of_range& e) {
        }

        // Skontroluj výsledky validácie
        if (guiPortOk && automatonPortOk) {
            portsValid = true; // Všetko je v poriadku, ukonči cyklus
            // Ulož platné hodnoty do členských premenných
            this->portGUI = tempGuiPort;
            this->portAutomat = tempAutomatonPort;
            qInfo() << "Valid ports entered: GUI=" << QString::fromStdString(this->portGUI)
                    << ", Automaton=" << QString::fromStdString(this->portAutomat);
        } else {
            // Zobraz chybovú správu
            QString errorMsg = "Invalid input:\n";
            if (!guiPortOk) {
                errorMsg += "- GUI Port must be a number between 1024 and 65535.\n";
            }
            if (!automatonPortOk) {
                 if (automatonPortInt == guiPortInt && guiPortOk) { // Špeciálny prípad rovnakých portov
                      errorMsg += "- Automaton Port must be different from GUI Port.\n";
                 } else {
                      errorMsg += "- Automaton Port must be a number between 1024 and 65535.\n";
                 }
            }
            QMessageBox::warning(this, "Invalid Ports", errorMsg);
            // Ponechaj posledné platné/predvolené hodnoty v guiPortStr a automatonPortStr pre ďalšiu iteráciu
            guiPortStr = tempGuiPort; // Ukáž užívateľovi, čo zadal nesprávne
            automatonPortStr = tempAutomatonPort;
        }
    } // Koniec while (!portsValid)

    guiSocket_ = new QUdpSocket(this);
    connect(guiSocket_, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
    bindGuiSocket(); // Uisti sa, že socket počúva
    
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

    std::string automatonInitialStateName; // Premenovaná pre jasnosť
    bool initialStateSet = false;

    while (!initialStateSet) { // Opakuj, kým sa nepodarí nastaviť
        automatonInitialStateName = MainWindowUtils::ProccessOneArgumentDialog("Enter automaton starting state name:");

        if (automatonInitialStateName.empty()) {
             // Používateľ stlačil Cancel v dialógu mena
             QMessageBox::warning(this, "Cancelled", "Starting state selection cancelled.");
             return; // Ukonči funkciu, ak používateľ zruší výber stavu
        }

        // Pokús sa nastaviť počiatočný stav v modeli
        try {
            machine->setInitialState(automatonInitialStateName);
            qInfo() << "Initial state set to:" << QString::fromStdString(automatonInitialStateName);
            initialStateSet = true; // Podarilo sa, ukonči cyklus

        } catch (const std::runtime_error& e) {
            // Zachyť konkrétnu výnimku z Machine::setInitialState
            qWarning() << "Error setting initial state:" << e.what();
            QMessageBox::warning(this, "Invalid State", QString("Could not set initial state:\n%1\nPlease enter a valid state name.").arg(e.what()));
            // Cyklus while pobeží znova a opýta sa používateľa
        } catch (const std::exception& e) {
             // Zachyť iné neočakávané chyby
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
        setInputFieldsEnabled(true);
        // Ulož PID do členskej premennej (ak ju máš)
        //currentAutomatonPid_ = pid; // Predpokladá, že máš premennú `qint64 currentAutomatonPid_ = 0;` v mainwindow.h

        // TODO: Aktualizuj stav GUI (napr. zobraz PID, zmeň text tlačidla)
        // ui->statusbar->showMessage("Automaton '" + automatonName + "' running (PID: " + QString::number(pid) + ")", 0);
        // ui->runAutomatButton->setText("Running (Stop?)"); // Alebo samostatné Stop tlačidlo
        // ui->pidLabel->setText(QString::number(pid)); // Ak máš label pre PID

    } else {
        qCritical() << "Failed to start detached automaton process.";
        QMessageBox::critical(this, "Process Error", "Could not start the automaton process.");
        setInputFieldsEnabled(false);
        // ui->statusbar->showMessage("Failed to start automaton!", 0);
    }

    qDebug() << "Starting automaton on PID:" << pid;

    
}

void MainWindow::processPendingDatagrams()
{
    while (guiSocket_->hasPendingDatagrams()) {
        // Modernejší spôsob čítania (odporúčaný)
        QNetworkDatagram datagram = guiSocket_->receiveDatagram();
        if (!datagram.isValid()) {
            continue; // Preskoč neplatný datagram
        }
        QString message = QString::fromUtf8(datagram.data());
        QHostAddress senderAddress = datagram.senderAddress();
        quint16 senderPort = datagram.senderPort();

        // Logovanie prijatej správy
        qDebug() << "Received from" << senderAddress.toString() << ":" << senderPort << "->" << message;

        // --- Parsovanie a Spracovanie Správy ---

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

                // --- Load the Machine from JSON ---
                // !!! ENSURE JsonPersistence::loadFromFile IS IMPLEMENTED !!!
                std::unique_ptr<Machine> loadedMachine = JsonPersistence::loadFromFile(jsonPath.toStdString());

                if (!loadedMachine) {
                    qCritical() << "Failed to load machine from JSON:" << jsonPath;
                    QMessageBox::critical(this, "Error", "Failed to parse or load the automaton definition file:\n" + jsonPath);
                    connectedAutomatonName = ""; // Reset connection state
                    return;
                }

                // --- Success: Update GUI ---
                machine = loadedMachine.release(); // Take ownership
                qInfo() << "Successfully loaded machine '" << connectedAutomatonName << "' from JSON.";
                // ui->statusbar->showMessage("Connected to: " + connectedAutomatonName, 0);

                // Clear scene (should be clear already, but just in case)
                
                scene->clear();
                objectStateId = 0; // Reset counters if needed
                objectTransitionId = 0;

                // Redraw based on the loaded model
                redrawAutomatonFromModel();

                // Populate UI lists (variables, inputs, outputs)
                populateUIFromModel();   
                
                setInputFieldsEnabled(true);

            } else {
                // Received NAME unexpectedly (already connected?) - maybe just log
                 qWarning() << "Received NAME message while not expecting it (already connected?). Name:" << receivedName;
            }
        }else if (message.startsWith("READY ")) {
            QString automatonName = message.mid(6);
            qInfo() << "Automaton" << automatonName << "is READY.";
            // TODO: Aktualizuj status v GUI, povoľ tlačidlá na posielanie INPUT/CMD
            //ui->statusbar->showMessage("Automaton '" + automatonName + "' ready.", 5000);
            // ui->sendInputButton->setEnabled(true); // Príklad
            // ui->getStatusButton->setEnabled(true);

        } else if (message.startsWith("STATE ")) {
            QString activeStateName = message.mid(6).trimmed();
            qInfo() << "Automaton entered state:" << activeStateName;
            // TODO: Zobraz aktuálny stav v GUI
            //ui->currentStateLabel->setText(stateName); // Predpokladá QLabel
            // 1. Resetuj farbu VŠETKÝCH stavov na normálnu
            GraphicsView* gView = ui->graphicsView;
            QGraphicsScene* scene = gView->scene();

            // Prejdi všetky itemy v scéne
            for (QGraphicsItem* item : scene->items()) {
                QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(item);
                if (ellipse) {
                    QString ellipseName = ellipse->data(0).toString();  // Načítaj meno stavu
                    if (ellipseName == activeStateName) {
                        ellipse->setBrush(QBrush(activeStateColor));
                    } else {
                        ellipse->setBrush(QBrush(normalStateColor));
                    }
                }
            }

        } else if (message.startsWith("OUTPUT ")) {
            // Parsovanie: OUTPUT output_name="value"
            QString dataPart = message.mid(7); // Odstráň "OUTPUT "
            int assignmentPos = dataPart.indexOf('=');
            if (assignmentPos != -1) {
                QString outputName = dataPart.left(assignmentPos).trimmed();
                QString outputValue = dataPart.mid(assignmentPos + 1).trimmed();
                // Odstráň prípadné úvodzovky okolo hodnoty
                if (outputValue.length() >= 2 && outputValue.startsWith('"') && outputValue.endsWith('"')) {
                    outputValue = outputValue.mid(1, outputValue.length() - 2);
                }
                qInfo() << "Received OUTPUT:" << outputName << "=" << outputValue;
                
                updateOutputDisplay(outputName.toStdString(), outputValue.toStdString());



            } else {
                qWarning() << "Malformed OUTPUT message:" << message;
            }

        } else if (message.startsWith("VAR ")) {
            // Parsovanie: VAR var_name="value" (rovnaké ako OUTPUT)
             QString dataPart = message.mid(4); // Odstráň "VAR "
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

                // TERAZ ULOŽ DO GUI:
                updateVariableDisplay(varName.toStdString(), varValue.toStdString());

            } else {
                qWarning() << "Malformed VAR message:" << message;
            }

        } else if (message.startsWith("LOG ")) {
            QString logMsg = message.mid(4); // Získaj text za "LOG "
            qInfo() << "[Automaton LOG]" << logMsg;

            // Špeciálne spracovanie pre DEFINITION_PATH (ak používaš toto riešenie)
            if (logMsg.startsWith("DEFINITION_PATH:")) {
                QString jsonPath = logMsg.mid(16); // Odstráň "DEFINITION_PATH:"
                qInfo() << "Received automaton definition path:" << jsonPath;
                // TODO: Načítaj JSON z tejto cesty a prekresli graf/model v GUI
                // loadMachineFromJsonPath(jsonPath);
            } else {
                 // TODO: Zobraz bežnú log správu v GUI (napr. v QTextEdit)
                 // ui->logTextEdit->append("[AUTO] " + logMsg);
            }

        } else if (message.startsWith("ERROR ")) {
            QString errorMsg = message.mid(6); // Získaj text za "ERROR "
            qWarning() << "[Automaton ERROR]" << errorMsg;
            // TODO: Zobraz chybu v GUI (napr. v logu červenou, v status bare)
            // ui->logTextEdit->append("<font color='red'>[AUTO ERR] " + errorMsg + "</font>");
            // ui->statusbar->showMessage("Automaton Error: " + errorMsg, 8000); // Zobraz na 8s
            QMessageBox::warning(this, "Automaton Error", errorMsg);


        } else if (message == "TERMINATING") {
            qInfo() << "Automaton is TERMINATING.";
            // TODO: Aktualizuj status v GUI, zablokuj tlačidlá
            QMessageBox::information(this, "Automaton", "Automaton terminating...");
            setInputFieldsEnabled(false);
            // ui->sendInputButton->setEnabled(false);
            // ui->getStatusButton->setEnabled(false);
            // Ak si manažoval QProcess, tu by si mohol čakať na jeho dokončenie alebo ho odpojiť

            for (QGraphicsItem* item : scene->items()) {
                QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(item);
                if (ellipse) {
                    QString ellipseName = ellipse->data(0).toString();  // Načítaj meno stavu
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
    // Nepotrebujeme čítať z UI, použijeme členskú premennú
    int guiListenPort_ = std::stoi(portGUI);

    guiSocket_->close();
    if (guiSocket_->bind(QHostAddress::AnyIPv4, guiListenPort_)) { // <<< POUŽÍVA SA TU
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
    // qWarning() << ...
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

    // --- 1. Get Ports from User ---
    // (You can reuse/adapt your existing dialog logic or simplify)
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
        // Note: No need to rebind automaton's socket, only GUI's
    }

    // Rebind GUI socket if its port changed
    if (portChanged) {
        bindGuiSocket();
        if (!guiSocket_->isValid()) { // Check if binding failed
             QMessageBox::critical(this, "Error", "Failed to bind GUI socket to new port. Cannot connect.");
             return;
        }
    }

    // --- 2. Clear Current State (Optional: Add confirmation) ---
    if (machine) { // If a machine is already loaded/running visualization
        // Optional: Ask user "Disconnect from current automaton and connect to new one?"
        qDebug() << "Clearing existing automaton view and model.";
        scene->clear(); // Clear graphics
        // Reset UI group boxes (variable, input, output lists) - you might need a helper function for this
        // TODO: Implement clearUILists();
        delete machine; // Delete the old machine object
        machine = nullptr;
        connectedAutomatonName = "";
        // ui->statusbar->clearMessage();
        // Disable interaction buttons until connected
        // TODO: ui->sendInputButton->setEnabled(false); ... etc.
        // Reset highlights, etc.
    }

    // --- 3. Send GET_STATUS Command ---
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
        // ui->statusbar->showMessage("Attempting to connect...", 0);

        // Optional: Start timeout timer
        /*
        if (!connectionTimeoutTimer) {
            connectionTimeoutTimer = new QTimer(this);
            connectionTimeoutTimer->setSingleShot(true);
            connect(connectionTimeoutTimer, &QTimer::timeout, this, [this]() {
                if (waitingForAutomatonInfo) {
                    qWarning() << "Connection attempt timed out.";
                    QMessageBox::warning(this, "Timeout", "No response received from the automaton.");
                    waitingForAutomatonInfo = false;
                    // ui->statusbar->showMessage("Connection timed out.", 5000);
                }
            });
        }
        connectionTimeoutTimer->start(5000); // 5 second timeout
        */
    }
}


///////////////////////////////////////////////////////////////////////////////////



void MainWindow::on_saveJsonButton_clicked() {
    qDebug() << "Save JSON button clicked.";

    // 1. Skontroluj, či existuje model na uloženie
    if (!machine) {
        QMessageBox::warning(this, "No Automaton", "There is no automaton model loaded or created to save.");
        qWarning() << "Save attempted but no machine model exists.";
        return;
    }

    // 2. Priprav názov súboru a cestu
    // Použi názov automatu ako predvolený názov súboru
    QString defaultFileName = QString::fromStdString(machine->getName()) + ".json";
    // Odstráň neplatné znaky pre názov súboru (miernejšie ako pre adresár)
    defaultFileName.replace(QRegExp("[\\\\/:*?\"<>|]"), "_");

    // Získaj predvolený adresár (napr. Dokumenty alebo posledný použitý)
    // Alebo môžeme cieliť priamo do `generated_automatons`, ak chceme prepisovať
    // Verzia pre adresár projektu:
    QString safeDirName = QString::fromStdString(machine->getName());
    safeDirName.replace(QRegExp("[^a-zA-Z0-9_.-]"), "_");
    QString appDirPath = QCoreApplication::applicationDirPath();
    QDir buildSubDir(appDirPath);
    buildSubDir.cdUp(); // Predpokladáme build adresár
    buildSubDir.cdUp(); // Do koreňa projektu
    buildSubDir.cdUp(); // Do koreňa projektu
    QString projectPath = buildSubDir.absolutePath();
    QString targetBaseDir = projectPath + "/generated_automatons";
    QString targetAutomatonDir = targetBaseDir + "/" + safeDirName;
    QString defaultPath = targetAutomatonDir + "/" + defaultFileName;


    // Alternatíva: Predvolený adresár Dokumenty
    // QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    // QString defaultPath = defaultDir + "/" + defaultFileName;

    // 3. Zobraz dialóg "Uložiť ako..."
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save Automaton As", // Názov dialógu
                                                    defaultPath,         // Predvolená cesta a súbor
                                                    "JSON Files (*.json);;All Files (*)"); // Filter súborov

    // 4. Skontroluj, či užívateľ vybral súbor
    if (fileName.isEmpty()) {
        qDebug() << "Save cancelled by user.";
        return; // Užívateľ stlačil Zrušiť alebo zavrel dialóg
    }

    // 5. Zabezpeč, aby mal súbor príponu .json (voliteľné, ale odporúčané)
     if (!fileName.endsWith(".json", Qt::CaseInsensitive)) {
         fileName += ".json";
     }
     qInfo() << "Saving automaton model to:" << fileName;

    // 6. Zavolaj funkciu na uloženie z JsonPersistence
    bool success = JsonPersistence::saveToFile(*machine, fileName.toStdString());

    // 7. Informuj užívateľa o výsledku
    if (success) {
        QMessageBox::information(this, "Save Successful", "Automaton model saved successfully to:\n" + fileName);
        qInfo() << "Automaton model saved successfully.";
        // Voliteľne: Ulož si cestu pre budúce použitie (napr. pre load)
        // lastUsedSavePath = QFileInfo(fileName).absolutePath();
    } else {
        QMessageBox::critical(this, "Save Failed", "Could not save the automaton model to the specified file.\nCheck console output for details.");
        qCritical() << "Failed to save automaton model to:" << fileName;
    }
}


void MainWindow::on_loadJsonButton_clicked() {
    qDebug() << "Load JSON button clicked.";
    GraphicsView* gView = ui->graphicsView; // Use the correct type

    if(gView) {
        gView->ensureVisible(QRectF(0, 0, 1, 1), 0, 0);
    }
    // 1. Priprav predvolenú cestu (napr. adresár projektu alebo posledný použitý)
    // Verzia pre adresár projektu:
    QString appDirPath = QCoreApplication::applicationDirPath();
    QDir buildSubDir(appDirPath);
    buildSubDir.cdUp(); // Predpokladáme build adresár
    buildSubDir.cdUp(); // Do koreňa projektu
    buildSubDir.cdUp(); // Do koreňa projektu
    QString projectPath = buildSubDir.absolutePath();
    QString defaultDir = projectPath; // Alebo /examples

    // Alternatíva: Posledný použitý adresár alebo Dokumenty
    // QString defaultDir = lastUsedLoadPath.isEmpty() ? QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) : lastUsedLoadPath;

    // 2. Zobraz dialóg "Otvoriť súbor..."
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Automaton from JSON", // Názov dialógu
                                                    defaultDir,                 // Predvolený adresár
                                                    "JSON Files (*.json);;All Files (*)"); // Filter súborov

    // 3. Skontroluj, či užívateľ vybral súbor
    if (fileName.isEmpty()) {
        qDebug() << "Load cancelled by user.";
        return; // Užívateľ stlačil Zrušiť alebo zavrel dialóg
    }

    qInfo() << "Attempting to load automaton model from:" << fileName;

    // 4. Zavolaj funkciu na načítanie z JsonPersistence
    std::unique_ptr<Machine> loadedMachine = JsonPersistence::loadFromFile(fileName.toStdString());

    // 5. Skontroluj, či načítanie prebehlo úspešne
    if (!loadedMachine) {
        QMessageBox::critical(this, "Load Failed", "Could not load or parse the automaton model from the specified file.\nCheck console output for details.");
        qCritical() << "Failed to load machine from:" << fileName;
        return; // Chyba pri načítaní
    }

    qInfo() << "Successfully loaded machine '" << QString::fromStdString(loadedMachine->getName()) << "' from JSON.";

    // 6. Nahraď aktuálny model novým (ak nejaký existoval) a aktualizuj GUI
    //    Najprv vyčisti starý stav GUI
    qDebug() << "Clearing old model and GUI elements...";
    scene->clear(); // Vyčisti grafickú scénu
    // TODO: Vyčisti zoznamy premenných, vstupov, výstupov (podobne ako v populateUIFromModel)
    //       Môžeš vytvoriť pomocnú funkciu clearUILists()
    
    //clearVariableList(); // Predpokladané pomocné funkcie
    //clearInputList();
    //clearOutputList();


    // Vymaž starý objekt Machine, ak existoval
    if (machine) {
        delete machine;
        machine = nullptr;
    }

    // Prevezmi vlastníctvo nového objektu
    machine = loadedMachine.release();
    qDebug() << "Took ownership of the newly loaded machine object.";
    


    // Aktualizuj interné premenné GUI (porty atď.), ak je to potrebné
    // V tomto prípade pri načítaní asi nie je potrebné meniť porty,
    // tie sa používajú skôr pri pripájaní k bežiacemu automatu.
    // connectedAutomatonName = QString::fromStdString(machine->getName()); // Možno aktualizovať

    // Prekresli automat a naplň UI dáta z nového modelu
    qDebug() << "Redrawing automaton and populating UI from the new model...";
    redrawAutomatonFromModel();
    populateUIFromModel();

    setInputFieldsEnabled(false);

    // Metóda 1: Priame nastavenie viditeľnosti (odporúčané)

    // Povoľ relevantné tlačidlá (napr. Save, Run, Edit...)


    // ui->terminateAutomatButton->setEnabled(false); // Terminate nemá zmysel pre nebežiaci automat
    // ui->connectButton->setEnabled(true); // Connect by malo byť vždy povolené?

    QMessageBox::information(this, "Load Successful", "Automaton model loaded successfully from:\n" + fileName);
    // Voliteľne: Ulož si cestu pre budúce použitie
    // lastUsedLoadPath = QFileInfo(fileName).absolutePath();
}

void MainWindow::redrawAutomatonFromModel() {
    // 1. Základné kontroly a vyčistenie
    if (!machine) {
        qWarning() << "Cannot redraw: machine model is null.";
        return;
    }
    if (!scene) {
         qWarning() << "Cannot redraw: scene is null.";
         return;
    }
    qDebug() << "Redrawing automaton '" << QString::fromStdString(machine->getName()) << "' from loaded model.";
    scene->clear(); // Vyčisti scénu od starých prvkov
    // Resetni počítadlá ID, ak ich používaš na generovanie nových ID pri kreslení
    objectStateId = 0;
    objectTransitionId = 0;

    // 2. Získaj stavy a vypočítaj rozloženie
    const auto& statesMap = machine->getStates();
    int stateCount = statesMap.size();
    if (stateCount == 0) {
        qDebug() << "No states to draw.";
        return; // Nie je čo kresliť
    }

    // Výpočet rozmerov mriežky (gridu)
    int itemsPerRow = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(stateCount))));
    qDebug() << "Calculated items per row:" << itemsPerRow;

    // Rozmery a medzery medzi prvkami
    const qreal itemWidth = 60;
    const qreal itemHeight = 60;
    const qreal hSpacing = 100; // Horizontálna medzera
    const qreal vSpacing = 100; // Vertikálna medzera
    const qreal margin = 20;   // Okraj od kraja scény

    // Mapa na uloženie vytvorených QGraphicsItemGroup pre stavy (kľúčom je meno stavu)
    std::map<std::string, QGraphicsItemGroup*> stateSceneItems;

    // 3. Vykresli Stavy (v mriežke)
    int currentRow = 0;
    int currentCol = 0;
    for (const auto& pair : statesMap) {
        const std::string& stateName = pair.first;
        const State* state = pair.second.get();
        // Získaj ID stavu (buď z objektu alebo vygeneruj)
        // int stateId = state->getStateId(); // Ak máš metódu getStateId
        int stateId = objectStateId++;       // Alebo generuj nové

        // Vypočítaj pozíciu
        qreal x = margin + currentCol * (itemWidth + hSpacing);
        qreal y = margin + currentRow * (itemHeight + vSpacing);

        // --- Vytvorenie vizuálnej reprezentácie (rovnaký kód ako v on_addStateButton_clicked) ---
        QGraphicsItemGroup *group = new QGraphicsItemGroup();
        QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, itemWidth, itemHeight, group);

        // Nastav farbu podľa toho, či je počiatočný (alebo neskôr aktívny)
        bool isInitial = (state == machine->getInitialState());
        ellipse->setBrush(QBrush(isInitial ? activeStateColor : normalStateColor)); // Zvýrazni počiatočný
        // ellipse->setBrush(QBrush(normalStateColor)); // Alebo začni všetko normálne

        ellipse->setPen(QPen(Qt::black));
        ellipse->setData(0, QVariant(QString::fromStdString(stateName))); // Meno stavu do elipsy (pre updateStateItemColor)

        QGraphicsTextItem *text = new QGraphicsTextItem(QString::fromStdString(stateName), group);
        QRectF textRect = text->boundingRect();
        qreal textX = (itemWidth - textRect.width()) / 2;
        qreal textY = (itemHeight - textRect.height()) / 2;
        text->setPos(textX, textY);

        group->setPos(x, y);
        group->setFlag(QGraphicsItem::ItemIsMovable);
        group->setFlag(QGraphicsItem::ItemIsSelectable);
        group->setData(0, QVariant(stateId)); // Ulož ID stavu
        group->setData(1, "state");            // Identifikátor typu "state"
        group->setData(2, QVariant(QString::fromStdString(stateName))); // Ulož aj meno stavu do groupy

        // TODO: Znovu pripojiť signál na pohyb, ak chceš dynamické prekresľovanie šípok pri pohybe
        // connect(scene, &QGraphicsScene::changed, ...)

        scene->addItem(group); // Pridaj skupinu do scény
        stateSceneItems[stateName] = group; // Ulož skupinu do mapy pre kreslenie prechodov
        qDebug() << " Drew state:" << QString::fromStdString(stateName) << "at (" << x << "," << y << ")";

        // Posuň sa v mriežke
        currentCol++;
        if (currentCol >= itemsPerRow) {
            currentCol = 0;
            currentRow++;
        }
    }

    // 4. Vykresli Prechody
    qDebug() << "Drawing transitions...";
    const auto& transitionsList = machine->getTransitions();
    for (const auto& trans_ptr : transitionsList) {
        const Transition* transition = trans_ptr.get();
        const std::string& sourceName = transition->getSourceState().getName();
        const std::string& targetName = transition->getTargetState().getName();
        // Získaj ID prechodu (buď z objektu alebo vygeneruj)
        // int transId = transition->getTransitionId();
        int transId = objectTransitionId++;

        // Nájdi zodpovedajúce QGraphicsItemGroup pre stavy
        auto itSource = stateSceneItems.find(sourceName);
        auto itTarget = stateSceneItems.find(targetName);

        if (itSource != stateSceneItems.end() && itTarget != stateSceneItems.end()) {
            QGraphicsItemGroup* startItemGroup = itSource->second;
            QGraphicsItemGroup* endItemGroup = itTarget->second;

            // Získaj stredové body skupín
            QPointF startCenter = startItemGroup->sceneBoundingRect().center();
            QPointF endCenter = endItemGroup->sceneBoundingRect().center();

            // Vytvor label prechodu (z condition a delay)
            QString label = QString::fromStdString(transition->getCondition());
            // Ak getCondition nevracia delay, pridaj ho:
            // int delayMs = transition->getDelayMs();
            // if (delayMs > 0 && !label.contains("@")) { // Pridaj len ak tam už nie je
            //     label += " @" + QString::number(delayMs);
            // }

            // Vykresli šípku
            QVariant actualStartPos, actualEndPos; // Nepoužívame ich tu, ale drawArrow ich môže potrebovať
            drawArrow(startCenter, endCenter, label, transId, scene, &actualStartPos, &actualEndPos);
             qDebug() << "  Drew transition:" << QString::fromStdString(sourceName) << "->" << QString::fromStdString(targetName) << "Label:" << label;

        } else {
            qWarning() << " Skipping transition draw: Cannot find scene item for source '" << QString::fromStdString(sourceName) << "' or target '" << QString::fromStdString(targetName) << "'";
        }
    }

    qDebug() << "Finished redrawing automaton.";
    
    GraphicsView* gView = ui->graphicsView; // Use the correct type

    if(gView) {
        gView->ensureVisible(QRectF(0, 0, 1, 1), 0, 0);
    }
}

void MainWindow::setInputFieldsEnabled(bool enabled) {
    qDebug() << "Setting input fields enabled state to:" << enabled;
    // Nájdi group box
    QGroupBox* groupBox = ui->editInGroupBox;
    if (!groupBox) {
        qWarning() << "setInputFieldsEnabled: editInGroupBox not found.";
        return;
    }

    // Nájdi všetky QLineEdit vnútri group boxu
    QList<QLineEdit*> inputEdits = groupBox->findChildren<QLineEdit*>();

    for (QLineEdit* edit : inputEdits) {
        edit->setEnabled(enabled);
        // Prípadne uprav štýl, ak si použil setReadOnly a setStyleSheet
        // if (enabled) {
        //     edit->setReadOnly(false);
        //     edit->setStyleSheet(""); // Reset štýlu
        // } else {
        //     edit->setReadOnly(true);
        //     edit->setStyleSheet("background-color: #f0f0f0;");
        // }
    }
    qDebug() << "Updated enabled state for" << inputEdits.count() << "input fields.";
}

// POMOCNÁ funkcia na rekurzívne čistenie layoutov - BEZPEČNEJŠIA VERZIA
void MainWindow::clearLayout(QLayout* layout) {
    if (!layout) return;
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        // Odpojiť všetky signály widgetu
        if (QWidget* widget = item->widget()) {
            widget->disconnect(); // <<< DÔLEŽITÉ
            widget->deleteLater();
        }
        else if (QLayout* subLayout = item->layout()) {
            clearLayout(subLayout);
            delete subLayout;
        }
    }
    qDebug() << "Layout cleared (items removed/deleted).";
}

// Upravená clearVariableList, ktorá volá pomocnú funkciu
void MainWindow::clearVariableList() {
    qDebug() << "Clearing Variable List UI";
    if (QLayout* layout = ui->editVarGroupBox->layout()) {
         qDebug() << "Layout found, calling clearLayout...";
        clearLayout(layout); // Zavolaj bezpečnú pomocnú funkciu
    } else {
         qDebug() << "No layout found for editVarGroupBox.";
    }
}

void MainWindow::populateUIFromModel() {
    // 1. Basic Check
    if (!machine) {
         qWarning() << "Cannot populate UI: machine model is null.";
         return;
    }
     qDebug() << "Populating UI elements from loaded model for machine:" << QString::fromStdString(machine->getName());

    // --- Clear existing UI lists FIRST ---
    // (Assuming you have implemented these as shown previously)

    // --- 2. Populate Variables ---
    qDebug() << "Populating Variables...";
    const auto& variablesMap = machine->getVariables();
    if (!variablesMap.empty()) clearVariableList();
    qDebug() << "0";
    for (const auto& pair : variablesMap) {
        const Variable* var = pair.second.get(); // Get raw pointer from unique_ptr
        qDebug() << "1";
        qDebug() << " Adding variable to UI:" << QString::fromStdString(var->getName());
        qDebug() << "2";
        // Call your existing function to add the row to the UI
        addVarRowToGUI(var->getName(), var->getTypeHint(), var->getValueAsString());
        qDebug() << "3";
    }

    qDebug() << "4";



    // --- 3. Populate Inputs ---
    qDebug() << "Populating Inputs...";
    const auto& inputsMap = machine->getInputs();
    if (!inputsMap.empty()) clearInputList();
    for (const auto& pair : inputsMap) {
         const Input* input = pair.second.get();
         qDebug() << " Adding input to UI:" << QString::fromStdString(input->getName());
         // Call your existing function to add the row
         addInputRowToGUI(input->getName());
         // Optionally, set the initial value in the QLineEdit if you store it
         QLineEdit* le = ui->editInGroupBox->findChild<QLineEdit*>(QString::fromStdString(input->getName()));
         if(le) {
             // Input value might be empty initially, use value_or
             le->setText(QString::fromStdString(input->getLastValue().value_or("")));
         }
    }



    // --- 4. Populate Outputs ---
    qDebug() << "Populating Outputs...";
    const auto& outputsMap = machine->getOutputs();
    if (!outputsMap.empty()) clearOutputList();
    QGroupBox *groupBox = ui->trackingOutputsGroupBox; // Get the target group box
    if(groupBox) {
        // Ensure layout exists (get or create)
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(groupBox->layout());
        if (!layout) {
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

            // Create the display text (e.g., "outputName = ?")
             // We don't usually load the *last sent value* from the JSON definition,
             // so we'll initialize it with a placeholder or just the name.
            QString labelText = QString::fromStdString(outputName) + " = ?"; // Placeholder value

            // --- Create Widgets for the output row ---
             QLabel *outputLabel = new QLabel(labelText, groupBox);
             // Set object name using the ACTUAL output name for later updates
             outputLabel->setObjectName(QString::fromStdString(outputName));

             // --- Optional: Add Delete Button for Outputs ---
             QHBoxLayout *rowLayout = new QHBoxLayout();
             rowLayout->addWidget(outputLabel); // Add label first

             QPushButton *deleteButton = new QPushButton("x", groupBox);
             deleteButton->setFixedSize(20, 20); // Make button small
             deleteButton->setToolTip("Delete this output");
             rowLayout->addWidget(deleteButton); // Add delete button

             // Connect delete button
             connect(deleteButton, &QPushButton::clicked, this, [this, layout, rowLayout, outputName]() {
                 qDebug() << "Delete button clicked for output:" << QString::fromStdString(outputName);
                 // Remove from machine first
                 machine->removeOutput(outputName);
                 // Remove the row layout and its widgets from the UI layout
                 layout->removeItem(rowLayout);
                 // Delete widgets within the rowLayout FIRST
                 QLayoutItem *childItem;
                 while((childItem = rowLayout->takeAt(0)) != nullptr) {
                     delete childItem->widget();
                     delete childItem;
                 }
                 delete rowLayout; // Then delete the row layout itself
                 qDebug() << "Output removed from UI and machine:" << QString::fromStdString(outputName);
                 // Optional: Hide groupbox if last output removed
                 if(layout->count() <= 1) { // Check if only stretch remains
                     ui->trackingOutputsGroupBox->setVisible(false);
                 }
             });
             // --- End Optional Delete Button ---

            // Add the row layout (containing label and button) to the main VBox layout
            // Insert at index 0 to add new items at the top (before the stretch)
            layout->insertLayout(0, rowLayout);

        } // End for loop over outputs

    } else {
        qWarning() << "Cannot populate outputs: ui->trackingOutputsGroupBox is null.";
    }
     qDebug() << "Finished populateUIFromModel.";
}

// Podobne uprav aj clearInputList a clearOutputList, aby volali clearLayout
void MainWindow::clearInputList() {
    qDebug() << "Clearing Input List UI";
    if (QLayout* layout = ui->editInGroupBox->layout()) {
        qDebug() << "Layout found, calling clearLayout...";
        clearLayout(layout);
    } else {
        qDebug() << "No layout found for editInGroupBox.";
    }
}

void MainWindow::clearOutputList() {
    qDebug() << "Clearing Output List UI";
   if (QLayout* layout = ui->trackingOutputsGroupBox->layout()) {
        qDebug() << "Layout found, calling clearLayout...";
        clearLayout(layout);
        // Ak si pridával stretch, musíš ho pridať znova, ak chceš
        // layout->addStretch();
    } else {
         qDebug() << "No layout found for trackingOutputsGroupBox.";
    }
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
 * @brief Slot volaný po kliknutí na tlačidlo premenovania automatu.
 */
void MainWindow::on_renameButton_clicked() {
    qDebug() << "Rename Automaton button clicked.";

    // 1. Skontroluj, či existuje model na premenovanie
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
        // Ak by Machine::setName mohla vyhodiť výnimku (napr. pri neplatnom mene),
        // je dobré to dať do try-catch. Ak nie, try-catch nie je nutný.
         machine->setName(newName); // <<< TU ZAVOLAJ METÓDU Z Machine
    } catch (const std::exception& e) {
         qCritical() << "Error renaming automaton in the model:" << e.what();
         QMessageBox::critical(this, "Rename Error", QString("Failed to set the new name in the model: %1").arg(e.what()));
         return; // Chyba pri zmene mena v modeli
    }
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

    valueEdit->setEnabled(false);

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
    QString oldName = QString::fromStdString(outputName);

    outputName = outputName + " = value";

    // 3. Vytvor nový QLabel pre zobrazenie mena vstupu
    QLabel *newLabel = new QLabel(QString::fromStdString(outputName), groupBox); // Použi qInputName
    newLabel->setObjectName(oldName);  // Nastav objectName == outputName (napr. "out")

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
        rowLayout->deleteLater(); // Alebo necháš Qt, keď zanikne rodič.

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
    dialog.setMinimumSize(400, 400);
    QFont font = dialog.font();
    font.setPointSize(16);  // Zvýši font aj pre tlačidlá
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
    

    // --- 4. Create Visual Representation (Ellipse + Name) ---
    qreal width = 60; qreal height = 60;
    int itemCount = scene->items().count();
    qreal x = (itemCount % 5) * (width + 20);
    qreal y = (itemCount / 5) * (height + 20);

    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(0, 0, width, height);
    ellipse->setBrush(QBrush(Qt::cyan));
    ellipse->setPen(QPen(Qt::black));
    ellipse->setData(0, QVariant(stateName));


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

        if(addingTransitionMode) {
            return;
        }

        if (group->isSelected()) {
            //qDebug() << "Group moved. New position:" << group->pos();
            //qDebug() << "Moved state ID:" << group->data(0).toInt();
            
            //StateGraphicItem* stateGraphicItem = machine->getStateGraphicItem(group->data(0).toInt());
            //stateGraphicItem->updateTransitions(scene);

            State *state = machine->getState(group->data(0).toInt());
            if(state->currentPos != group->sceneBoundingRect().center()) {
                state->currentPos = group->sceneBoundingRect().center();
                state->updateTransitionPositions(scene, group, machine);

            }
            
        }
    });
   
    //connect(group, &QGraphicsItemGroup::yChanged, this, [this, group]() {
        //qDebug() << "Group moved. New position:" << group->pos();
        // Handle the movement event here
    //});

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
        // Metóda 1: ensureVisible - jednoduchšie
        // Použi sceneBoundingRect() skupiny, aby si bol istý, že vidíš celý prvok
         gView->ensureVisible(group->sceneBoundingRect(), 50, 50); // 50px okraj okolo prvku
         qDebug() << "Ensured newly added state is visible.";

        // Metóda 2: centerOn - vycentruje pohľad na stred prvku
        // gView->centerOn(group->sceneBoundingRect().center());
        // qDebug() << "Centered view on newly added state.";
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
        qDebug() << "handleStateClick called. Not in transition mode.";
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

        if(!addingTransitionMode && item->data(1).toString() == "transition") {
            editTransition(item);
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
           

            QPointF actualStartPos;
            QPointF actualEndPos;

            QGraphicsItemGroup *transitionGroup = MainWindowUtils::drawArrow(startItemForTransition->sceneBoundingRect().center(), endItemForTransition->sceneBoundingRect().center(), QString::fromStdString(condition.toStdString()), objectTransitionId, scene, &actualStartPos, &actualEndPos);
            

            
            this->startStateForTransition->addOutgoingTransitionGroup(transitionGroup, actualStartPos, actualEndPos);
            this->endStateForTransition->addIncomingTransitionGroup(transitionGroup, actualStartPos, actualEndPos);
            
            

            qDebug() << "---------------------------------------------------";
            qDebug() << "Transition ID:" << objectTransitionId;

            qDebug() << "Start state name:" << QString::fromStdString(startStateForTransition->getName());
            qDebug() << "End state name:" << QString::fromStdString(endStateForTransition->getName());


            startStateForTransition->printTransitions();
            endStateForTransition->printTransitions();

            
            qDebug() << "---------------------------------------------------";
            
            
            
            //scene->addItem(transitionGroup);


            try {
                if(this->startStateForTransition && this->endStateForTransition) {
                    std::unique_ptr<Transition> newTransition = std::make_unique<Transition>(
                        this->startStateForTransition,
                        this->endStateForTransition,
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
    /*
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
    */
    else {
        qDebug() << "Clicked item is not a valid state or line item.";
    }
}

void MainWindow::editTransition(QGraphicsItemGroup *item) {

    int transitionId = item->data(0).toInt(); // Retrieve the state ID stored in the item
    Transition* transition = machine->getTransition(transitionId); // Get the state object using the ID
    if (!transition) {
        qDebug() << "State not found for ID:" << transitionId;
        return;
    }
    


    
    std::string newName;
    std::string newAction;
    
    std::string currentName = transition->getCondition();
    std::string currentAction = std::to_string(transition->getDelayMs());
    

    ProccessMultipleArgsInputEditDialog("Edit State", "Enter new state name:", "Enter new action:",
        currentName, currentAction, &newName, &newAction);

    qDebug () << "New name:" << QString::fromStdString(newName);

    transition->setCondition(newName);
    transition->setDelay(std::stoi(newAction));

    /*

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

        //qDebug() << "State item text updated to:" << newNameQt;
    
    qWarning() << "Could not find text item within the group to update.";
    
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










