/**
 * @file JsonCreator.cpp
 * @brief Implementation of the JsonCreator class.
 * Renamed from MachineJsonSerializer.cpp
 */
#include "JsonCreator.h" // Include the corresponding header file
#include "Machine.h"    // Need full definition of Machine and its components
#include "State.h"
#include "Transition.h"
#include "Variable.h"
#include "Input.h"
#include "Output.h"

#include <QFile>         // For writing to file
#include <QJsonDocument> // For creating the JSON structure
#include <QJsonObject>   // For JSON objects ({ "key": value })
#include <QJsonArray>    // For JSON arrays ([ value1, value2 ])
#include <QJsonValue>    // For representing JSON values (string, number, bool, null, object, array)
#include <QDebug>        // For debug output

/**
 * @brief Saves the machine definition to a JSON file.
 */
// Use the new class name for the static method definition
bool JsonCreator::saveToFile(const Machine& machine, const QString& filename) {
    qDebug() << "Attempting to save machine '" << QString::fromStdString(machine.getName()) << "' to JSON file:" << filename;

    // --- Create the main JSON object ---
    QJsonObject rootObject;

    // --- Add basic machine info ---
    rootObject["jmenoAutomatu"] = QString::fromStdString(machine.getName());
    rootObject["komentar"] = "Automat vygenerovaný nástrojom"; // Placeholder
    /*
    // --- Add Inputs ---
    QJsonArray inputsArray;
    for (const auto& pair : machine.getInputs()) {
        inputsArray.append(QString::fromStdString(pair.second->getName()));
    }
    rootObject["vstupy"] = inputsArray;

    // --- Add Outputs ---
    QJsonArray outputsArray;
    for (const auto& pair : machine.getOutputs()) {
        outputsArray.append(QString::fromStdString(pair.second->getName()));
    }
    rootObject["vystupy"] = outputsArray;

    // --- Add Variables ---
    QJsonArray variablesArray;
    for (const auto& pair : machine.getVariables()) {
        const Variable* var = pair.second.get();
        QJsonObject varObject;
        varObject["jmeno"] = QString::fromStdString(var->getName());
        varObject["typ"] = QString::fromStdString(var->getTypeHint());
        varObject["hodnota"] = QString::fromStdString(var->getValueAsString());
        variablesArray.append(varObject);
    }
    rootObject["promenne"] = variablesArray;
    */
    // --- Add States ---
    QJsonArray statesArray;
    const State* initialStatePtr = machine.getInitialState();
    QString initialStateName = initialStatePtr ? QString::fromStdString(initialStatePtr->getName()) : "";

    for (const auto& pair : machine.getStates()) {
        const State* state = pair.second.get();
        QJsonObject stateObject;
        QString currentName = QString::fromStdString(state->getName());
        stateObject["jmeno"] = currentName;
        stateObject["akce"] = QString::fromStdString(state->getAction());
        stateObject["pocatecni"] = (currentName == initialStateName); // Simplified boolean assignment
        statesArray.append(stateObject);
    }
    rootObject["stavy"] = statesArray;
    

    // --- Add Transitions ---
    QJsonArray transitionsArray;
    for (const auto& transitionPtr : machine.getTransitions()) {
        const Transition* trans = transitionPtr.get();
        QJsonObject transObject;
        transObject["zdroj"] = QString::fromStdString(trans->getSourceState().getName());
        transObject["cil"] = QString::fromStdString(trans->getTargetState().getName());
        transObject["condition"] = QString::fromStdString(trans->getCondition());
        transObject["delay"] = QString::number(trans->getDelayMs());

        //int delay = trans->getDelayMs();
        //if (delay == -1) {
             // TODO: Store the variable name for the delay if available in Transition
             //transObject["zpozdeni"] = "@variable_placeholder"; // Placeholder
        //} else {
             //transObject["zpozdeni"] = delay;
        //}
        // Clean up empty/default values
        //if (transObject["udalost"].toString().isEmpty()) transObject.remove("udalost");
        //if (transObject["podminka"].toString().isEmpty()) transObject.remove("podminka");
        //if (delay <= 0) transObject.remove("zpozdeni");

        transitionsArray.append(transObject);
    }
    rootObject["prechody"] = transitionsArray;
    
    // --- Convert the root object to a JSON document ---
    QJsonDocument jsonDoc(rootObject);

    // --- Open the file for writing ---
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qWarning() << "Could not open file for writing JSON:" << filename << file.errorString();
        return false;
    }

    // --- Write the JSON document to the file ---
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Machine successfully saved to JSON:" << filename;
    return true;
}
