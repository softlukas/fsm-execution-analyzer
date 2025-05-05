#ifndef STATE_H // Include guard - prevents multiple inclusions
#define STATE_H

#include <string> // We'll need this for state name and action
#include <vector> // Maybe later for a list of transitions originating from this state? (TBD based on design)
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QVariant>
#include <map>
#include <tuple>

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

    void updateTransitionPositions(QGraphicsScene* scene);


    void addIncomingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QVariant& data1, const QVariant& data2) {
        incomingTranstions.insert({transitionGroup->data(0).toInt(), std::make_tuple(transitionGroup, data1, data2)});
    }

    void addOutgoingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QVariant& data1, const QVariant& data2) {
        outgoingTransitions.insert({transitionGroup->data(0).toInt(), std::make_tuple(transitionGroup, data1, data2)});
    }


    // Potentially other methods in the future...
    // E.g., for working with transitions, if you decide the state should know its transitions

private:
    std::string stateName;  // State name
    std::string stateOutput; // Action assigned to the state (as a code string)
    const int stateId;
    bool isActive = false;
    //QGraphicsItemGroup* stateGroup = nullptr;
    std::map<int, std::tuple<QGraphicsItemGroup*, QVariant, QVariant>> incomingTranstions;
    std::map<int, std::tuple<QGraphicsItemGroup*, QVariant, QVariant>> outgoingTransitions;

    //QGraphicsItemGroup * stateGroup = nullptr; // Pointer to the QGraphicsItemGroup representing the state in the scene
    // Potentially other member variables...
};

#endif // STATE_H