#ifndef STATE_H // Include guard - prevents multiple inclusions
#define STATE_H

#include <string> // We'll need this for state name and action
#include <vector> // Maybe later for a list of transitions originating from this state? (TBD based on design)

/**
 * @brief Represents a single state of the interpreted finite automaton.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-24 // Current creation date
 */
class State {
public:
    /**
     * @brief Constructor for creating a state.
     * @param name The name (identifier) of the state.
     * @param output The output assigned to the state (as a code string).
     */
    State(const std::string& name, const std::string& output, int stateId);

    /**
     * @brief Sets the name of the state.
     * @param name The new name (identifier) of the state.
     */
    void setName(const std::string& name);

    /**
     * @brief Sets the action associated with the state.
     * @param action The new action string for the state.
     */
    void setAction(const std::string& action);

    const int getStateId() const;


    /**
     * @brief Gets the name of the state.
     * @return const std::string& Reference to the state name.
     */
    const std::string& getName() const;

    /**
     * @brief Gets the action associated with the state.
     * @return const std::string& Reference to the state action string.
     */
    const std::string& getAction() const;

    // Potentially other methods in the future...
    // E.g., for working with transitions, if you decide the state should know its transitions

private:
    std::string stateName;  // State name
    std::string stateOutput; // Action assigned to the state (as a code string)
    const int stateId;
    // Potentially other member variables...
};

#endif // STATE_H