/**
 * @file State.cpp
 * @brief Implementation of the State class.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-24 // Current creation date
 */

 #include "State.h" // Include the header file
 #include <QGraphicsScene> // Include the QGraphicsScene header
 #include "../gui_app/mainWindowUtils.h" // Include the MainWindowUtils header
 #include <QDebug> // Include the QDebug header for debugging
 #include "Transition.h" // Include the Transition header
#include "Machine.h" // Include the Machine header
 
 State::State(const std::string& name, const std::string& action, int stateId)
     : stateName(name), stateOutput(action), stateId(stateId) {
     // Constructor body can be empty for now,
     // initialization happened in the initializer list.
 }
 
 const std::string& State::getName() const {
     return stateName;
 }
 
 const std::string& State::getAction() const {
     return stateOutput;
 }
 
 void State::setName(const std::string& name) {
     stateName = name;
 }
 
 void State::setAction(const std::string& action) {
     stateOutput = action;
 }
 
 const int State::getStateId() const {
     return stateId;
 }
 
 
 QPointF State::getVisualCenterOfStateItem_static(QGraphicsItemGroup* stateItemGroup) {
    if (!stateItemGroup) {
        qWarning() << "MainWindow::getVisualCenterOfStateItem_static: Received null stateItemGroup.";
        return QPointF(); // Vráť neplatný bod alebo predvolený bod
    }

    for (QGraphicsItem* child : stateItemGroup->childItems()) {
        if (QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(child)) {
            // ellipse->rect() je v lokálnych súradniciach elipsy (napr. (0,0,šírka,výška))
            // ellipse->rect().center() je stred elipsy v jej vlastných súradniciach (napr. (šírka/2, výška/2))
            // mapToScene pretransformuje tento lokálny stred do súradníc scény
            return stateItemGroup->mapToScene(ellipse->rect().center());
        }
    }

    // Fallback, ak sa elipsa nenájde (nemalo by nastať pri správnej štruktúre skupiny)
    qWarning() << "MainWindow::getVisualCenterOfStateItem_static: Ellipse not found in state group for item at scene pos"
               << stateItemGroup->scenePos() << ". Falling back to sceneBoundingRect().center().";
    return getVisualCenterOfStateItem_static(stateItemGroup);
}

void State::updateTransitionPositions(QGraphicsScene* scene, QGraphicsItemGroup* stateGroup, Machine *machine) {
    
    qDebug() << "----------------------------------------------------------------";
    qDebug() << "move method called for state:" << QString::fromStdString(stateName);
    qDebug() << "----------------------------------------------------------------";

    printTransitions();
    qDebug() << "----------------------------------------------------------------";


    //if (outgoingTransitions.empty()) {
        //qDebug() << "No outgoing transitions to update.";
        //return;
    //}
    //if(incomingTranstions.empty()) {
        //qDebug() << "No incoming transitions to update.";
        //return;
    //}

    //qDebug() << "---------------------------------------------------------------";

    for (const auto& [id, transition] : outgoingTransitions) {
        Transition *trans = machine->getTransition(id);
        State *targetState = trans->getTargetState();
        std::string condition = trans->getCondition();

        //QGraphicsItemGroup* group = MainWindowUtils::findItemGroupByIdAndType(scene, id, "Transition");
        QGraphicsItemGroup* group = std::get<0>(transition);

        const QPointF startPos = getVisualCenterOfStateItem_static(stateGroup);
        const QPointF endPos = std::get<2>(transition);

        qDebug() << "StartPos:" << startPos;
        qDebug() << "EndPos:" << endPos;

        if (group && scene->items().contains(group)) {
            scene->removeItem(group);
        } else if (!group) {
            qDebug() << "Group is null. Cannot remove.";
        } else {
            qDebug() << "Group not found in the scene. Cannot remove.";
        }

        //if (startPos == endPos) {
            //continue;
        //}

        QPointF actualStartPos;
        QPointF actualEndPos;

        QGraphicsItemGroup* newTransition = MainWindowUtils::drawArrow(startPos, endPos, QString::fromStdString(condition), id, scene, &actualStartPos, &actualEndPos);
        if (newTransition) {
            outgoingTransitions[id] = std::make_tuple(newTransition, actualStartPos, actualEndPos);
            targetState->setIncomingTransitionGroup(id, newTransition, actualStartPos, actualEndPos);
            qDebug() << "Outgoing transition state updated";
            //incomingTranstions[id] = std::make_tuple(newTransition, actualStartPos, actualEndPos);
        } else {
            
        }
    }

    if (outgoingTransitions.empty()) {
       
    }

    for (const auto& [id, transition] : incomingTranstions) {
        Transition *trans = machine->getTransition(id);
        std::string condition = trans->getCondition();
        State *sourceState = trans->getSourceState();

        QGraphicsItemGroup* group = std::get<0>(transition);
       

        const QPointF startPos = std::get<1>(transition);
        const QPointF endPos = getVisualCenterOfStateItem_static(stateGroup);

        scene->removeItem(group);

        //if (startPos == endPos) {
            //continue;
        //}

        QPointF actualStartPos;
        QPointF actualEndPos;

        QGraphicsItemGroup* newTransition = MainWindowUtils::drawArrow(startPos, endPos, QString::fromStdString(condition), id, scene, &actualStartPos, &actualEndPos);
        if (newTransition) {
            incomingTranstions[id] = std::make_tuple(newTransition, actualStartPos, actualEndPos);
            //outgoingTransitions[id] = std::make_tuple(newTransition, actualStartPos, actualEndPos);
            sourceState->setOutgoingTransitionGroup(id, newTransition, actualStartPos, actualEndPos);

            qDebug() << "Incoming transition state udpdated";
            
        } else {
            
        }
    }
}

 




void State::printTransitions() const {
    qDebug() << "---------------------------------------------------------------";
    qDebug() << "Incoming Transitions: for state " << QString::fromStdString(stateName);
    for (const auto& [id, transition] : incomingTranstions) {
        const QPointF startPos = std::get<1>(transition);
        const QPointF endPos = std::get<2>(transition);
        qDebug() << "ID:" << id << "Start Position:" << startPos << "End Position:" << endPos;

        qDebug() << "Pointer to transition object in gui: " << std::get<0>(transition);
    }
    qDebug() << "---------------------------------------------------------------";
    qDebug() << "Outgoing Transitions: for state " << QString::fromStdString(stateName);
    for (const auto& [id, transition] : outgoingTransitions) {
        const QPointF startPos = std::get<1>(transition);
        const QPointF endPos = std::get<2>(transition);
        qDebug() << "ID:" << id << "Start Position:" << startPos << "End Position:" << endPos;
       
        qDebug() << "Pointer to transition object in gui: " << std::get<0>(transition);
        
    }
}


 