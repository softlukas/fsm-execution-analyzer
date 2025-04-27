/**
 * @file Machine.cpp
 * @brief Implementation of the Machine class (formerly Automaton).
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-26 // Date of creation/update
 */

#include "Machine.h"     // Include the corresponding header file
#include <stdexcept>     // For exceptions like std::runtime_error
#include <iostream>      // For debug/log output (optional)
#include <algorithm>     // For std::remove_if (used in timer logic)
#include <utility>       // For std::move

// --- Constructor ---

/**
 * @brief Constructs a Machine object with the given name.
 * @param name The name of the state machine.
 */
Machine::Machine(const std::string& name)
    : machineName(name), // Initialize the machine name
    running(false),
    terminationRequested(false) // Initialize interpreter state flags
{
    // Constructor body can be empty if initialization is done in the list
    std::cout << "Machine '" << machineName << "' created." << std::endl;
}

// --- Methods for building the automaton ---

/**
 * @brief Adds a state to the machine.
 * @param state A unique_ptr to the State object to add. Ownership is transferred.
 * @throws std::runtime_error if a state with the same name already exists.
 */
void Machine::addState(std::unique_ptr<State> state) {
    if (!state) return; // Do nothing if null pointer is passed
    auto name = state->getName();
    if (states.count(name)) {
        throw std::runtime_error("State with name '" + name + "' already exists.");
    }
    states[name] = std::move(state); // Move ownership into the map
}

/**
 * @brief Adds a transition to the machine.
 * @param transition A unique_ptr to the Transition object to add. Ownership is transferred.
 */
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

/**
 * @brief Adds a variable to the machine.
 * @param variable A unique_ptr to the Variable object to add. Ownership is transferred.
 * @throws std::runtime_error if a variable with the same name already exists.
 */
void Machine::addVariable(std::unique_ptr<Variable> variable) {
    if (!variable) return;
    auto name = variable->getName();
    if (variables.count(name)) {
        throw std::runtime_error("Variable with name '" + name + "' already exists.");
    }
    variables[name] = std::move(variable);
}

/**
 * @brief Adds an input channel to the machine.
 * @param input A unique_ptr to the Input object to add. Ownership is transferred.
 * @throws std::runtime_error if an input with the same name already exists.
 */
void Machine::addInput(std::unique_ptr<Input> input) {
    if (!input) return;
    auto name = input->getName();
    if (inputs.count(name)) {
        throw std::runtime_error("Input with name '" + name + "' already exists.");
    }
    inputs[name] = std::move(input);
}



/**
 * @brief Adds an output channel to the machine.
 * @param output A unique_ptr to the Output object to add. Ownership is transferred.
 * @throws std::runtime_error if an output with the same name already exists.
 */
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

/**
 * @brief Gets the name of the machine.
 */
const std::string& Machine::getName() const {
    return machineName;
}

/**
 * @brief Gets a pointer to the initial state.
 * @return const State* Pointer to the initial state, or nullptr if not set/found.
 */
const State* Machine::getInitialState() const {
    if (initialStateName.empty() || !states.count(initialStateName)) {
        return nullptr;
    }
    // .at() throws if key not found, but we checked with .count()
    // .get() retrieves the raw pointer from unique_ptr
    return states.at(initialStateName).get();
}

/**
 * @brief Gets a pointer to a state by its name.
 * @param name The name of the state to find.
 * @return const State* Pointer to the state, or nullptr if not found.
 */
State* Machine::getState(const std::string& name) const {
    auto it = states.find(name);
    if (it != states.end()) {
        return it->second.get(); // Return raw pointer from unique_ptr
    }
    return nullptr; // State not found
}

/**
 * @brief Gets a pointer to a variable by its name.
 * @param name The name of the variable to find.
 * @return const Variable* Pointer to the variable, or nullptr if not found.
 */
const Variable* Machine::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Gets a pointer to an input channel by its name.
 * @param name The name of the input to find.
 * @return const Input* Pointer to the input, or nullptr if not found.
 */
const Input* Machine::getInput(const std::string& name) const {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Gets a pointer to an output channel by its name.
 * @param name The name of the output to find.
 * @return const Output* Pointer to the output, or nullptr if not found.
 */
const Output* Machine::getOutput(const std::string& name) const {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief Gets a constant reference to the map of all states.
 */
const std::map<std::string, std::unique_ptr<State>>& Machine::getStates() const {
    return states;
}

/**
 * @brief Gets a constant reference to the map of all variables.
 */
const std::map<std::string, std::unique_ptr<Variable>>& Machine::getVariables() const {
    return variables;
}

/**
 * @brief Gets a constant reference to the map of all inputs.
 */
const std::map<std::string, std::unique_ptr<Input>>& Machine::getInputs() const {
    return inputs;
}

/**
 * @brief Gets a constant reference to the map of all outputs.
 */
const std::map<std::string, std::unique_ptr<Output>>& Machine::getOutputs() const {
    return outputs;
}

/**
 * @brief Gets a constant reference to the vector of all transitions.
 */
const std::vector<std::unique_ptr<Transition>>& Machine::getTransitions() const {
    return transitions;
}

// --- Methods for interacting during runtime ---

/**
 * @brief Sets the value of an existing variable.
 * @param name The name of the variable to set.
 * @param value The new value (as a string).
 * @return bool True if the variable was found and set, false otherwise.
 */
bool Machine::setVariableValue(const std::string& name, const std::string& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second->setValue(value); // Call setValue on the Variable object
        return true;
    }
    return false; // Variable not found
}

/**
 * @brief Updates the last known value of an input channel.
 * @param name The name of the input channel.
 * @param value The new value received.
 * @return bool True if the input was found and updated, false otherwise.
 */
bool Machine::updateInputValue(const std::string& name, const std::string& value) {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        it->second->updateValue(value);
        return true;
    }
    return false; // Input not found
}

/**
 * @brief Gets the last known value of a specific input channel.
 * @param name The name of the input channel.
 * @return std::optional<std::string> The last value, or std::nullopt if not found or never set.
 */
std::optional<std::string> Machine::getLastInputValue(const std::string& name) const {
    auto it = inputs.find(name);
    if (it != inputs.end()) {
        return it->second->getLastValue();
    }
    return std::nullopt; // Input not found
}

/**
 * @brief Records that a value was sent to an output channel.
 * Also performs the actual output action (e.g., logging, network send).
 * @param name The name of the output channel.
 * @param value The value that was sent.
 * @return bool True if the output channel was found, false otherwise.
 */
bool Machine::recordOutputValue(const std::string& name, const std::string& value) {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        it->second->recordSentValue(value);
        // --- TODO: Implement actual output mechanism here ---
        // This could involve:
        // - Logging the output event
        // - Sending data over a socket (UDP as suggested)
        // - Emitting a Qt signal for the GUI to display
        std::cout << "Output [" << name << "] -> " << value << std::endl; // Basic logging for now
        // --- End of TODO ---
        return true;
    }
    return false; // Output not found
}

/**
 * @brief Gets the last recorded value sent to a specific output channel.
 * @param name The name of the output channel.
 * @return std::optional<std::string> The last sent value, or std::nullopt if not found or never sent.
 */
std::optional<std::string> Machine::getLastOutputValue(const std::string& name) const {
    auto it = outputs.find(name);
    if (it != outputs.end()) {
        return it->second->getLastSentValue();
    }
    return std::nullopt; // Output not found
}



