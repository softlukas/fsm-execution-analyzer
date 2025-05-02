/**
 * @file State.cpp
 * @brief Implementation of the State class.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-24 // Current creation date
 */

#include "State.h" // Include the header file

State::State(const std::string& name, const std::string& action, int stateId)
    : stateName(name), stateOutput(action), stateId(stateId) {
    // Constructor body can be empty for now,
    // initialization happened in the initializer list.
}

const std::string& State::getName() const {
    return stateName;
}

const std::string& State::getAction() const {
    return stateOutput;
}

void State::setName(const std::string& name) {
    stateName = name;
}

void State::setAction(const std::string& action) {
    stateOutput = action;
}

const int State::getStateId() const {
    return stateId;
}

