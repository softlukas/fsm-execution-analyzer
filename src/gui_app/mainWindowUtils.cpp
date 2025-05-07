#include <string>
#include <QString>
#include <QInputDialog>
#include <QDebug>
#include <QtMath>
#include <QGraphicsScene>

// Define a constant for the state radius
const qreal STATE_RADIUS = 30.0; // Adjust the value as needed
const qreal ARROW_SIZE = 10.0; // Adjust the value as needed

#include "mainWindowUtils.h"

std::string MainWindowUtils::ProccessOneArgumentDialog(const std::string& textToDisplay) {

    QInputDialog dialog;
    dialog.setWindowTitle("Enter " + QString::fromStdString(textToDisplay));
    dialog.setLabelText(QString::fromStdString(textToDisplay));

    // Nastavíme väčšie písmo
    QFont font = dialog.font();
    font.setPointSize(16);
    dialog.setFont(font);

    // Nastavíme väčšiu minimálnu veľkosť okna
    dialog.setMinimumSize(350, 350);

    // Zobrazíme dialóg
    if (dialog.exec() == QDialog::Accepted) {
        QString input = dialog.textValue();
        if (!input.trimmed().isEmpty()) {
            return input.trimmed().toStdString();
        }
    }

    qDebug() << "Dialog cancelled or empty input.";
    return "";  // Namiesto nullptr - prázdny string je bezpečnejší
}


QGraphicsItemGroup* MainWindowUtils::drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene, QPointF *actualStartPos, QPointF *actualEndPos) {
    // --- Common Arrow Properties ---
    QPen arrowPen(Qt::red, 2); // Pen for the arrow line and head
    qreal arrowSize = 10;      // Size of the arrowhead

    // --- Create a group for the entire transition representation ---
    QGraphicsItemGroup *arrowGroup = new QGraphicsItemGroup();

    // --- Store Transition ID in the group ---
    arrowGroup->setData(0, "Transition"); // Item type identifier
    arrowGroup->setData(1, QVariant(transitionId)); // Store the actual transition ID

    // Check if start and end points are the same (or very close) for self-loop
    if (QLineF(startPos, endPos).length() < 1.0) {
        // --- Draw Self-Loop (Arc) ---
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
        arrowLHeadItem->setPen(arrowPen); arrowLHeadItem->setBrush(Qt::red);
        arrowGroup->addToGroup(arrowLHeadItem);

        QGraphicsTextItem* textL = new QGraphicsTextItem(label);
        QRectF textLRect = textL->boundingRect();
        textL->setPos(loopTopCenter.x() - textLRect.width() / 2, loopTopCenter.y() - radiusY - textLRect.height() - 2);
        arrowGroup->addToGroup(textL);

        *actualStartPos = startPos;
        *actualEndPos = loopEnd;

    } else {

// This is the 'else' block within your MainWindowUtils::drawArrow function.
// It assumes 'startPos' and 'endPos' are the center points of the start and end states,
// and 'arrowPen', 'ARROW_SIZE', 'STATE_RADIUS' are defined earlier in the function or globally.
// QGraphicsItemGroup *arrowGroup, QGraphicsScene *scene,
// QString label, int transitionId,
// QPointF *actualStartPos, QPointF *actualEndPos are also parameters to drawArrow.

// --- Kreslenie prechodu medzi dvoma rôznymi stavmi (A -> B) ---
// startPos a endPos sú tu CENTRÁ stavov

// Predpokladáme, že STATE_RADIUS je definovaný (napr. const qreal STATE_RADIUS = 30.0;)
// Predpokladáme, že ARROW_SIZE je definovaný (napr. const qreal ARROW_SIZE = 10.0;)

qreal angularOffsetForAttachment = 0.35; // Uhlový posun (cca 20 stupňov) - môžeš experimentovať
const qreal gap = 2.5; // <<< NOVÁ KONŠTANTA PRE MEDZERU MEDZI ŠÍPKOU A STAVOM

// Smerový faktor pre posunutie - určený kanonicky pre pár stavov
// Aby A->B a B->A mali konzistentný, ale opačný vizuálny posun
qreal pairOffsetDirectionFactor;
if (startPos.x() < endPos.x()) { // Používame pôvodné stredy (startPos, endPos)
    pairOffsetDirectionFactor = 1.0; // Napr. A->B (A je vľavo od B) dostane +1
} else if (startPos.x() > endPos.x()) {
    pairOffsetDirectionFactor = -1.0; // Napr. B->A (B je vpravo od A, teda startPos.x > endPos.x) dostane -1
} else { // X súradnice sú rovnaké, rozhodne Y
    if (startPos.y() < endPos.y()) { // A je vyššie ako B (šípka ide dole)
        pairOffsetDirectionFactor = 1.0;
    } else if (startPos.y() > endPos.y()){ // A je nižšie ako B (šípka ide hore)
        pairOffsetDirectionFactor = -1.0;
    } else {
         // Stavy sú na sebe - tento prípad je už ošetrený v hlavnej if podmienke
         pairOffsetDirectionFactor = 1.0; // Default
    }
}

// Uhol priamky medzi stredmi stavov
qreal lineAngleRad = qAtan2(-(endPos.y() - startPos.y()), // Y je invertované v Qt
                           endPos.x() - startPos.x());

// Výpočet adjustedStartPos, teraz s medzerou od kružnice
// Bod, kde sa začína čiara šípky, bude (STATE_RADIUS + gap) od stredu začiatočného stavu.
qreal startAttachAngle = lineAngleRad - pairOffsetDirectionFactor * angularOffsetForAttachment;
QPointF adjustedStartPos(startPos.x() + (STATE_RADIUS + gap) * qCos(startAttachAngle),
                         startPos.y() - (STATE_RADIUS + gap) * qSin(startAttachAngle)); // -sin kvôli Y osi

// Výpočet adjustedEndPos, teraz s medzerou od kružnice
// Bod, kam smeruje hrot šípky, bude (STATE_RADIUS + gap) od stredu koncového stavu.
qreal endToStartAngleRad = lineAngleRad + M_PI;
qreal endAttachAngle = endToStartAngleRad - pairOffsetDirectionFactor * angularOffsetForAttachment;
QPointF adjustedEndPos(endPos.x() + (STATE_RADIUS + gap) * qCos(endAttachAngle),
                       endPos.y() - (STATE_RADIUS + gap) * qSin(endAttachAngle)); // -sin kvôli Y osi


// --- Kreslenie zakrivenej šípky medzi novými adjustedStartPos a adjustedEndPos ---
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

// --- Hrot šípky ---
// Smer posledného segmentu krivky (od kontrolného bodu k koncovému bodu)
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
arrowHeadItem->setBrush(Qt::red);
arrowGroup->addToGroup(arrowHeadItem);

// --- Text podmienky ---
QGraphicsTextItem* textItem = new QGraphicsTextItem(label);
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

    // Prejdeme všetky položky najvyššej úrovne v scéne
    for (QGraphicsItem* item : scene->items()) {
        // Bezpečné pretypovanie na QGraphicsItemGroup
        QGraphicsItemGroup* group = qgraphicsitem_cast<QGraphicsItemGroup*>(item);

        // Skontrolujeme, či je to skupina a či má platné dáta pre naše kľúče
        if (group && group->data(0).isValid() && group->data(1).isValid()) {
            // Porovnáme ID (kľúč 0) a typ (kľúč 1)
            if (group->data(1).toInt() == id && group->data(0).toString() == type) {
                // Našli sme zhodu!
                return group;
            }
        }
    }

    // Prešli sme všetky položky a nenašli sme zhodu
    qDebug() << "findItemGroupByIdAndType: Group with ID" << id << "and type" << type << "not found.";
    return nullptr;
}

/*
static QPointF MainWindowUtils::adjustPointToEdge(const QGraphicsItemGroup* stateGroup, const QPointF& targetCenter, const QPointF& otherPoint, qreal padding = 5.0)
{
    if (!stateGroup) {
        qWarning() << "adjustPointToEdge: stateGroup is null.";
        return targetCenter; // Cannot calculate without the group
    }

    // Estimate the radius of the state group (assuming roughly circular/elliptical)
    // Use the smaller dimension of the bounding rect for a safer radius estimate
    qreal radius = qMin(stateGroup->sceneBoundingRect().width(),
                        stateGroup->sceneBoundingRect().height()) / 2.0;

    // Create a line from the target center towards the other point
    QLineF line(targetCenter, otherPoint);

    // Check if the line length is valid (greater than radius + padding)
    // If the points are too close, just return the center to avoid issues.
    if (line.length() <= (radius + padding) || line.length() < 0.001) {
        qDebug() << "adjustPointToEdge: Points too close or line length zero, returning targetCenter.";
        // In this case, the arrow would likely start/end inside the state anyway.
        // Returning the center is a safe fallback.
        return targetCenter;
    }

    // Set the length of the line to be radius + padding
    line.setLength(radius + padding);

    // The endpoint of this adjusted line (p2()) is the desired point
    return line.p2();
}
    */
