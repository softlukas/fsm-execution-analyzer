/**
 * @file Input.h
 * @brief Declares the Input class, representing an input channel in a machine.
 * @details This header file provides the declaration of the Input class, which inherits from MachineElement.
 * It includes methods for managing the last known value of the input and ensures proper encapsulation.
 * The class is designed to be used as part of a larger machine framework.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */



#ifndef INPUT_H // Include guard start
#define INPUT_H

#include "MachineElement.h" // Include the base class definition
#include <string>           // For std::string
#include <optional>         // For std::optional to store the last value


class Input : public MachineElement { // Public inheritance from MachineElement
public:
    /**
     * @brief Constructor for creating an input channel.
     * @param name The name of the input (passed to the base class constructor).
     */
    explicit Input(const std::string& name)
        : MachineElement(name), // Call the base class constructor to initialize the name
          lastKnownValue(std::nullopt) // Initialize optional as empty
    {}

    // --- Getters specific to Input ---
    /**
     * @brief Gets the last known value received on this input.
     * @return std::optional<std::string> The last value, or std::nullopt if no value has arrived yet.
     * Marked const as it doesn't change the Input object's state.
     */
    std::optional<std::string> getLastValue() const;

    // --- Setter specific to Input ---
    /**
     * @brief Updates the last known value for this input.
     * @details This would typically be called when an external event arrives for this input.
     * @param value The new value received (as a string).
     */
    void updateValue(const std::string& value);

    // Disable copy/move operations
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&) = delete;
    Input& operator=(Input&&) = delete;

private:
    // elementName is inherited from MachineElement

    /**
     * @brief Stores the last known value received on this input channel.
     * std::optional is used to distinguish between no value received yet
     * and receiving an empty string.
     */
    std::optional<std::string> lastKnownValue;
};

#endif // INPUT_H // Include guard end
