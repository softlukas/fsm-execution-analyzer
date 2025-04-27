#ifndef VARIABLE_H // Include guard start
#define VARIABLE_H

#include "MachineElement.h" // Include the base class definition
#include <string>           // For std::string
#include <any>              // Optional: Consider using std::any later for typed values

/**
 * @brief Represents an internal variable within the automaton, inheriting from MachineElement.
 * @details Stores the variable's name (via base class), its current value, and a type hint.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-27 // Date of creation
 */
class Variable : public MachineElement { // Public inheritance from MachineElement
public:
    /**
     * @brief Constructor for creating a variable.
     * @param name The name of the variable (passed to the base class constructor).
     * @param initialValue The initial value of the variable as a string.
     * @param typeHint A string representing the intended C++ type (e.g., "int", "string").
     */
    Variable(const std::string& name, const std::string& initialValue, const std::string& typeHint = "")
        : MachineElement(name), // Call the base class constructor to initialize the name
          currentValue(initialValue),
          typeHint(typeHint)
    {}

    // Note: getName() is inherited from MachineElement and doesn't need to be redefined.
    // Note: The virtual destructor ~MachineElement() is also inherited.

    // --- Getters specific to Variable ---
    /**
     * @brief Gets the current value of the variable as a string.
     * @return const std::string& Constant reference to the current value string.
     */
    const std::string& getValueAsString() const;

    /**
     * @brief Gets the type hint string for the variable.
     * @return const std::string& Constant reference to the type hint string.
     */
    const std::string& getTypeHint() const;

    // --- Setter specific to Variable ---
    /**
     * @brief Sets the value of the variable.
     * @param newValue The new value as a string.
     */
    void setValue(const std::string& newValue);

    // Disable copy/move operations if appropriate for this derived class as well
    Variable(const Variable&) = delete;
    Variable& operator=(const Variable&) = delete;
    Variable(Variable&&) = delete;
    Variable& operator=(Variable&&) = delete;

private:
    // elementName is inherited from MachineElement (as protected or private)

    /**
     * @brief The current value of the variable, stored as a string initially.
     */
    std::string currentValue;

    /**
     * @brief A hint about the intended C++ data type of the variable.
     */
    std::string typeHint;

    // std::any typedValue; // Optional: Alternative storage for typed values
};

#endif // VARIABLE_H // Include guard end
