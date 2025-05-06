#ifndef STATE_H // Include guard - prevents multiple inclusions
#define STATE_H

#include <string> // We'll need this for state name and action
#include <vector> // Maybe later for a list of transitions originating from this state? (TBD based on design)
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QDebug>
#include <QVariant>
#include <map>
#include <tuple>
//#include "Machine.h"

class Machine; 

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

    void updateTransitionPositions(QGraphicsScene* scene, QGraphicsItemGroup* stateGroup, Machine *machine);


    void addIncomingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
        if (incomingTranstions.find(transitionGroup->data(1).toInt()) != incomingTranstions.end()) {
            qDebug() << "Incoming transition with ID" << transitionGroup->data(0).toInt() << "already exists. No new record created.";
            return;
        }
        incomingTranstions.insert({transitionGroup->data(1).toInt(), std::make_tuple(transitionGroup, data1, data2)});
    }

    void addOutgoingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
        if (outgoingTransitions.find(transitionGroup->data(1).toInt()) != outgoingTransitions.end()) {
            qDebug() << "Outgoing transition with ID" << transitionGroup->data(0).toInt() << "already exists. No new record created.";
            return;
        }
        outgoingTransitions.insert({transitionGroup->data(1).toInt(), std::make_tuple(transitionGroup, data1, data2)});
    }

    void setIncomingTransitionGroup(int transitionId, QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
        //if (incomingTranstions.find(transitionId) != incomingTranstions.end()) {
            incomingTranstions[transitionId] = std::make_tuple(transitionGroup, data1, data2);
        //}
    }

    void setOutgoingTransitionGroup(int transitionId, QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
        //if (outgoingTransitions.find(transitionId) != outgoingTransitions.end()) {
            outgoingTransitions[transitionId] = std::make_tuple(transitionGroup, data1, data2);
        //}
    }

    void printTransitions() const;
    
    static QPointF getVisualCenterOfStateItem_static(QGraphicsItemGroup* stateItemGroup);

    QPointF currentPos; 

    // Potentially other methods in the future...
    // E.g., for working with transitions, if you decide the state should know its transitions

private:

    // Metóda na aktualizáciu grafiky prichádzajúceho prechodu (volá sa na CIEĽOVOM stave)
    void updateIncomingTransitionGraphic(int transitionId, QGraphicsItemGroup* newGroup, const QPointF& actualStart, const QPointF& actualEnd);

    // Metóda na aktualizáciu grafiky odchádzajúceho prechodu (volá sa na ZDROJOVOM stave)
    void updateOutgoingTransitionGraphic(int transitionId, QGraphicsItemGroup* newGroup, const QPointF& actualStart, const QPointF& actualEnd);

    // Metóda na odstránenie grafiky prichádzajúceho prechodu (volá sa na CIEĽOVOM stave)
    void removeIncomingTransitionGraphic(int transitionId);

    // Metóda na odstránenie grafiky odchádzajúceho prechodu (volá sa na ZDROJOVOM stave)
    void removeOutgoingTransitionGraphic(int transitionId);


    std::string stateName;  // State name
    std::string stateOutput; // Action assigned to the state (as a code string)
    const int stateId;
    bool isActive = false;
    //QGraphicsItemGroup* stateGroup = nullptr;
    std::map<int, std::tuple<QGraphicsItemGroup*, QPointF, QPointF>> incomingTranstions;
    std::map<int, std::tuple<QGraphicsItemGroup*, QPointF, QPointF>> outgoingTransitions;

    // Current position of the state in the scene
    //QGraphicsItemGroup * stateGroup = nullptr; // Pointer to the QGraphicsItemGroup representing the state in the scene
    // Potentially other member variables...
};

#endif // STATE_H