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
    State(const std::string& name, const std::string& output);

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
    // Potentially other member variables...
};

#endif // STATE_H