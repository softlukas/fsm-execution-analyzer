/**
 * @file CodeGenerator.h
 * @brief Defines the CodeGenerator class responsible for generating C++ interpreter code.
 * @authors Your Authors (xsiaket00, xsimonl00)
 * @date 2025-05-05 // Date of last modification
 */

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <string>
#include <stdexcept>


class Machine;

/**
 * @brief Responsible for generating C++ interpreter code for a given automaton
 *        using the Inja templating engine.
 * @details Loads a template file, receives the automaton's definition via a JSON file path,
 *          and renders the final code by combining the template and the JSON data.
 */
class CodeGenerator {
public:
    /**
     * @brief Custom exception class for errors occurring during code generation.
     * @details Inherits from std::runtime_error.
     */
    class GenerationError : public std::runtime_error {
    public:
        /**
         * @brief Constructs a GenerationError exception.
         * @param msg The error message detailing the cause of the failure.
         */
        explicit GenerationError(const std::string& msg) : std::runtime_error(msg) {}
    };

    /**
     * @brief Constructs a CodeGenerator instance.
     * @param templatePath The path to the Inja template file (e.g., "templates/automaton.tpl").
     */
    explicit CodeGenerator(const std::string& templatePath);

    /**
     * @brief Generates the C++ code for the specified automaton based on its JSON definition.
     * @param machine A constant reference to the Machine object (currently used for logging its name).
     * @param jsonDefinitionPath The path to the JSON file containing the automaton's definition.
     * @return std::string The generated C++ code as a string.
     * @throws GenerationError If an error occurs during template loading, JSON file reading/parsing,
     *         or template rendering.
     * @throws nlohmann::json::exception If JSON parsing fails (propagated from the nlohmann library).
     */
    std::string generate(const Machine& machine, const std::string& jsonDefinitionPath);

    CodeGenerator(const CodeGenerator&) = delete;
    CodeGenerator& operator=(const CodeGenerator&) = delete;
    CodeGenerator(CodeGenerator&&) = delete;
    CodeGenerator& operator=(CodeGenerator&&) = delete;

private:
    /**
     * @brief Stores the file path to the Inja template.
     */
    std::string templateFilePath;

};

#endif // CODEGENERATOR_H