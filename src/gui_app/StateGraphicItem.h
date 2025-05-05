#ifndef STATEGRAPHICITEMDATA_H // Renamed guard to reflect it's data only
#define STATEGRAPHICITEMDATA_H

#include <QPointF> // For storing coordinates
#include <QList>   // For storing lists of points
#include <QString> // For state name
#include <QGraphicsItemGroup> // For grouped arrow objects

// Forward declaration for ArrowItemData (if you create a similar data class for arrows)
// class ArrowItemData;

/**
 * @brief Data container holding information about a state's visual representation.
 *
 * NOTE: This class DOES NOT inherit from QGraphicsItem. It only stores data.
 * Separate QGraphicsItem objects (like QGraphicsItemGroup) must be created
 * and managed in the QGraphicsScene based on the data in this object.
 *
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date    2025-05-04 // Date of creation
 */
class StateGraphicItem { // Renamed class to emphasize it holds data

public:
    /**
     * @brief Constructor.
     * @param name The name of the state.
     * @param initialPos The initial center position for the state's visual.
     */
    StateGraphicItem(const int stateId, const QPointF& initialPos)
        : stateId(stateId),
          currentCenterPos(initialPos)
    {}

    // --- Getters ---

    

    /**
     * @brief Gets the current center position of the state's visual representation.
     * @return const QPointF& The current center position.
     */
    const QPointF& getCurrentCenterPos() const {
        return currentCenterPos;
    }

    


    // --- Setters / Modifiers ---

    /**
     * @brief Sets the current center position.
     * @param pos The new center position.
     */
    void setCurrentCenterPos(const QPointF& pos) {
        currentCenterPos = pos;
        // NOTE: This only updates the data. It does NOT move anything in the scene.
        // External code needs to react to this change and update the actual QGraphicsItem.
    }

   

    void updateTransitions(QGraphicsScene* scene);
   



    void addTransitionGroup(QGraphicsItemGroup* group) {
        transitionsGroupList.append(group);
    }

    // TODO: Add methods to remove/update arrow positions when transitions are deleted or moved.


private:


    QPointF getArrowStartPosition(const QGraphicsItemGroup* arrowGroup) const;

    const int stateId;
    QPointF currentCenterPos;
    // Storing absolute positions of connected arrows is generally not robust.
    // Consider storing pointers/IDs to connected ArrowItemData objects instead.
    //QList<QPointF> outgoingArrowStartPositions;
    //QList<QPointF> incomingArrowEndPositions; // Added for completeness
    QList<QGraphicsItemGroup*> arrowGroups; // List to store grouped arrow objects

    QList<QGraphicsItemGroup*> transitionsGroupList; // List to store outgoing transition groups
   

};

#endif // STATEGRAPHICITEMDATA_H // Renamed guard