/**
 * @file MachineElement.h
 * @brief Declares the MachineElement class, a base class for elements within the state machine that have a name.
 * @details This header file defines the MachineElement class, which serves as a base for Variable, Input, and Output classes, providing a common interface for named elements.
*  @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */



#ifndef MACHINEELEMENT_H // Include guard start
#define MACHINEELEMENT_H

#include <string>


class MachineElement {
public:
    /**
     * @brief Constructor requiring a name for the element.
     * @param name The name of the machine element.
     */
    explicit MachineElement(const std::string& name)
        : elementName(name) // Initialize the name using initializer list
    {}

    /**
     * @brief Virtual destructor.
     * Important for base classes to ensure proper cleanup when derived objects
     * are deleted via base class pointers.
     */
    virtual ~MachineElement() = default; // Default destructor is sufficient here

    /**
     * @brief Gets the name of the machine element.
     * @return const std::string& A constant reference to the element's name.
     * Marked const as it does not modify the object's state.
     */
    const std::string& getName() const {
        return elementName;
    }

    // Disable copy and move operations for the base class if they are not needed
    MachineElement(const MachineElement&) = delete;
    MachineElement& operator=(const MachineElement&) = delete;
    MachineElement(MachineElement&&) = delete;
    MachineElement& operator=(MachineElement&&) = delete;


protected: // Use protected if derived classes need direct access, otherwise private
    /**
     * @brief The name of this machine element.
     */
    std::string elementName;

};

#endif // MACHINEELEMENT_H // Include guard end