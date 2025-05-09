/**
 * @file mainWindowUtils.h
 * @brief Header file for the MainWindowUtils class.
 * @details This file declares the MainWindowUtils class, which provides utility functions
 * for the main GUI application. These functions include dialog handling, arrow drawing,
 * and item group management within the graphics scene.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */

#include <string>
#include <QGraphicsItemGroup>

class MainWindowUtils {


public: 
    /**
     * @brief Displays a dialog to process a single argument and returns the result.
     * 
     * This static function presents a dialog box to the user with the specified 
     * text to display. The user can input a value, which is then returned as a string.
     * 
     * @param textToDisplay The text to display in the dialog box, providing context 
     *                      or instructions to the user.
     * @return std::string The user-provided input from the dialog box.
     */
    static std::string ProccessOneArgumentDialog(const std::string& textToDisplay);

    /**
     * @brief Draws an arrow between two points on a QGraphicsScene.
     * 
     * This function creates an arrow representation between the specified start and end positions,
     * optionally labeled with a text and associated with a transition ID. The arrow is added to the
     * provided QGraphicsScene and the actual start and end positions of the arrow can be retrieved
     * through the provided pointers.
     * 
     * @param startPos The starting position of the arrow.
     * @param endPos The ending position of the arrow.
     * @param label The text label to display on the arrow.
     * @param transitionId An identifier for the transition represented by the arrow.
     * @param scene The QGraphicsScene where the arrow will be drawn.
     * @param actualStartPos Pointer to store the actual starting position of the arrow (optional).
     * @param actualEndPos Pointer to store the actual ending position of the arrow (optional).
     * @return A pointer to the QGraphicsItemGroup representing the drawn arrow.
     */
    static QGraphicsItemGroup* drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene, QPointF *actualStartPos, QPointF *actualEndPos);

    
    static QGraphicsItemGroup* findItemGroupByIdAndType(QGraphicsScene* scene, int id, const QString& type);
};