/**
 * @file State.h
 * @brief Defines the State class for managing states in the automaton.
 * @details This header file declares the State class, which encapsulates a single state in the interpreted finite automaton. It provides methods for managing state attributes, transitions, and graphical representations within the automaton.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */


#include "State.h" 
#include <QGraphicsScene> 
#include "../gui_app/mainWindowUtils.h" 
#include <QDebug> 
#include "Transition.h" 
#include "Machine.h" 
 
 State::State(const std::string& name, const std::string& action, int stateId)
     : stateName(name), stateOutput(action), stateId(stateId) {
     
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
        return QPointF();
    }

    for (QGraphicsItem* child : stateItemGroup->childItems()) {
        if (QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(child)) {
            
            return stateItemGroup->mapToScene(ellipse->rect().center());
        }
    }

   
    qWarning() << "MainWindow::getVisualCenterOfStateItem_static: Ellipse not found in state group for item at scene pos"
               << stateItemGroup->scenePos() << ". Falling back to sceneBoundingRect().center().";
    return getVisualCenterOfStateItem_static(stateItemGroup);
}

void State::updateTransitionPositions(QGraphicsScene* scene, QGraphicsItemGroup* stateGroup, Machine *machine) {
    
    //qDebug() << "----------------------------------------------------------------";
    //qDebug() << "move method called for state:" << QString::fromStdString(stateName);
    //qDebug() << "----------------------------------------------------------------";

    //printTransitions();
    //qDebug() << "----------------------------------------------------------------";

    for (const auto& [id, transition] : outgoingTransitions) {
        Transition *trans = machine->getTransition(id);
        State *targetState = trans->getTargetState();
        std::string condition = trans->getCondition();

       
        QGraphicsItemGroup* group = std::get<0>(transition);

        const QPointF startPos = getVisualCenterOfStateItem_static(stateGroup);
        const QPointF endPos = std::get<2>(transition);

        //qDebug() << "StartPos:" << startPos;
        //qDebug() << "EndPos:" << endPos;

        if (group && scene->items().contains(group)) {
            scene->removeItem(group);
        } else if (!group) {
            qDebug() << "Group is null. Cannot remove.";
        } else {
            qDebug() << "Group not found in the scene. Cannot remove.";
        }

        QPointF actualStartPos;
        QPointF actualEndPos;

        QGraphicsItemGroup* newTransition = MainWindowUtils::drawArrow(startPos, endPos, QString::fromStdString(condition), id, scene, &actualStartPos, &actualEndPos);
        if (newTransition) {
            outgoingTransitions[id] = std::make_tuple(newTransition, actualStartPos, actualEndPos);
            targetState->setIncomingTransitionGroup(id, newTransition, actualStartPos, actualEndPos);
            //qDebug() << "Outgoing transition state updated";
            
        }
    }

   

    for (const auto& [id, transition] : incomingTranstions) {
        Transition *trans = machine->getTransition(id);
        std::string condition = trans->getCondition();
        State *sourceState = trans->getSourceState();

        QGraphicsItemGroup* group = std::get<0>(transition);
       

        const QPointF startPos = std::get<1>(transition);
        const QPointF endPos = getVisualCenterOfStateItem_static(stateGroup);

        scene->removeItem(group);

        

        QPointF actualStartPos;
        QPointF actualEndPos;

        QGraphicsItemGroup* newTransition = MainWindowUtils::drawArrow(startPos, endPos, QString::fromStdString(condition), id, scene, &actualStartPos, &actualEndPos);
        if (newTransition) {
            incomingTranstions[id] = std::make_tuple(newTransition, actualStartPos, actualEndPos);
           
            sourceState->setOutgoingTransitionGroup(id, newTransition, actualStartPos, actualEndPos);

            //qDebug() << "Incoming transition state udpdated";
            
        }
    }
}

 
void State::addIncomingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
    if (incomingTranstions.find(transitionGroup->data(1).toInt()) != incomingTranstions.end()) {
        qDebug() << "Incoming transition with ID" << transitionGroup->data(0).toInt() << "already exists. No new record created.";
        return;
    }
    incomingTranstions.insert({transitionGroup->data(1).toInt(), std::make_tuple(transitionGroup, data1, data2)});
}

void State::addOutgoingTransitionGroup(QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
    if (outgoingTransitions.find(transitionGroup->data(1).toInt()) != outgoingTransitions.end()) {
        qDebug() << "Outgoing transition with ID" << transitionGroup->data(0).toInt() << "already exists. No new record created.";
        return;
    }
    outgoingTransitions.insert({transitionGroup->data(1).toInt(), std::make_tuple(transitionGroup, data1, data2)});
}

void State::setIncomingTransitionGroup(int transitionId, QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
    
    incomingTranstions[transitionId] = std::make_tuple(transitionGroup, data1, data2);

}

void State::setOutgoingTransitionGroup(int transitionId, QGraphicsItemGroup* transitionGroup, const QPointF& data1, const QPointF& data2) {
    
    outgoingTransitions[transitionId] = std::make_tuple(transitionGroup, data1, data2);
    
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


 