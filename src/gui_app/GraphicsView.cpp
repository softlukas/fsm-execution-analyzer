/**
 * @file GraphicsView.cpp
 * @brief Implementation of the custom GraphicsView class.
 */
#include "GraphicsView.h"
#include <QDebug> // For debug output
#include <QGraphicsItemGroup> // Needed for casting

// ... Constructors ...
GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent) {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        setDragMode(QGraphicsView::ScrollHandDrag);
    }

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        setDragMode(QGraphicsView::ScrollHandDrag);
    }

void GraphicsView::wheelEvent(QWheelEvent *event) {
    // Faktor škálovania pre jeden krok kolieska (napr. 15%)
    const qreal scaleFactor = 1.15;

    if (event->angleDelta().y() > 0) {
        // Zoom In
        scale(scaleFactor, scaleFactor);
    } else if (event->angleDelta().y() < 0) {
        // Zoom Out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
    // event->accept(); // Môžeš nechať alebo odstrániť, ak nechceš explicitne zastaviť šírenie
}

/**
 * @brief Handles mouse press events within the view.
 */
void GraphicsView::mousePressEvent(QMouseEvent *event) {
    // Check if the click was a left mouse button click
    //if (event->button() == Qt::LeftButton) {
        // Get the item at the click position
        QGraphicsItem *clickedItem = itemAt(event->pos()); // itemAt uses view coordinates

        QGraphicsItemGroup *stateItemGroup = nullptr; // Initialize to nullptr
        QGraphicsLineItem* lineItem = nullptr; // Initialize to nullptr
        // --- Logic to find the parent QGraphicsItemGroup ---
        if (clickedItem) {
            // Check if the clicked item itself is the group
            stateItemGroup = dynamic_cast<QGraphicsItemGroup*>(clickedItem);
            if (!stateItemGroup) {
                // If not, check if its parent is the group we are looking for
                // This handles clicks on the ellipse or text inside the group
                stateItemGroup = dynamic_cast<QGraphicsItemGroup*>(clickedItem->parentItem());
            }
            lineItem = dynamic_cast<QGraphicsLineItem*>(clickedItem);
            
        }
        // --- End of logic ---

        qDebug() << "Mouse pressed in GraphicsView. Item clicked:" << clickedItem;
        if(stateItemGroup) {
             qDebug() << "Identified State Item Group:" << stateItemGroup->data(0).toString(); // Log name if possible
        } if(lineItem) {
             qDebug() << "Identified Line Item:" << lineItem->line();
        }
         else {
             qDebug() << "Clicked item is not a state group or background.";
        }

        // Emit the signal with the found state item group (or nullptr if background/other item clicked)
        if (event->button() == Qt::LeftButton) {
            emit stateItemClicked(stateItemGroup, lineItem); // Emit signal with nullptr for line item
        }
        if(event->button() == Qt::RightButton) {
            emit stateItemRightClicked(stateItemGroup, lineItem); // Emit signal with nullptr for line item
            // Handle right-click logic here if needed
        }
        
    //}

    // Call the base class implementation AFTER emitting the signal
    // to handle default behavior (like selection propagation which might clear our highlight)
    QGraphicsView::mousePressEvent(event);
}
