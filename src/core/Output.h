/**
 * @file Output.h
 * @brief Defines the Output class for managing output channels in the machine.
 * @details This header file declares the Output class, which represents an output channel in the machine. It provides methods for recording and retrieving the last sent value, ensuring proper encapsulation and functionality for output management.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */



#ifndef OUTPUT_H // Include guard start
#define OUTPUT_H

#include "MachineElement.h"
#include <string>           
#include <optional>         


class Output : public MachineElement { // Public inheritance from MachineElement
public:
    /**
     * @brief Constructor for creating an output channel.
     * @param name The name of the output (passed to the base class constructor).
     */
    explicit Output(const std::string& name)
        : MachineElement(name), // Call the base class constructor to initialize the name
          lastSentValue(std::nullopt) // Initialize optional as empty
    {}

    // --- Getters specific to Output ---
    /**
     * @brief Gets the last value known to be sent on this output.
     * @return std::optional<std::string> The last value, or std::nullopt if no value has been sent yet.
     * Marked const as it doesn't change the Output object's state.
     */
    std::optional<std::string> getLastSentValue() const;

    // --- Setter specific to Output ---
    /**
     * @brief Records the value that was just sent through this output channel.
     * @details This would typically be called by the machine's action execution logic
     * when an 'output(...)' action is performed.
     * @param value The value that was sent (as a string).
     */
    void recordSentValue(const std::string& value);

    // Disable copy/move operations
    Output(const Output&) = delete;
    Output& operator=(const Output&) = delete;
    Output(Output&&) = delete;
    Output& operator=(Output&&) = delete;

private:
    // elementName is inherited from MachineElement

    /**
     * @brief Stores the last value known to have been sent through this output channel.
     */
    std::optional<std::string> lastSentValue;
};

#endif // OUTPUT_H // Include guard end
