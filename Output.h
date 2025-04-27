#ifndef OUTPUT_H // Include guard start
#define OUTPUT_H

#include "MachineElement.h" // Include the base class definition
#include <string>           // For std::string
#include <optional>         // For std::optional to store the last sent value

/**
 * @brief Represents an output channel of the automaton, inheriting from MachineElement.
 * @details Stores the output name (via base class) and the last value sent through it.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-27 // Date of creation
 */
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

    // Note: getName() is inherited from MachineElement.
    // Note: Virtual destructor is inherited.

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
