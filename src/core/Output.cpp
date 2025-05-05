/**
 * @file Output.cpp
 * @brief Implementation of the Output class, derived from MachineElement.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-27 // Date of creation
 */

 #include "Output.h" // Include the corresponding header file

 // Note: The constructor is fully defined inline in the header file (Output.h)
 // using an initializer list, so it doesn't need a separate definition here.
 
 // --- Getter Implementation ---
 
 /**
  * @brief Gets the last value known to be sent on this output.
  * @return std::optional<std::string> The last value, or std::nullopt if no value has been sent yet.
  */
 std::optional<std::string> Output::getLastSentValue() const {
     return lastSentValue;
 }
 
 // --- Setter Implementation ---
 
 /**
  * @brief Records the value that was just sent through this output channel.
  * @details This would typically be called by the machine's action execution logic.
  * @param value The value that was sent (as a string).
  */
 void Output::recordSentValue(const std::string& value) {
     lastSentValue = value; // Assign the new value to the optional member
     // Optionally, emit a signal here if the GUI needs to display the last sent value.
 }
 