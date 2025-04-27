#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItemGroup> // Needed for pointer type
#include "Machine.h"        // Include Automaton header
#include <memory>             // For std::unique_ptr
// #include "GraphicsView.h" // Include the custom view header - uncomment if using custom GraphicsView

// Forward declaration for the UI class (generated from .ui file)
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Forward declaration for the custom GraphicsView if not included above
class GraphicsView;

/**
 * @brief The main window of the application.
 * Handles UI interactions, manages the graphics scene, and interacts
 * with the Automaton data model.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT // Required for signals and slots

public:
    /**
     * @brief Constructor.
     * @param parent Optional parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot executed when the 'Add State' button (addStateButton) is clicked.
     * Prompts user for state details and adds the state.
     */
    void on_addStateButton_clicked();

    /**
     * @brief Slot executed when the 'File -> Save' menu action (actionSave) is triggered.
     * Saves the current automaton definition to a file.
     */
    void on_actionSave_triggered(); // Keep this slot declaration

    /**
     * @brief Slot executed when the 'Add Transition' button (addTransitionButton) is clicked.
     * Initiates the process of adding a transition by waiting for state clicks.
     */
    void on_addTransitionButton_clicked();

    /**
     * @brief Slot connected to the GraphicsView's stateItemClicked signal (if using custom view)
     * or connected manually to scene selection changes.
     * Handles the logic for selecting start and end states for a transition.
     * @param item The QGraphicsItemGroup (state) that was clicked/selected, or nullptr.
     */
    void handleStateClick(QGraphicsItemGroup *item, QGraphicsLineItem *lineItem); // Slot to handle state clicks/selection
   

    void on_addVariableButton_clicked();
    void on_addInputButton_clicked();
    void on_addOutputButton_clicked();
    void on_saveJsonButton_clicked();
    void on_loadJsonButton_clicked();


    // If NOT using custom GraphicsView with stateItemClicked signal,
    // use this slot connected to scene's selectionChanged signal instead:
    // void on_scene_selectionChanged();


    


private:
    /**
     * @brief Visually highlights a state item.
     * @param item The state item (group) to highlight.
     */
    void highlightItem(QGraphicsItemGroup* item);

    /**
     * @brief Removes visual highlighting from a state item.
     * @param item The state item (group) to unhighlight.
     */
    void unhighlightItem(QGraphicsItemGroup* item);
    /**
     * @brief Draws an arrow between two state items.
     * @param start The starting state item.
     * @param end The ending state item.
     */
    void drawArrow(const QGraphicsItemGroup *start, const QGraphicsItemGroup *end, const QString &label);

   
    /**
     * @brief Displays a dialog with the specified text.
     * 
     * This function opens a dialog window and displays the provided text to the user.
     * It can be used to show messages, warnings, or other information.
     * 
     * @param textToDisplay The text to be displayed in the dialog.
     * @return A string containing the user's response or an empty string if no response is provided.
     */
    std::string displayDialog(const std::string& textToDisplay);

    //void createTransition(const std::string& startStateName, const std::string& endStateName);
    


    Ui::MainWindow *ui;    // Pointer to the UI elements defined in mainwindow.ui
    QGraphicsScene *scene; // The scene where graphics items will be drawn
    std::unique_ptr<Machine> currentAutomaton; // The backend automaton data model
    // int stateCounter = 0; // Counter for positioning, keep if needed

    // --- Variables for Transition Drawing ---
    QGraphicsItemGroup *startItemForTransition = nullptr; // Stores the starting state item when adding a transition
    QGraphicsItemGroup *endItemForTransition = nullptr;   // Stores the ending state item when adding a transition
    bool addingTransitionMode = false; // Flag to indicate if the next click selects a start/end state for a transition
    Machine *machine = nullptr; // Pointer to the machine instance (if needed)
    State *startStateForTransition = nullptr; // Pointer to the start state (if needed)
    State *endStateForTransition = nullptr;   // Pointer to the end state (if needed)
    int objectTransitionId = 0;
};

#endif // MAINWINDOW_H
