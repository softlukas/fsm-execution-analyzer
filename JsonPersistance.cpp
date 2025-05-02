/**
 * @file JsonPersistence.cpp
 * @brief Implementation of the JsonPersistence class using nlohmann/json.
 */
#include "JsonPersistance.h" // Zmenený názov headeru
#include "Machine.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream> // Pre std::cerr/cout
#include <stdexcept> // Pre std::runtime_error

// Include súbor s to_json / from_json funkciami
#include "json_conversions.h" // <- Sem si dal tie funkcie vyššie

using json = nlohmann::json;

/**
 * @brief Saves the machine definition to a JSON file using nlohmann/json.
 */
bool JsonPersistence::saveToFile(const Machine& machine, const std::string& filename) {
    std::cout << "Attempting to save machine '" << machine.getName() << "' to JSON file: " << filename << std::endl;
    try {
        // --- 1. Prevod na nlohmann::json objekt v pamäti ---
        // Toto automaticky zavolá tvoju funkciu: to_json(json& j, const Machine& m)
        json machine_json = machine;

        // --- 2. Zápis do súboru ---
        std::ofstream outFile(filename);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file for writing JSON: " << filename << std::endl;
            return false;
        }
        // Zapíš pekne formátovaný JSON (s odsadením 2 medzery)
        // Použi .dump() metódu z nlohmann/json
        outFile << machine_json.dump(2);
        outFile.close();

        std::cout << "Machine successfully saved to JSON: " << filename << std::endl;
        return true;

    } catch (const json::exception& e) {
        std::cerr << "Error during JSON serialization or file writing: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        // Zachytenie iných potenciálnych chýb
        std::cerr << "An unexpected error occurred during saving: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Loads the machine definition from a JSON file using nlohmann/json.
 * @param filename The path to the JSON file to load.
 * @return std::unique_ptr<Machine> A pointer to the loaded Machine object, or nullptr on failure.
 */
std::unique_ptr<Machine> JsonPersistence::loadFromFile(const std::string& filename) {
    /*
    std::cout << "Attempting to load machine from JSON file: " << filename << std::endl;
    try {
        // --- 1. Načítanie a parsovanie súboru ---
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
             std::cerr << "Error: Could not open file for reading JSON: " << filename << std::endl;
            return nullptr;
        }
        json loaded_json;
        inFile >> loaded_json;
        inFile.close();

        // --- 2. Vytvor nový Machine objekt a naplň ho pomocou from_json ---
        // Získaj meno z JSONu pre konštruktor Machine
        std::string machineName = loaded_json.value("automaton_name", "UnnamedMachine");
        auto machine_ptr = std::make_unique<Machine>(machineName);

        // Zavolaj from_json, ktorá naplní už existujúci objekt
        from_json(loaded_json, *machine_ptr); // Odovzdaj dereferencovaný pointer (referenciu)

        std::cout << "Machine successfully loaded from JSON: " << filename << std::endl;
        return machine_ptr; // Vráť unique_ptr

    } catch (const json::exception& e) {
         std::cerr << "Error during JSON parsing or conversion: " << e.what() << std::endl;
        return nullptr;
    } catch (const std::exception& e) {
         std::cerr << "An unexpected error occurred during loading: " << e.what() << std::endl;
        return nullptr;
    }
        */
}
