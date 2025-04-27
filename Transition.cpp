/**
 * @file Transition.cpp
 * @brief Implementation of the Transition class.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-24 // Date of creation
 */

#include "Transition.h" // Make sure this line is exactly like this
#include "State.h" // Include the State class header
#include <sstream> // Include for std::ostringstream
/*
Transition::Transition(State* sourceState,
                       State* targetState,
                       const std::string& eventName,
                       const std::string& condition,
                       int delayMs) 
    : sourceState(sourceState),
      targetState(targetState),
      triggerEvent(eventName),
      guardCondition(condition),
      delayMs(delayMs) {
    // Constructor body
}
*/

Transition::Transition(State& sourceState, State& targetState, const int transitionId, 
                       const std::string& condition, const int delayMs)
    : sourceState(sourceState), // Create a new State object
      targetState(targetState), // Create a new State object
      transitionId(transitionId),
      condition(condition),
        delayMs(delayMs)
        
      {

      }


    const State& Transition::getSourceState() const {
        return sourceState;
    }

    const State& Transition::getTargetState() const {
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
    oss << "Transition from " << sourceState.getName().c_str() << " to " << targetState.getName().c_str() << "\n";
    oss << "Condition: " << condition << "\n";
    oss << "Delay: " << delayMs << " ms\n";
    result = oss.str();
    return result;
}

 
 // Implementations of other methods (like condition evaluation) will come later...
 