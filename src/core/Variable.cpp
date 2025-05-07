/**
 * @file Variable.cpp
 * @brief Implementation of the Variable class for managing internal variables within the automaton.
 * @details This file contains the implementation of the Variable class, which extends MachineElement and provides methods to handle a variable's name, value, and type hint.
 * @authors xsiaket00, xsimonl00
 * @date Last modified: 2025-05-05
 */

#include "Variable.h" // Include the corresponding header file


// --- Getter Implementations ---


const std::string& Variable::getValueAsString() const {
    // If using std::any later, you would convert the stored 'typedValue' back to a string here.
    // For now, just return the string member.
    return currentValue;
}


const std::string& Variable::getTypeHint() const {
    return typeHint;
}

// --- Setter Implementation ---

void Variable::setValue(const std::string& newValue) {

    currentValue = newValue;

}
 