/**
 * @file State.cpp
 * @brief Implementation of the State class.
 * @authors Your Authors (xname01, xname02, xname03) // !!! FILL IN YOUR NAMES/LOGINS !!!
 * @date 2025-04-24 // Current creation date
 */

 #include "State.h" // Include the header file
 #include <QGraphicsScene> // Include the QGraphicsScene header
 
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
 

 void State::updateTransitionPositions(QGraphicsScene* scene) {
     // Declare incomingTransitions as a placeholder for demonstration
     
 
     // Iterate through incoming transitions and update their positions
     for (const auto& [id, transition] : incomingTranstions) {
         QGraphicsItemGroup* group = std::get<0>(transition);
         QVariant data1 = std::get<1>(transition);
         QVariant data2 = std::get<2>(transition);
         // Update the position of the transition group based on the state position
         //group->setPos(scene->sceneRect().center());
         scene->removeItem(group);
     }
 
     // Iterate through outgoing transitions and update their positions
     for (const auto& [id, transition] : outgoingTransitions) {
         QGraphicsItemGroup* group = std::get<0>(transition);
         QVariant data1 = std::get<1>(transition);
         QVariant data2 = std::get<2>(transition);
         // Update the position of the transition group based on the state position
         //group->setPos(scene->sceneRect().center());
         scene->removeItem(group);
     }
 }