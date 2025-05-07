/**
 * @file Output.cpp
 * @brief Implements the Output class for managing output channels in the machine.
 * @details This source file provides the implementation of the Output class methods, which handle recording and retrieving the last sent value for output management.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */

 #include "Output.h" 

 
 
 // --- Getter Implementation ---
 
 
 std::optional<std::string> Output::getLastSentValue() const {
     return lastSentValue;
 }
 
 // --- Setter Implementation ---
 
 
 void Output::recordSentValue(const std::string& value) {
     lastSentValue = value; // Assign the new value to the optional member
     // Optionally, emit a signal here if the GUI needs to display the last sent value.
 }
 