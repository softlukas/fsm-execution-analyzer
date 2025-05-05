/**
 * @file JsonPersistence.cpp
 * @brief Implementation of the JsonPersistence class using nlohmann/json for serialization/deserialization.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#include "JsonPersistance.h"
#include "core/Machine.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <QDebug>
#include "json_conversions.h"

// Use alias for nlohmann::json
using json = nlohmann::json;

bool JsonPersistence::saveToFile(const Machine& machine, const std::string& filename) {
    std::cout << "Attempting to save machine '" << machine.getName() << "' to JSON file: " << filename << std::endl;
    try {
        // Convert the Machine object to a nlohmann::json object
        // This implicitly calls the `to_json(json& j, const Machine& m)` function
        // defined in "json_conversions.cpp".
        json machine_json = machine;

        // Write the JSON object to the specified file
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file for writing JSON: " << filename << std::endl;
            return false;
        }

        // Write the JSON data to the file with pretty printing (indentation of 2 spaces).
        // Use the dump() method from nlohmann/json.
        outFile << machine_json.dump(2);
        outFile.close(); // Close the file stream

        std::cout << "Machine successfully saved to JSON: " << filename << std::endl;
        return true;

    } catch (const json::exception& e) {
        std::cerr << "Error during JSON serialization or file writing: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred during saving: " << e.what() << std::endl;
        return false;
    }
}

std::unique_ptr<Machine> JsonPersistence::loadFromFile(const std::string& filename) {

    qDebug() << "Attempting to load machine from JSON file:" << QString::fromStdString(filename); // Use qDebug
    try {
        // Read and parse the JSON file
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
             qCritical() << "Error: Could not open file for reading JSON:" << QString::fromStdString(filename); // Use qCritical
            return nullptr;
        }
        json loaded_json; // Create a json object to store the parsed data
        
        // Parse the file content into the json object.
        // Catch parsing errors specifically.
        try {
            inFile >> loaded_json;
        } catch (const json::parse_error& e) {
            inFile.close();
            qCritical() << "Error parsing JSON file:" << QString::fromStdString(filename) << "-" << e.what();
            return nullptr;
        }
        inFile.close();
        qDebug() << "JSON file parsed successfully.";

        // Create a new Machine object and populate it using from_json 
        // Extract the machine name from the JSON data (use default if not found)
        std::string machineName = loaded_json.value("automaton_name", "UnnamedMachine");
        qDebug() << "Creating Machine object for:" << QString::fromStdString(machineName);
        // Create a new Machine instance owned by a unique_ptr
        auto machine_ptr = std::make_unique<Machine>(machineName);

        // Call the custom `from_json` function (from json_conversions.cpp)
        // This function populates the already existing Machine object (*machine_ptr).
        qDebug() << "Calling from_json(json, Machine)...";
        from_json(loaded_json, *machine_ptr); // Pass the Machine object by reference
        qDebug() << "from_json(json, Machine) finished.";

        qInfo() << "Machine successfully loaded from JSON:" << QString::fromStdString(filename);
        return machine_ptr; // Vráť unique_ptr

    } catch (const json::exception& e) {
        // Catch errors during JSON *access* (e.g., .at(), .value()) within from_json
         qCritical() << "Error during JSON data access/conversion:" << e.what() << "in file:" << QString::fromStdString(filename);
        return nullptr;
    } catch (const std::exception& e) {
        // Catch other standard exceptions (e.g., from std::make_unique, map operations)
         qCritical() << "An unexpected standard error occurred during loading:" << e.what() << "from file:" << QString::fromStdString(filename);
        return nullptr;
    } catch (...) {
        // Catch any other unknown exceptions
        qCritical() << "An unknown error occurred during loading from file:" << QString::fromStdString(filename);
        return nullptr;
    }
}