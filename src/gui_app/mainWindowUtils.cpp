#include <string>
#include <QString>
#include <QInputDialog>
#include <QDebug>
#include <QtMath>
#include <QGraphicsScene>

#include "mainWindowUtils.h"

std::string MainWindowUtils::ProccessOneArgumentDialog(const std::string& textToDisplay) {

    
    bool okClicked;
    QString input = QInputDialog::getText(nullptr, "Enter " + QString::fromStdString(textToDisplay),
                                          QString::fromStdString(textToDisplay), QLineEdit::Normal,
                                          "", &okClicked);
    if (!okClicked || input.trimmed().isEmpty()) {
        qDebug() << "Dialog cancelled or empty input.";
        return nullptr; // Return an empty string if cancelled or no input
    }
    return input.trimmed().toStdString();
}

/*
QGraphicsItemGroup* MainWindowUtils::drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene) {
   

    // --- Common Arrow Properties ---
    QPen arrowPen(Qt::red, 2); // Pen for the arrow line and head
    qreal arrowSize = 10;      // Size of the arrowhead

    // --- Create a group for the entire transition representation ---
    QGraphicsItemGroup *arrowGroup = new QGraphicsItemGroup();

    // --- Store Transition ID in the group ---
    arrowGroup->setData(0, "Transition"); // Item type identifier
    arrowGroup->setData(1, QVariant(transitionId)); // Store the actual transition ID
    // Optional: Store start/end state names if needed for selection, passed as extra args
    // arrowGroup->setData(2, QVariant(startStateName));
    // arrowGroup->setData(3, QVariant(targetStateName));


    // Check if start and end points are the same (or very close) for self-loop
    // Use a small tolerance for floating point comparison
    if (QLineF(startPos, endPos).length() < 1.0) {
        // --- Draw Self-Loop (Arc) ---
        qDebug() << "Drawing self-loop at" << startPos << "ID:" << transitionId;

        // Define loop parameters relative to the point
        qreal radiusX = 30; // Fixed size loop radius
        qreal radiusY = 30;
        // Position the loop slightly above the point
        QPointF loopTopCenter(startPos.x(), startPos.y() - radiusY * 1.2); // Adjust vertical offset

        // Create the path for the arc
        QPainterPath loopPath;
        qreal startAngle = 210; qreal spanAngle = -240;
        QRectF arcRect(loopTopCenter.x() - radiusX, loopTopCenter.y() - radiusY, 2 * radiusX, 2 * radiusY);
        loopPath.arcMoveTo(arcRect, startAngle);
        loopPath.arcTo(arcRect, startAngle, spanAngle);
        QGraphicsPathItem* loopItem = new QGraphicsPathItem(loopPath);
        loopItem->setPen(arrowPen);
        arrowGroup->addToGroup(loopItem);

        // Arrowhead for loop
        QPointF loopEnd = loopPath.currentPosition();
        qreal angle = loopPath.angleAtPercent(1.0);
        QPointF arrowLP1 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) + M_PI / 3) * arrowSize, qCos(qDegreesToRadians(-angle) + M_PI / 3) * arrowSize);
        QPointF arrowLP2 = loopEnd + QPointF(qSin(qDegreesToRadians(-angle) + M_PI - M_PI / 3) * arrowSize, qCos(qDegreesToRadians(-angle) + M_PI - M_PI / 3) * arrowSize);
        QPolygonF arrowLHead; arrowLHead << loopEnd << arrowLP1 << arrowLP2;
        QGraphicsPolygonItem *arrowLHeadItem = new QGraphicsPolygonItem(arrowLHead);
        arrowLHeadItem->setPen(arrowPen); arrowLHeadItem->setBrush(Qt::red);
        arrowGroup->addToGroup(arrowLHeadItem);

        // Label for loop
        QGraphicsTextItem* textL = new QGraphicsTextItem(label);
        QRectF textLRect = textL->boundingRect();
        textL->setPos(loopTopCenter.x() - textLRect.width() / 2, loopTopCenter.y() - radiusY - textLRect.height() - 2);
        arrowGroup->addToGroup(textL);

    } else {
        // --- Draw Curved Arrow ---
        qDebug() << "Drawing curved arrow from" << startPos << "to" << endPos << "ID:" << transitionId;

        // Line connecting the provided start and end points
        QLineF line(startPos, endPos);

        // Create the path object
        QPainterPath curvePath;
        curvePath.moveTo(startPos); // Start at the start point

        // Calculate control point for the quadratic Bezier curve
        QPointF midPoint = line.pointAt(0.5);
        QPointF delta = endPos - startPos;
        QPointF perp(delta.y(), -delta.x()); // Perpendicular vector
        qreal perpLength = QLineF(QPointF(0,0), perp).length();
        QPointF normPerp = (perpLength > 0) ? (perp / perpLength) : QPointF(0, -1);
        qreal curveFactor = (endPos.x() > startPos.x()) ? -1.0 : 1.0; // Determine curve direction
        qreal curveMagnitude = qMin(line.length() * 0.2, 40.0); // Bend amount
        QPointF controlPoint = midPoint + normPerp * curveMagnitude * curveFactor;

        // Add the quadratic curve to the path
        curvePath.quadTo(controlPoint, endPos);

        // Create a QGraphicsPathItem for the curve
        QGraphicsPathItem* curveItem = new QGraphicsPathItem(curvePath);
        curveItem->setPen(arrowPen);
        arrowGroup->addToGroup(curveItem); // Add curve to the group

        // --- Calculate Arrowhead for the curve ---
        // Use angle of the line segment from control point to end point
        QLineF endSegment(controlPoint, endPos);
        qreal angleDeg = endSegment.angle(); // Angle in degrees
        double angleRad = qDegreesToRadians(-angleDeg); // Convert for trig functions

        QPointF arrowP1 = endPos + QPointF(cos(angleRad - M_PI / 6.0) * arrowSize,
                                           sin(angleRad - M_PI / 6.0) * arrowSize);
        QPointF arrowP2 = endPos + QPointF(cos(angleRad + M_PI / 6.0) * arrowSize,
                                           sin(angleRad + M_PI / 6.0) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << endPos << arrowP1 << arrowP2; // Points for the triangle
        QGraphicsPolygonItem *arrowHeadItem = new QGraphicsPolygonItem(arrowHead);
        arrowHeadItem->setPen(arrowPen);
        arrowHeadItem->setBrush(Qt::red); // Fill the arrowhead
        arrowGroup->addToGroup(arrowHeadItem); // Add arrowhead to the group

        // --- Position Label for the curve ---
        QGraphicsTextItem* text = new QGraphicsTextItem(label);
        QPointF labelPos = curvePath.pointAtPercent(0.5); // Midpoint of the curve path
        text->setPos(labelPos + normPerp * (curveFactor > 0 ? 10 : -15)); // Offset label
        arrowGroup->addToGroup(text); // Add label to the group
    }

    // --- Finalize Group ---
    arrowGroup->setFlag(QGraphicsItem::ItemIsSelectable); // Make the whole group selectable
     // Add the complete group to the scene

    qDebug() << "Arrow/Loop group added to scene for transition ID:" << transitionId;
    scene->addItem(arrowGroup); // Add the group to the scene
    return arrowGroup; // Return the created group
}*/