/**
 * @file mainWindowUtils.cpp
 * @brief Implementation file for the MainWindowUtils class.
 * @details This file contains the implementation of the MainWindowUtils class, which provides
 * utility functions for the main GUI application. These functions include dialog handling,
 * arrow drawing, and item group management within the graphics scene.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */


#include <string>
#include <QString>
#include <QInputDialog>
#include <QDebug>
#include <QtMath>
#include <QGraphicsScene>
#include "mainWindowUtils.h"

// Define a constant for the transition radius
const qreal STATE_RADIUS = 30.0;
const qreal ARROW_SIZE = 10.0;



std::string MainWindowUtils::ProccessOneArgumentDialog(const std::string& textToDisplay) {

    QInputDialog dialog;
    dialog.setWindowTitle("Enter " + QString::fromStdString(textToDisplay));
    dialog.setLabelText(QString::fromStdString(textToDisplay));

    
    QFont font = dialog.font();
    font.setPointSize(16);
    dialog.setFont(font);

    
    dialog.setMinimumSize(350, 350);

    
    if (dialog.exec() == QDialog::Accepted) {
        QString input = dialog.textValue();
        if (!input.trimmed().isEmpty()) {
            return input.trimmed().toStdString();
        }
    }

    qDebug() << "Dialog cancelled or empty input.";
    return "";
}


QGraphicsItemGroup* MainWindowUtils::drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene, QPointF *actualStartPos, QPointF *actualEndPos) {
    // --- Common Arrow Properties ---
    QPen arrowPen(Qt::red, 2); // Pen for the arrow line and head
    qreal arrowSize = 10;      // Size of the arrowhead

    Qt::GlobalColor defaultTextColor = Qt::red; // Default color

    if(endPos.x() < startPos.x()) {
        arrowPen.setColor(Qt::blue); // Change color if the arrow is going left
        defaultTextColor = Qt::blue; // Change also text color to blue
    }



    // --- Create a group for the entire transition representation ---
    QGraphicsItemGroup *arrowGroup = new QGraphicsItemGroup();

    // --- Store Transition ID in the group ---
    arrowGroup->setData(0, "Transition"); // Item type identifier
    arrowGroup->setData(1, QVariant(transitionId)); // Store the actual transition ID

    // Check if start and end points are the same (or very close) for self-loop
    // draw arrow from state A to A
    if (QLineF(startPos, endPos).length() < 1.0) {
        // --- Draw Self-Loop
        qreal radiusX = 30; // Fixed size loop radius
        qreal radiusY = 30;
        QPointF loopTopCenter(startPos.x(), startPos.y() - radiusY * 1.2);

        QPainterPath loopPath;
        qreal startAngle = 210; qreal spanAngle = -240;
        QRectF arcRect(loopTopCenter.x() - radiusX, loopTopCenter.y() - radiusY, 2 * radiusX, 2 * radiusY);
        loopPath.arcMoveTo(arcRect, startAngle);
        loopPath.arcTo(arcRect, startAngle, spanAngle);
        QGraphicsPathItem* loopItem = new QGraphicsPathItem(loopPath);
        loopItem->setPen(arrowPen);
        arrowGroup->addToGroup(loopItem);

        QPointF loopEnd = loopPath.currentPosition();
        qreal angle = loopPath.angleAtPercent(1.0);
        QPointF arrowLP1 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) - M_PI / 3) * arrowSize, qCos(qDegreesToRadians(-angle) - M_PI / 3) * arrowSize);
        QPointF arrowLP2 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) - M_PI + M_PI / 3) * arrowSize, qCos(qDegreesToRadians(-angle) - M_PI + M_PI / 3) * arrowSize);
        QPolygonF arrowLHead; arrowLHead << loopEnd << arrowLP1 << arrowLP2;
        QGraphicsPolygonItem *arrowLHeadItem = new QGraphicsPolygonItem(arrowLHead);
        arrowLHeadItem->setPen(arrowPen); arrowLHeadItem->setBrush(defaultTextColor);
        arrowGroup->addToGroup(arrowLHeadItem);

        QGraphicsTextItem* textL = new QGraphicsTextItem(label);
        textL->setDefaultTextColor(defaultTextColor); // Set the text color to blue
        QRectF textLRect = textL->boundingRect();
        textL->setPos(loopTopCenter.x() - textLRect.width() / 2, loopTopCenter.y() - radiusY - textLRect.height() - 2);
        arrowGroup->addToGroup(textL);

        *actualStartPos = startPos;
        *actualEndPos = loopEnd;

    } else {
        // draw arrow form state A to state B
        qreal angularOffsetForAttachment = 0.35; // Angular offset (approximately 20 degrees) - you can experiment
        const qreal gap = 2.5; // <<< NEW CONSTANT FOR THE GAP BETWEEN THE ARROW AND THE STATE

        // Directional factor for offset - determined canonically for a pair of states
        // Ensures A->B and B->A have consistent but opposite visual offsets
        qreal pairOffsetDirectionFactor;
        if (startPos.x() < endPos.x()) { // Using original centers (startPos, endPos)
            pairOffsetDirectionFactor = 1.0; // For example, A->B (A is to the left of B) gets +1
        } else if (startPos.x() > endPos.x()) {
            pairOffsetDirectionFactor = -1.0; // For example, B->A (B is to the right of A, so startPos.x > endPos.x) gets -1
        } else { // X coordinates are the same, decide based on Y
            if (startPos.y() < endPos.y()) { // A is above B (arrow goes down)
                pairOffsetDirectionFactor = 1.0;
            } else if (startPos.y() > endPos.y()){ // A is below B (arrow goes up)
                pairOffsetDirectionFactor = -1.0;
            } else {
                // States overlap - this case is already handled in the main if condition
                pairOffsetDirectionFactor = 1.0; // Default
            }
        }

        // Angle of the line between the centers of the states
        qreal lineAngleRad = qAtan2(-(endPos.y() - startPos.y()), // Y is inverted in Qt
                                endPos.x() - startPos.x());

        // Calculate adjustedStartPos, now with a gap from the circle
        // The point where the arrow line starts will be (STATE_RADIUS + gap) away from the center of the starting state.
        qreal startAttachAngle = lineAngleRad - pairOffsetDirectionFactor * angularOffsetForAttachment;
        QPointF adjustedStartPos(startPos.x() + (STATE_RADIUS + gap) * qCos(startAttachAngle),
                                startPos.y() - (STATE_RADIUS + gap) * qSin(startAttachAngle)); // -sin due to Y axis

        // Calculate adjustedEndPos, now with a gap from the circle
        // The point where the arrowhead points will be (STATE_RADIUS + gap) away from the center of the ending state.
        qreal endToStartAngleRad = lineAngleRad + M_PI;
        qreal endAttachAngle = endToStartAngleRad - pairOffsetDirectionFactor * angularOffsetForAttachment;
        QPointF adjustedEndPos(endPos.x() + (STATE_RADIUS + gap) * qCos(endAttachAngle),
                            endPos.y() - (STATE_RADIUS + gap) * qSin(endAttachAngle)); // -sin due to Y axis


        // --- Draw a curved arrow between the new adjustedStartPos and adjustedEndPos ---
        QLineF line(adjustedStartPos, adjustedEndPos);

        QPainterPath curvePath;
        curvePath.moveTo(adjustedStartPos);

        QPointF midPoint = line.pointAt(0.5);
        QPointF delta = adjustedEndPos - adjustedStartPos;
        QPointF perp(delta.y(), -delta.x());
        qreal perpLength = QLineF(QPointF(0,0), perp).length();
        QPointF normPerp = (perpLength > 0) ? (perp / perpLength) : QPointF(0, -1);

        qreal curveMagnitude = qMin(line.length() * 0.20, 30.0);

        qreal bezierCurveFactor = 1.0; 


        QPointF controlPoint = midPoint + normPerp * curveMagnitude * bezierCurveFactor;

        curvePath.quadTo(controlPoint, adjustedEndPos);

        QGraphicsPathItem* curveItem = new QGraphicsPathItem(curvePath);
        curveItem->setPen(arrowPen);
        arrowGroup->addToGroup(curveItem);

        // --- Arrowhead ---
        // Direction of the last segment of the curve (from the control point to the endpoint)
        QLineF endSegmentDirection(controlPoint, adjustedEndPos);
        if (endSegmentDirection.length() < 0.01) { 
            endSegmentDirection = QLineF(adjustedStartPos, adjustedEndPos); 
            if (endSegmentDirection.length() < 0.01 && line.length() > 0.01) { 
                endSegmentDirection = line;
            } else if (endSegmentDirection.length() < 0.01) { 
                endSegmentDirection = QLineF(adjustedEndPos, adjustedEndPos + QPointF(1,0));
            }
        }

        double angleRadArrow = qAtan2(-(endSegmentDirection.dy()), endSegmentDirection.dx());

        QPointF arrowP1 = adjustedEndPos - QPointF(cos(angleRadArrow - M_PI / 6.0) * ARROW_SIZE,
                                                -sin(angleRadArrow - M_PI / 6.0) * ARROW_SIZE); 
        QPointF arrowP2 = adjustedEndPos - QPointF(cos(angleRadArrow + M_PI / 6.0) * ARROW_SIZE,
                                                -sin(angleRadArrow + M_PI / 6.0) * ARROW_SIZE); 


        QPolygonF arrowHeadPolygon;
        arrowHeadPolygon << adjustedEndPos << arrowP1 << arrowP2;
        QGraphicsPolygonItem *arrowHeadItem = new QGraphicsPolygonItem(arrowHeadPolygon);
        arrowHeadItem->setPen(arrowPen);
        arrowHeadItem->setBrush(defaultTextColor); // Set the color of the arrowhead
        arrowGroup->addToGroup(arrowHeadItem);

        // --- Condition text ---
        QGraphicsTextItem* textItem = new QGraphicsTextItem(label);
        textItem->setDefaultTextColor(defaultTextColor); // Set the text color to blue
        QPointF textOffsetDirection = normPerp * bezierCurveFactor; 
        qreal textDistance = 12; 

        QPointF labelPositionOffset; 
        qreal textOffsetDirectionLength = QLineF(QPointF(0,0), textOffsetDirection).length();
        QPointF normalizedTextOffsetDirection = (textOffsetDirectionLength > 0) ? (textOffsetDirection / textOffsetDirectionLength) : QPointF(0,0);

        if (textOffsetDirection.y() < 0) { 
            labelPositionOffset = normalizedTextOffsetDirection * textDistance;
        } else { 
            labelPositionOffset = normalizedTextOffsetDirection * (textDistance + textItem->boundingRect().height());
        }
        if (qAbs(delta.x()) > qAbs(delta.y()) * 2 ) { 
            labelPositionOffset.setY(labelPositionOffset.y() - textItem->boundingRect().height() / 2.0);
        }


        textItem->setPos(controlPoint + labelPositionOffset - QPointF(textItem->boundingRect().width()/2,0) );
        arrowGroup->addToGroup(textItem);


        *actualStartPos = startPos;
        *actualEndPos = endPos;
    }

    arrowGroup->setFlag(QGraphicsItem::ItemIsSelectable);
    arrowGroup->setZValue(1); // Z-index for the arrow group
    scene->addItem(arrowGroup);
    return arrowGroup;
}


QGraphicsItemGroup* MainWindowUtils::findItemGroupByIdAndType(QGraphicsScene* scene, int id, const QString& type) {
    if (!scene) {
        qWarning() << "findItemGroupByIdAndType: Invalid scene pointer.";
        return nullptr;
    }

    // Iterate through all top-level items in the scene
    for (QGraphicsItem* item : scene->items()) {
        // Safely cast to QGraphicsItemGroup
        QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);

        // Check if it is a group and if it has valid data for our keys
        if (group && group->data(0).isValid() && group->data(1).isValid()) {
            // Compare ID (key 1) and type (key 0)
            if (group->data(1).toInt() == id && group->data(0).toString() == type) {
                // Match found!
                return group;
            }
        }
    }

    // Went through all items and did not find a match
    qDebug() << "findItemGroupByIdAndType: Group with ID" << id << "and type" << type << "not found.";
    return nullptr;
}


