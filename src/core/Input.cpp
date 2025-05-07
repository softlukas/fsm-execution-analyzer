/**
 * @file Input.cpp
 * @brief Implements the Input class, representing an input mechanism for the system.
 * @details This source file provides the implementation of the Input class, including methods for managing and updating input values.
 * It also includes functionality for retrieving the last known input value.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */

#include "Input.h" // Include the corresponding header file



// --- Getter Implementation ---


std::optional<std::string> Input::getLastValue() const {
    return lastKnownValue;
}

// --- Setter Implementation ---


void Input::updateValue(const std::string& value) {
    lastKnownValue = value; // Assign the new value to the optional member
    
}
 