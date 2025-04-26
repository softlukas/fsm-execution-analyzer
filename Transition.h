#ifndef TRANSITION_H // Include guard
#define TRANSITION_H

#include <string>
#include <optional> // Or use empty strings / sentinel values for optional parts
#include "State.h" // Include the State class header
/**
 * @brief Represents a transition between states in the interpreted finite automaton.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-24 // Date of creation
 */
class Transition {
public:
    /**
     * @brief Constructor for creating a transition.
     * @param sourceName Name of the source state.
     * @param targetName Name of the target state.
     * @param eventName Name of the input event that triggers this transition (can be empty).
     * @param condition Guard condition as a C/C++ expression string (can be empty).
     * @param delayMs Delay in milliseconds before the transition occurs (-1 or 0 if immediate).
     */
    //Transition(const State* sourceState,
               //const State* targetState,
               //const std::string& eventName, // Use "" if no specific event needed
               //const std::string& condition, // Use "" if no guard condition
               //int delayMs);                 // Use e.g., 0 or -1 for no delay

    /**
     * @brief Constructor for creating a transition with only source and target states.
     * @param sourceState Pointer to the source state object.
     * @param targetState Pointer to the target state object.
     */
    Transition(State& sourceState, State& targetState, const std::string& condition = "", const int delayMs = 0);

    // --- Getters ---
    const std::string& getSourceName() const;
    const std::string& getTargetName() const;
    const std::string& getTriggerEventName() const;
    const std::string& getGuardCondition() const;
    const State& getSourceState() const;
    const State& getTargetState() const;
    const int getDelayMs() const;
    const std::string& getCondition() const;
    

    // We might add methods later to evaluate the condition, etc.

private:
    State sourceState;              // Source state object
    State targetState;    // Name of the target state
    std::string triggerEvent;       // Name of the input event required (if any)
    std::string guardCondition;     // Guard condition string (if any)
    std::string condition;
    int         delayMs;            // Delay in milliseconds (e.g., 0 or -1 for none)
    
};

#endif // TRANSITION_H
