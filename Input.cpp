/**
 * @file Input.cpp
 * @brief Implementation of the Input class, derived from MachineElement.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-27 // Date of creation
 */

#include "Input.h" // Include the corresponding header file

// Note: The constructor is fully defined inline in the header file (Input.h)
// using an initializer list, so it doesn't need a separate definition here.

// --- Getter Implementation ---

/**
 * @brief Gets the last known value received on this input.
 * @return std::optional<std::string> The last value, or std::nullopt if no value has arrived yet.
 */
std::optional<std::string> Input::getLastValue() const {
    return lastKnownValue;
}

// --- Setter Implementation ---

/**
 * @brief Updates the last known value for this input.
 * @details This would typically be called when an external event arrives for this input.
 * @param value The new value received (as a string).
 */
void Input::updateValue(const std::string& value) {
    lastKnownValue = value; // Assign the new value to the optional member
    // Optionally, emit a signal here if the GUI needs to be updated
    // when an input value changes.
}
 