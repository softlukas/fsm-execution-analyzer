#include <string>
#include <QString>
#include <QInputDialog>
#include <QDebug>
#include <QtMath>
#include <QGraphicsScene>

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

        // Shorten the line connecting startPos and endPos by 50 points on both sides
        QLineF shortenedLine(startPos, endPos);
        if (shortenedLine.length() > 100) { // Ensure the line is long enough to shorten
            QPointF offset = shortenedLine.unitVector().p2() - shortenedLine.unitVector().p1();
            shortenedLine.setP1(shortenedLine.p1() + offset * 50);
            shortenedLine.setP2(shortenedLine.p2() - offset * 50);
        }

       

       

        QPointF adjustedStartPos = shortenedLine.p1();
        QPointF adjustedEndPos = shortenedLine.p2();

        // --- Draw Curved Arrow ---
        QLineF line(adjustedStartPos, adjustedEndPos);

        QPainterPath curvePath;
        curvePath.moveTo(adjustedStartPos);

        QPointF midPoint = line.pointAt(0.5);
        QPointF delta = adjustedEndPos - adjustedStartPos;
        QPointF perp(delta.y(), -delta.x());
        qreal perpLength = QLineF(QPointF(0,0), perp).length();
        QPointF normPerp = (perpLength > 0) ? (perp / perpLength) : QPointF(0, -1);
        //qreal curveFactor = (adjustedEndPos.x() > adjustedStartPos.x()) ? -1.0 : 1.0;

        qDebug() << "------------------------------";
        qDebug() << "Adjusted Start Position:" << adjustedStartPos.x();
        qDebug() << "Adjusted End Position:" << adjustedEndPos.x();
        qDebug() << "------------------------------";

        /*
        // Nová logika pre curveFactor podľa tvojich požiadaviek:
        qreal curveFactor;
        if (adjustedStartPos.x() < adjustedEndPos.x()) {
            // Šípka smeruje prevažne doprava
            curveFactor = 1.0; // Jeden smer zakrivenia
        } else if (adjustedStartPos.x() > adjustedEndPos.x()) {
            // Šípka smeruje prevažne doľava
            curveFactor = -1.0; // Opačný smer zakrivenia
        } else {
            // X-ové súradnice sú rovnaké (vertikálna šípka)
            if (adjustedStartPos.y() < adjustedEndPos.y()) {
                // Šípka smeruje nadol
                curveFactor = 1.0; // Jeden smer zakrivenia pre vertikálne
            } else {
                // Šípka smeruje nahor (alebo adjustedStartPos.y() == adjustedEndPos.y(), čo by znamenalo rovnaké body,
                // ale to je už ošetrené v hlavnej if podmienke funkcie)
                curveFactor = -1.0; // Opačný smer zakrivenia pre vertikálne
            }
        }
        */


        // Použijeme pôvodné stredy stavov (sceneStartCenter, sceneEndCenter)
        // na určenie kanonického smeru pre výpočet curveFactor.
        // Tým zabezpečíme, že curveFactor bude rovnaký pre A->B aj B->A.
        // Keďže normPerp mení znamienko, výsledné vyklenutie bude opačné.

        QPointF p_ref_start = startPos; // Pôvodný stred začiatočného stavu
        QPointF p_ref_end = endPos;     // Pôvodný stred koncového stavu

        // Určíme kanonický "začiatok" a "koniec" pre pár bodov, aby bol curveFactor konzistentný.
        // Napríklad, bod s menšou x-ovou súradnicou je "prvý". Ak sú x rovnaké, rozhodne y.
        QPointF canonicalOrderedStart = p_ref_start;
        QPointF canonicalOrderedEnd = p_ref_end;

        if (p_ref_start.x() > p_ref_end.x()) {
            canonicalOrderedStart = p_ref_end;
            canonicalOrderedEnd = p_ref_start;
        } else if (p_ref_start.x() == p_ref_end.x()) {
            if (p_ref_start.y() > p_ref_end.y()) {
                canonicalOrderedStart = p_ref_end;
                canonicalOrderedEnd = p_ref_start;
            }
        }
        // Teraz máme canonicalOrderedStart a canonicalOrderedEnd, kde canonicalOrderedStart je "menší".

        qreal curveFactor;
        // Aplikujeme tvoju logiku na tieto kanonicky usporiadané body:
        // "pokial xova suradnica statru < x ciela, chod jednym smerom, inak opacne."
        // V našom prípade canonicalOrderedStart.x() bude vždy <= canonicalOrderedEnd.x()
        // (okrem prípadu, keď sú body identické, čo je ošetrené vyššie).
        // Takže sa zameriame na hlavný smer.
        
        // Ak chceme, aby sa to vždy vyklenulo napr. "jedným smerom" pre kanonický smer A->B
        // a `normPerp` sa postará o otočenie pre B->A, môžeme `curveFactor` nastaviť na konštantu.
        // Napríklad vždy 1.0.
        // curveFactor = 1.0;

        int deltaX = adjustedStartPos.x() - adjustedStartPos.x();
        int deltaY = adjustedEndPos.y() - adjustedEndPos.y();


        // Ak chceš použiť tvoju logiku na kanonických bodoch:
        if (canonicalOrderedStart.x() < canonicalOrderedEnd.x()) {
           
            curveFactor = 1.0; // "Jeden smer" pre hlavný smer x
        } else if (canonicalOrderedStart.x() > canonicalOrderedEnd.x()) {
            
             // Toto by sa nemalo stať po kanonickom usporiadaní, ale pre úplnosť
            curveFactor = -1.0;
            //QPen arrowPen(Qt::green, 2); // Zmenené na modrú
        } else { // X-ové súradnice sú rovnaké
            if (canonicalOrderedStart.y() < canonicalOrderedEnd.y()) {
                
                curveFactor = 1.0; // "Jeden smer" pre hlavný smer y
            } else if (canonicalOrderedStart.y() > canonicalOrderedEnd.y()) {
               
                curveFactor = -1.0;
                //QPen arrowPen(Qt::green, 2);
            } else {
                // Body sú identické, ale to je ošetrené v hlavnej if podmienke.
                // Pre istotu nastavíme default.
                curveFactor = 1.0;
            }
        }
        // Dôležité: Tento curveFactor je teraz rovnaký pre A->B aj B->A,
        // pretože je odvodený od kanonicky usporiadaných pôvodných stredov.


        qreal curveMagnitude = qMin(line.length() * 0.2, 40.0);
        QPointF controlPoint = midPoint + normPerp * curveMagnitude * curveFactor;

        curvePath.quadTo(controlPoint, adjustedEndPos);

        QGraphicsPathItem* curveItem = new QGraphicsPathItem(curvePath);
        curveItem->setPen(arrowPen);
        arrowGroup->addToGroup(curveItem);

        QLineF endSegment(controlPoint, adjustedEndPos);
        qreal angleDeg = endSegment.angle();
        double angleRad = qDegreesToRadians(-angleDeg);

        QPointF arrowP1 = adjustedEndPos + QPointF(cos(angleRad + M_PI / 6.0) * arrowSize,
                                           sin(angleRad + M_PI / 6.0) * arrowSize);
        QPointF arrowP2 = adjustedEndPos + QPointF(cos(angleRad - M_PI / 6.0) * arrowSize,
                                           sin(angleRad - M_PI / 6.0) * arrowSize);

        QPolygonF arrowHead;
        arrowHead << adjustedEndPos << arrowP1 << arrowP2;
        QGraphicsPolygonItem *arrowHeadItem = new QGraphicsPolygonItem(arrowHead);
        arrowHeadItem->setPen(arrowPen);
        arrowHeadItem->setBrush(Qt::red);
        arrowGroup->addToGroup(arrowHeadItem);

        QGraphicsTextItem* text = new QGraphicsTextItem(label);
        QPointF labelPos = curvePath.pointAtPercent(0.5);
        text->setPos(labelPos + normPerp * (curveFactor > 0 ? 10 : -15));
        arrowGroup->addToGroup(text);

        *actualStartPos = startPos;
        *actualEndPos = endPos;
    }

    arrowGroup->setFlag(QGraphicsItem::ItemIsSelectable);
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
