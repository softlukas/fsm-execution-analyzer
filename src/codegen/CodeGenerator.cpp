/**
 * @file CodeGenerator.cpp
 * @brief Implements the CodeGenerator class for generating C++ automaton interpreter code.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#include "CodeGenerator.h"
#include "core/Machine.h"
#include "nlohmann/json.hpp"
#include "inja/inja.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "persistence/json_conversions.h"

using json = nlohmann::json;

/**
 * @brief Constructs a CodeGenerator instance.
 * @param templatePath The file path to the Inja template used for code generation.
 */
CodeGenerator::CodeGenerator(const std::string& templatePath)
    : templateFilePath(templatePath) {}

/**
 * @brief Generates C++ source code for the automaton interpreter based on a JSON definition file.
 * @details Reads the automaton structure from the specified JSON file, loads the Inja template,
 *          and renders the template using the parsed JSON data.
 * @param machine The Machine object instance (currently used only for logging the name).
 * @param jsonDefinitionPath The file path to the JSON file containing the automaton's definition.
 * @return std::string A string containing the generated C++ source code.
 * @throws CodeGenerator::GenerationError If reading the JSON file, parsing the JSON, loading the template,
 *         or rendering the template fails.
 */
std::string CodeGenerator::generate(const Machine& machine, const std::string& jsonDefinitionPath) {
    std::cout << "[CodeGen] Starting code generation for machine: " << machine.getName() << std::endl;
    std::cout << "[CodeGen] Using template: " << templateFilePath << std::endl;


    json machine_data;
    const std::string jsonFilePath = jsonDefinitionPath;

    try {
        std::ifstream inFile(jsonFilePath);
        if (!inFile.is_open()) {
            throw GenerationError("Could not open JSON file for reading: " + jsonFilePath);
        }

        // Parse the JSON directly from the input stream into the json object
        inFile >> machine_data;
        inFile.close();

        std::cout << "[CodeGen] Successfully loaded and parsed JSON from: " << jsonFilePath << std::endl;

    } catch (const json::parse_error& e) {
        std::cerr << "[CodeGen] Error parsing JSON file '" << jsonFilePath << "': " << e.what() << std::endl;
        throw GenerationError(std::string("JSON parsing failed: ") + e.what());
    } catch (const std::exception& e) {
        std::cerr << "[CodeGen] Error reading JSON file '" << jsonFilePath << "': " << e.what() << std::endl;
        throw GenerationError(std::string("JSON file reading failed: ") + e.what());
    }

    //Load Template and Render using Inja
    try {
        inja::Environment env; // Create an Inja environment
        // Load and parse the template file
        inja::Template code_template = env.parse_template(templateFilePath);

        // Render the template using the parsed JSON data
        std::cout << "[CodeGen] Rendering template..." << std::endl;
        std::string generated_code = env.render(code_template, machine_data);
        std::cout << "[CodeGen] Template rendered successfully." << std::endl;

        return generated_code; // Return the generated C++ code as a string

    } catch (const std::exception& e) {
        std::cerr << "[CodeGen] Error processing template '" << templateFilePath << "': " << e.what() << std::endl;
        throw GenerationError(std::string("Template processing failed: ") + e.what());
    }
}