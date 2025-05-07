/**
 * @file Transition.h
 * @brief Declaration of the Transition class for managing state transitions within the automaton.
 * @details This file contains the declaration of the Transition class, which represents a transition between states in the interpreted finite automaton. It includes methods to handle the source and target states, transition conditions, delays, and other related properties.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */



#ifndef TRANSITION_H // Include guard
#define TRANSITION_H

#include <string>
#include <optional> 
#include "State.h" // Include the State class header
#include <iostream>



class Transition {
public:
    /**
     * @brief Constructs a Transition object.
     * 
     * @param sourceState Pointer to the source state of the transition.
     * @param targetState Pointer to the target state of the transition.
     * @param transitionId Unique identifier for the transition.
     * @param condition Optional guard condition for the transition (default is an empty string).
     * @param delayMs Optional delay in milliseconds before the transition occurs (default is 0).
     */
    Transition(State* sourceState, State* targetState, const int transitionId, const std::string& condition = "", const int delayMs = 0);

    // --- Getters ---
    /**
     * @brief Gets the name of the start state of the transition.
     * 
     * @return std::string The name of the start state.
     */
    const std::string& getSourceName() const;
    /**
     * @brief Gets the name of the target state of the transition.
     * 
     * @return std::string The name of the target state.
     */
    const std::string& getTargetName() const;
    /**
     * @brief Gets the name of the trigger event for the transition.
     * 
     * @return std::string The name of the trigger event.
     */
    const std::string& getTriggerEventName() const;
    /**
     * @brief Gets the guard condition for the transition.
     * 
     * @return std::string The guard condition.
     */
    const std::string& getGuardCondition() const;
    /**
     * @brief Gets the source state of the transition.
     * 
     * @return State* Pointer to the source state.
     */
    State* getSourceState() const;
    /**
     * @brief Gets the target state of the transition.
     * 
     * @return State* Pointer to the target state.
     */
    State* getTargetState() const;
    /**
     * @brief Gets the delay in milliseconds for the transition.
     * 
     * @return int The delay in milliseconds.
     */
    const int getDelayMs() const;
    /**
     * @brief Gets the condition for the transition.
     * 
     * @return std::string The condition.
     */
    const std::string& getCondition() const;
    /**
     * @brief Gets the ID of the transition.
     * 
     * @return int The ID of the transition.
     */
    const int getTransitionId() const;
    /**
     * @brief Gets the transition based on its ID.
     * 
     * @return const Transition& Reference to the transition.
     */
    const Transition& getTransitionBasedOnId() const;
    
    /**
     * @brief Gets the display transition string.
     * 
     * @return const std::string& The display transition string.
     */
    const std::string& DisplayTransition() const;

    /**
     * @brief Gets the ID of the destination state.
     * @return const int The ID of the destination state.
     */
    int getDestinationStateId() const; // <-- PRIDAJ TENTO RIADOK

    // Odporúčam pridať aj túto pre symetriu:
    /**
     * @brief Gets the ID of the source state.
     * @return const int The ID of the source state.
     */
    int getSourceStateId() const;      // <-- AJ TENTO RIADOK

    // --- Setters ---
    /**
     * @brief Sets the source state of the transition.
     * 
     * @param sourceState Pointer to the new source state.
     */
    void setCondition(const std::string& condition);
    /**
     * @brief Sets the target state of the transition.
     * 
     * @param targetState Pointer to the new target state.
     */
    void setDelay(int delayMs);

private:
    /**
     * @brief The source state object of the transition.
     */
    State *sourceState;
    /**
     * @brief The target state object of the transition.
     */             
    State *targetState;
    /**
     * @brief The trigger of the transition.
     */              
    std::string triggerEvent;
    /**
     * @brief The guard condition string for the transition.
     */
    std::string guardCondition;     
    /**
     * @brief The condition string for the transition.
     */
    std::string condition;
     /**
     * @brief The delay in milliseconds for the transition.
     */
    int         delayMs;
    /**
     * @brief The unique identifier for the transition.
     */            
    const int transitionId;
    
};

#endif // TRANSITION_H
