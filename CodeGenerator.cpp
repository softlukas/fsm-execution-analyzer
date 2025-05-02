#include "CodeGenerator.h"
#include "Machine.h"
#include "nlohmann/json.hpp" // Potrebuješ pre json objekt
#include "inja/inja.hpp"     // Potrebuješ pre renderovanie
#include <fstream>           // Pre čítanie šablóny
#include <iostream>          // Pre debug
#include <stdexcept>         // Pre výnimky

// Include súbor s to_json funkciami
#include "json_conversions.h" // <- Tu máš definované to_json

using json = nlohmann::json;

CodeGenerator::CodeGenerator(const std::string& templatePath)
    : templateFilePath(templatePath) {}

std::string CodeGenerator::generate(const Machine& machine) {
    std::cout << "[CodeGen] Starting code generation for machine: " << machine.getName() << std::endl;
    std::cout << "[CodeGen] Using template: " << templateFilePath << std::endl;

    // --- 1. Preveď Machine na nlohmann::json ---
    /*json machine_data;
    try {
        to_json(machine_data, machine); // <<< Zavolaj tvoju to_json funkciu
        // Debug výpis JSON dát (voliteľné, ale užitočné)
        // std::cout << "[CodeGen] JSON data:\n" << machine_data.dump(2) << std::endl;
    } catch (const json::exception& e) {
        std::cerr << "[CodeGen] Error converting Machine to JSON: " << e.what() << std::endl;
        throw GenerationError(std::string("JSON conversion failed: ") + e.what());
    }*/

    // --- 1. Read and Parse JSON data from file ---
    json machine_data;
    const std::string jsonFilePath = "automaton.json"; // Hardcoded filename

    try {
        std::ifstream inFile(jsonFilePath);
        if (!inFile.is_open()) {
            throw GenerationError("Could not open JSON file for reading: " + jsonFilePath);
        }

        // Parse the JSON directly from the input stream
        inFile >> machine_data;
        inFile.close(); // Good practice, though destructor handles it

        std::cout << "[CodeGen] Successfully loaded and parsed JSON from: " << jsonFilePath << std::endl;
        // Debug výpis JSON dát (voliteľné, ale užitočné)
        // std::cout << "[CodeGen] JSON data:\n" << machine_data.dump(2) << std::endl;

    } catch (const json::parse_error& e) {
        // Catch errors specifically from nlohmann::json parsing
        std::cerr << "[CodeGen] Error parsing JSON file '" << jsonFilePath << "': " << e.what() << std::endl;
        throw GenerationError(std::string("JSON parsing failed: ") + e.what());
    } catch (const std::exception& e) {
        // Catch other potential errors (like file stream errors)
        std::cerr << "[CodeGen] Error reading JSON file '" << jsonFilePath << "': " << e.what() << std::endl;
        throw GenerationError(std::string("JSON file reading failed: ") + e.what());
    }

    // --- 2. Načítaj a Spracuj Šablónu pomocou Inja ---
    try {
        inja::Environment env; // Vytvor Inja prostredie
        // Načítaj šablónu zo súboru
        inja::Template code_template = env.parse_template(templateFilePath);

        // Vyrenderuj šablónu s dátami
        std::cout << "[CodeGen] Rendering template..." << std::endl;
        std::string generated_code = env.render(code_template, machine_data); // <<< Kúzlo Inja!
        std::cout << "[CodeGen] Template rendered successfully." << std::endl;

        return generated_code; // Vráť vygenerovaný C++ kód

    } catch (const std::exception& e) {
        std::cerr << "[CodeGen] Error processing template '" << templateFilePath << "': " << e.what() << std::endl;
        throw GenerationError(std::string("Template processing failed: ") + e.what());
    }
}