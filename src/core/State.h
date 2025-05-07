/**
 * @file State.h
 * @brief Declaration of the State class for managing states within the automaton.
 * @details This file contains the declaration of the State class, which represents a single state in the interpreted finite automaton. It includes methods to handle state properties, transitions, and graphical representations.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */



#ifndef STATE_H // Include guard - prevents multiple inclusions
#define STATE_H

#include <string> 
#include <vector> 
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QDebug>
#include <QVariant>
#include <map>
#include <tuple>


class Machine; 


class State {
public:
    /**
     * @brief Constructor for creating a state.
     * @param name The name (identifier) of the state.
     * @param output The output assigned to the state (as a code string).
     * @param stateId The unique identifier for the state.
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
    
    /**
     * @brief Retrieves the unique identifier of the state.
     * 
     * @return The unique identifier of the state as an integer.
     */
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



    /**
     * @brief Updates the positions of transitions in the graphical scene.
     * 
     * This function adjusts the positions of transitions associated with a state
     * in the given QGraphicsScene. It ensures that the transitions are properly
     * aligned and visually connected to the state represented by the QGraphicsItemGroup.
     * 
     * @param scene Pointer to the QGraphicsScene where the transitions and state are rendered.
     * @param stateGroup Pointer to the QGraphicsItemGroup representing the state in the scene.
     * @param machine Pointer to the Machine object containing the state and its transitions.
     */
    void updateTransitionPositions(QGraphicsScene* scene, QGraphicsItemGroup* stateGroup, Machine *machine);


    /**
     * @brief Adds an incoming transition group to the state.
     * 
     * This function associates a group of graphical items representing transitions
     * with the state. It also takes two points as additional data, which may be used
     * for positioning or other purposes related to the transition group.
     * 
     * @param transitionGroup A pointer to the QGraphicsItemGroup representing the transition group.
     * @param data1 The first QPointF containing additional data for the transition group.
     * @param data2 The second QPointF containing additional data for the transition group.
     */
    void addIncomingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2);

    /**
     * @brief Adds an outgoing transition group to the state.
     * 
     * This function associates a group of graphical items representing transitions
     * with the state. It also takes two points as additional data, which can be used
     * to define the geometry or other properties of the transition group.
     * 
     * @param transitionGroup A pointer to the QGraphicsItemGroup representing the transition group.
     * @param data1 The first QPointF containing additional data for the transition group.
     * @param data2 The second QPointF containing additional data for the transition group.
     */
    void addOutgoingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2);

    /**
     * @brief Sets the incoming transition group for the state.
     * 
     * This function associates a transition group with the state, using the specified transition ID
     * and additional positional data points. The transition group is typically used to visually 
     * represent the transition in a graphical interface.
     * 
     * @param transitionId The unique identifier of the transition.
     * @param transitionGroup A pointer to the QGraphicsItemGroup representing the transition group.
     * @param data1 The first QPointF containing positional data related to the transition.
     * @param data2 The second QPointF containing additional positional data related to the transition.
     */
    void setIncomingTransitionGroup(int transitionId, QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2);
    

    /**
     * @brief Sets the outgoing transition group for a state.
     * 
     * This function associates a transition group with a specific transition ID
     * and defines two points of data related to the transition.
     * 
     * @param transitionId The unique identifier for the transition.
     * @param transitionGroup A pointer to the QGraphicsItemGroup representing the transition group.
     * @param data1 The first QPointF data associated with the transition.
     * @param data2 The second QPointF data associated with the transition.
     */
    void setOutgoingTransitionGroup(int transitionId, QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2); 
    

    /**
     * @brief Prints all the transitions associated with the current state.
     * 
     * This function outputs the details of all transitions for the current state
     * to the standard output or a designated logging mechanism. It is intended
     * for debugging or informational purposes.
     */
    void printTransitions() const;
    
    /**
     * @brief Calculates the visual center of a state item group.
     * 
     * This static function determines the visual center point of a given 
     * QGraphicsItemGroup representing a state. The visual center is computed 
     * based on the bounding rectangle of the group.
     * 
     * @param stateItemGroup A pointer to the QGraphicsItemGroup representing the state.
     * @return QPointF The visual center of the provided state item group.
     */
    static QPointF getVisualCenterOfStateItem_static(QGraphicsItemGroup* stateItemGroup);

    /**
     * @brief Represents the current position as a point in 2D space.
     * 
     * This variable stores the current position using the QPointF class,
     * which provides a way to handle 2D points with floating-point precision.
     */
    QPointF currentPos; 

private:

    
    /**
     * @brief Updates the graphical representation of an incoming transition.
     * 
     * This function modifies the graphical item group associated with a specific 
     * transition to reflect its updated state. It also adjusts the start and end 
     * points of the transition to match the provided coordinates.
     * 
     * @param transitionId The unique identifier of the transition to be updated.
     * @param newGroup A pointer to the new QGraphicsItemGroup representing the updated transition.
     * @param actualStart The actual starting point of the transition in the scene.
     * @param actualEnd The actual ending point of the transition in the scene.
     */
    void updateIncomingTransitionGraphic(int transitionId, QGraphicsItemGroup* newGroup, const QPointF& actualStart, const QPointF& actualEnd);

    
    /**
     * @brief Updates the graphical representation of an outgoing transition.
     * 
     * This function modifies the visual appearance of a specific outgoing transition
     * by replacing its associated graphics item group with a new one and updating
     * its start and end points.
     * 
     * @param transitionId The unique identifier of the transition to be updated.
     * @param newGroup A pointer to the new QGraphicsItemGroup representing the updated transition.
     * @param actualStart The actual starting point of the transition in the scene.
     * @param actualEnd The actual ending point of the transition in the scene.
     */
    void updateOutgoingTransitionGraphic(int transitionId, QGraphicsItemGroup* newGroup, const QPointF& actualStart, const QPointF& actualEnd);

    
    /**
     * @brief Removes the graphical representation of an incoming transition.
     * 
     * This function removes the visual element associated with the incoming 
     * transition specified by its unique identifier. It ensures that the 
     * transition is no longer displayed in the graphical interface.
     * 
     * @param transitionId The unique identifier of the incoming transition to be removed.
     */
    void removeIncomingTransitionGraphic(int transitionId);

    
    /**
     * @brief Removes the graphical representation of an outgoing transition.
     * 
     * This function removes the visual element associated with the outgoing 
     * transition identified by the given transition ID. It ensures that the 
     * transition is no longer displayed in the graphical interface.
     * 
     * @param transitionId The unique identifier of the outgoing transition to be removed.
     */
    void removeOutgoingTransitionGraphic(int transitionId);


    /**
     * @brief Represents the name of the state.
     * 
     * This string holds the name or identifier of a specific state
     * within the application. It is used to distinguish between
     * different states in the system.
     */
    std::string stateName; 
    /**
     * @brief Represents the output state as a string.
     */
    std::string stateOutput;

    /**
     * @brief Unique identifier for the state.
     * 
     * This constant integer represents the unique ID of a state.
     * It is immutable and assigned during the initialization of the state.
     */
    const int stateId;

    /**
     * @brief Indicates whether the state is currently active.
     * 
     * This boolean flag is used to determine if the state is active.
     * When set to true, the state is considered active; otherwise, it is inactive.
     */
    bool isActive = false;
    
    /**
     * @brief A map that associates an integer key with a tuple containing information about incoming transitions.
     * 
     * The map stores the following data for each key:
     * - A pointer to a QGraphicsItemGroup representing the graphical group associated with the transition.
     * - A QPointF representing the starting point of the transition.
     * - A QPointF representing the ending point of the transition.
     * 
     * @details
     * This structure is used to manage and organize incoming transitions in a graphical representation.
     * Each entry in the map corresponds to a specific transition, identified by an integer key.
     */
    std::map<int, std::tuple<QGraphicsItemGroup*, QPointF, QPointF>> incomingTranstions;

    /**
     * @brief A map that stores outgoing transitions for a state.
     * 
     * The key is an integer representing the transition ID.
     * The value is a tuple containing:
     * - A pointer to a QGraphicsItemGroup* representing the graphical representation of the transition.
     * - A QPointF representing the starting point of the transition.
     * - A QPointF representing the ending point of the transition.
     */
    std::map<int, std::tuple<QGraphicsItemGroup*, QPointF, QPointF>> outgoingTransitions;

    
};

#endif // STATE_H