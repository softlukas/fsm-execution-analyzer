/**
 * @file Machine.cpp
 * @brief Implements the Machine class, representing a state machine with states, transitions, variables, inputs, and outputs.
 * @details This source file provides the implementation of the Machine class, including methods for building, managing, and executing the logic of a state machine.
 * It also includes functionality for runtime interaction, such as processing inputs, updating variables, and executing transitions.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */

#include "Machine.h"     // Include the corresponding header file
#include <stdexcept>     // For exceptions like std::runtime_error
#include <iostream>      // For debug/log output (optional)
#include <algorithm>     // For std::remove_if (used in timer logic)
#include <utility>       // For std::move


Machine::Machine(const std::string& name)
    : machineName(name), // Initialize the machine name
    running(false),
    terminationRequested(false) // Initialize interpreter state flags
    {
        // Constructor body can be empty if initialization is done in the list
        std::cout << "Machine '" << machineName << "' created." << std::endl;
    }


void Machine::addState(std::unique_ptr<State> state) {
    if (!state) return; // Do nothing if null pointer is passed
    auto name = state->getName();
    if (states.count(name)) {
        throw std::runtime_error("State with name '" + name + "' already exists.");
    }
    states[name] = std::move(state); // Move ownership into the map
}


void Machine::addTransition(std::unique_ptr<Transition> transition) {
    if (!transition) return;
    // Optional: Add checks here if source/target states exist in the 'states' map
    transitions.push_back(std::move(transition)); // Move ownership into the vector
}

Transition* Machine::getTransition(const int transitionId) const {
    for (const auto& transition : transitions) {
        if (transition->getTransitionId() == transitionId) {
            return transition.get(); // Return raw pointer from unique_ptr
        }
    }
    return nullptr; // Transition not found
}


void Machine::addVariable(std::unique_ptr<Variable> variable) {
    if (!variable) return;
    auto name = variable->getName();
    if (variables.count(name)) {
        throw std::runtime_error("Variable with name '" + name + "' already exists.");
    }
    variables[name] = std::move(variable);
}


void Machine::addInput(std::unique_ptr<Input> input) {
    if (!input) return;
    auto name = input->getName();
    if (inputs.count(name)) {
        throw std::runtime_error("Input with name '" + name + "' already exists.");
    }
    inputs[name] = std::move(input);
}




void Machine::addOutput(std::unique_ptr<Output> output) {
    if (!output) return;
    auto name = output->getName();
    if (outputs.count(name)) {
        throw std::runtime_error("Output with name '" + name + "' already exists.");
    }
    outputs[name] = std::move(output);
}

/**
 * @brief Sets the name of the initial state for the machine.
 * @param stateName The name of the state to be set as initial.
 * @throws std::runtime_error if no state with the given name exists.
 */
void Machine::setInitialState(const std::string& stateName) {
    if (!states.count(stateName)) {
        throw std::runtime_error("Cannot set initial state: State '" + stateName + "' not found.");
    }
    initialStateName = stateName;
}

// --- Getters for definition ---


const std::string& Machine::getName() const {
    return machineName;
}

void Machine::setName(const std::string& newName) {
    if (newName.empty()) {
        
        std::cerr << "Warning: Attempted to set empty machine name." << std::endl;
        return;
    }
   
    this->machineName = newName;
    std::cout << "Machine renamed to '" << this->machineName << "'" << std::endl;
}


const State* Machine::getInitialState() const {
    if (initialStateName.empty() || !states.count(initialStateName)) {
        return nullptr;
    }
    // .at() throws if key not found, but we checked with .count()
    // .get() retrieves the raw pointer from unique_ptr
    return states.at(initialStateName).get();
}


State* Machine::getState(const int stateId) const {
    
    for (const auto& pair : states) {
       
        if (pair.second->getStateId() == stateId) {
            
            return pair.second.get(); 
            
            
        }
    }
    return nullptr;
}



Variable* Machine::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second.get();
    }
    return nullptr;
}


Input* Machine::getInput(const std::string& name) const {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        return it->second.get();
    }
    return nullptr;
}


void Machine::removeInput(const std::string& name) {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        inputs.erase(it);
    }
}
void Machine::removeOutput(const std::string& name) {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        outputs.erase(it);
    }
}
void Machine::removeVariable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        variables.erase(it);
    }
}


const Output* Machine::getOutput(const std::string& name) const {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        return it->second.get();
    }
    return nullptr;
}



const std::map<std::string, std::unique_ptr<State>>& Machine::getStates() const {
    return states;
}


const std::map<std::string, std::unique_ptr<Variable>>& Machine::getVariables() const {
    return variables;
}


const std::map<std::string, std::unique_ptr<Input>>& Machine::getInputs() const {
    return inputs;
}


const std::map<std::string, std::unique_ptr<Output>>& Machine::getOutputs() const {
    return outputs;
}


const std::vector<std::unique_ptr<Transition>>& Machine::getTransitions() const {
    return transitions;
}

// --- Methods for interacting during runtime ---


bool Machine::setVariableValue(const std::string& name, const std::string& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second->setValue(value); // Call setValue on the Variable object
        return true;
    }
    return false; // Variable not found
}



bool Machine::updateInputValue(const std::string& name, const std::string& value) {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        it->second->updateValue(value);
        return true;
    }
    return false; // Input not found
}


std::optional<std::string> Machine::getLastInputValue(const std::string& name) const {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        return it->second->getLastValue();
    }
    return std::nullopt; // Input not found
}


bool Machine::recordOutputValue(const std::string& name, const std::string& value) {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        it->second->recordSentValue(value);
        
        std::cout << "Output [" << name << "] -> " << value << std::endl; 
        
        return true;
    }
    return false; // Output not found
}


std::optional<std::string> Machine::getLastOutputValue(const std::string& name) const {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        return it->second->getLastSentValue();
    }
    return std::nullopt; // Output not found
}



