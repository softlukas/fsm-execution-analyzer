/**
 * @file Transition.h
 * @brief Declaration of the Transition class for managing state transitions within the automaton.
 * @details This file contains the declaration of the Transition class, which represents a transition between states in the interpreted finite automaton. It includes methods to handle the source and target states, transition conditions, delays, and other related properties.
 * @authors Your Authors (xname01, xname02, xname03)
 * @date Last modified: 2025-05-05
 */

#include "Transition.h" // Make sure this line is exactly like this
#include "State.h" // Include the State class header
#include <sstream> // Include for std::ostringstream


Transition::Transition(State* sourceState, State* targetState, const int transitionId, 
                       const std::string& condition, const int delayMs)
    : sourceState(sourceState), 
      targetState(targetState), 
      transitionId(transitionId),
      condition(condition),
        delayMs(delayMs)
        
      {

      }


State* Transition::getSourceState() const {
    return sourceState;
}

State* Transition::getTargetState() const {
    return targetState;
}
 
const std::string& Transition::getTriggerEventName() const {
    return triggerEvent;
}
 
const std::string& Transition::getGuardCondition() const {
    return guardCondition;
}
 
const int Transition::getDelayMs() const {
    return delayMs;
}

const std::string& Transition::getCondition() const {
    return condition;
}

const int Transition::getTransitionId() const {
    return transitionId;
}

const Transition& Transition::getTransitionBasedOnId() const {
    return *this;
}

const std::string& Transition::DisplayTransition() const {
    static std::string result;
    std::ostringstream oss;
    
    oss << "Condition: " << condition << "\n";
    oss << "Delay: " << delayMs << " ms\n";
    result = oss.str();
    return result;
}


void Transition::setCondition(const std::string& condition) {
    this->condition = condition;
}
void Transition::setDelay(int delayMs) {
    this->delayMs = delayMs;
}



int Transition::getSourceStateId() const {
   
    return sourceState->getStateId();
}


int Transition::getDestinationStateId() const {
    
    return targetState->getStateId();
}

 
 
 