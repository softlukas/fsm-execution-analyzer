/**
 * @file Variable.cpp
 * @brief Implementation of the Variable class, derived from MachineElement.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-27 // Date of creation
 */

 #include "Variable.h" // Include the corresponding header file

 // Note: The constructor is fully defined inline in the header file (Variable.h)
 // using an initializer list, so it doesn't need a separate definition here unless
 // it had more complex logic in its body.
 
 // --- Getter Implementations ---
 
 /**
  * @brief Gets the current value of the variable as a string.
  */
 const std::string& Variable::getValueAsString() const {
     // If using std::any later, you would convert the stored 'typedValue' back to a string here.
     // For now, just return the string member.
     return currentValue;
 }
 
 /**
  * @brief Gets the type hint string for the variable.
  */
 const std::string& Variable::getTypeHint() const {
     return typeHint;
 }
 
 // --- Setter Implementation ---
 
 /**
  * @brief Sets the value of the variable.
  * @param newValue The new value as a string.
  */
 void Variable::setValue(const std::string& newValue) {
     // If using std::any later, you would perform type conversion here
     // based on 'typeHint' before storing into 'typedValue'.
     // For now, just update the string member.
     currentValue = newValue;
     // Optionally, emit a signal here if other parts of the application
     // need to be notified when a variable's value changes.
 }
 
 // No other methods were declared in the header, so no further implementations needed here for now.
 