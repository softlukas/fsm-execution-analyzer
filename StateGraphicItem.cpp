/**
 * @file StateGraphicItemData.cpp
 * @brief Implementation file for the StateGraphicItemData class.
 * @details This file is mostly empty because all methods of StateGraphicItemData
 * were defined inline within the header file (StateGraphicItemData.h).
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date    2025-05-04 // Date of creation
 */

#include "StateGraphicItem.h" // Include the corresponding header file
#include "mainwindow.h" // Include the main window header for drawing arrows
#include "mainWindowUtils.h" // Include utility functions for drawing arrows

 // No method implementations are needed here as they are defined inline in the header.


void StateGraphicItem::updateTransitions(QGraphicsScene *scene) {
    for(auto & transitionGroup : transitionsGroupList) {
       
        //MainWindow * mainWindow = MainWindow::instance();
        MainWindowUtils::drawArrow(getArrowStartPosition(transitionGroup), currentCenterPos, "Transition", 5, scene);
    }
}


/**
 * @brief Gets the starting point (tail, the end without the arrowhead)
 * of a transition arrow group in scene coordinates.
 * @param arrowGroup Pointer to the QGraphicsItemGroup representing the arrow.
 * This group is expected to contain either a QGraphicsLineItem
 * or a QGraphicsPathItem representing the arrow's main body.
 * @return QPointF The starting point in scene coordinates. Returns a default QPointF()
 * (usually 0,0) if the arrow group is null or the line/path cannot be found.
 */
QPointF StateGraphicItem::getArrowStartPosition(const QGraphicsItemGroup* arrowGroup) const
{
    // 1. Check if the provided group pointer is valid
    if (!arrowGroup) {
        //qWarning() << "getArrowStartPosition called with null arrowGroup.";
        return QPointF(); // Return default point (0,0)
    }

    // 2. Iterate through the children of the group to find the line or path item
    for (QGraphicsItem* child : arrowGroup->childItems()) {
        // Check if the child is a QGraphicsLineItem
        if (QGraphicsLineItem* lineItem = dynamic_cast<QGraphicsLineItem*>(child)) {
            // Found a line item (used for straight arrows)
            // p1() gives the start point of the line in the line item's local coordinates
            QPointF localStart = lineItem->line().p1();
            // Map the local start point of the line to the scene's coordinate system,
            // considering the group's position and transformations.
            return arrowGroup->mapToScene(localStart);
        }

        // Check if the child is a QGraphicsPathItem
        if (QGraphicsPathItem* pathItem = dynamic_cast<QGraphicsPathItem*>(child)) {
            // Found a path item (used for curved arrows or loops)
            // pointAtPercent(0.0) gives the start point of the path in the item's local coordinates
            QPointF localStart = pathItem->path().pointAtPercent(0.0);
            // Map the local start point of the path to the scene's coordinate system
            return arrowGroup->mapToScene(localStart);
        }
    }

    // 3. If no line or path item was found within the group
    // (This might indicate an issue with how drawArrow creates the group)
    //qWarning() << "Could not find a QGraphicsLineItem or QGraphicsPathItem within the arrow group (ID:"
               //<< arrowGroup->data(1).toInt() // Attempt to log the transition ID for debugging
               //<< ") to determine its start position.";
    return QPointF(); // Return default point
}
 
 