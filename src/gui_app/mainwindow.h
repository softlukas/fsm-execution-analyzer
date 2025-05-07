/**
 * @file mainwindow.h
 * @brief Header file for the MainWindow class.
 * @details This file declares the MainWindow class, which serves as the main GUI interface
 * for the application. It manages user interactions, the graphics scene, and communication
 * with the backend automaton model.
 * @authors xsimonl00, xsiaket00
 * @date Last modified: 2025-05-05
 */



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsItemGroup> // Needed for pointer type
#include "core/Machine.h"        // Include Automaton header
#include <memory>             // For std::unique_ptr
// #include "GraphicsView.h" // Include the custom view header - uncomment if using custom GraphicsView
#include <QUdpSocket> // Potrebný include
#include <QMap>
#include <QColor> // Pre farby
#include <QGroupBox>

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

    /**
     * @brief Calculates the visual center of a given QGraphicsItemGroup.
     * 
     * This static method determines the visual center of the provided 
     * QGraphicsItemGroup, which represents a group of graphical items 
     * in a QGraphicsScene. The visual center is computed based on the 
     * bounding rectangle of the group.
     * 
     * @param stateItemGroup A pointer to the QGraphicsItemGroup whose 
     * visual center is to be calculated. Must not be null.
     * 
     * @return The visual center of the QGraphicsItemGroup as a QPointF.
     */
    static QPointF getVisualCenterOfStateItem_static(QGraphicsItemGroup* stateItemGroup);

private slots:
    /**
     * @brief Slot executed when the 'Add State' button (addStateButton) is clicked.
     * Prompts user for state details and adds the state.
     */
    void on_addStateButton_clicked();

    /**
     * @brief Handles the event when the return/enter key is pressed in an input field.
     * 
     * This function is triggered when the user presses the return/enter key while
     * interacting with a specific input field. It processes the input and performs
     * the necessary actions based on the application's logic.
     */
    void handleInputReturnPressed();
    /**
     * @brief Slot executed when the 'Edit State' button (editStateButton) is clicked.
     * Prompts user for state details and updates the state.
     */
    void handleVariableValueEdited();

    

    
   
    /**
     * @brief Slot executed when the 'Add Transition' button (addTransitionButton) is clicked.
     * Initiates the process of adding a transition by waiting for state clicks.
     */
    void on_addTransitionButton_clicked();

    /**
     * @brief Slot executed when the 'Delete State' button (deleteStateButton) is clicked.
     * Deletes the selected state from the automaton.
     * @param item The state item to be deleted.
     * @param lineItem The line item representing the transition to be deleted.
     */

    void handleStateClick(QGraphicsItemGroup *item, QGraphicsLineItem *lineItem); 
   
    /**
     * @brief Slot executed when the 'Delete Transition' button (deleteTransitionButton) is clicked.
     * Deletes the selected transition from the automaton.
     * @param item The transition item to be deleted.
     * @param lineItem The line item representing the transition to be deleted.
     */
    void handleRightClick(QGraphicsItemGroup *item, QGraphicsLineItem *lineItem);

    /**
     * @brief Slot triggered when the "Add Variable" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Add Variable" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_addVariableButton_clicked();
    /**
     * @brief Slot triggered when the "Add Input" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Add Input" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_addInputButton_clicked();
    /**
     * @brief Slot triggered when the "Add Output" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Add Output" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_addOutputButton_clicked();
    /**
     * @brief Slot triggered when the "Delete Variable" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Delete Variable" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_saveJsonButton_clicked();
    /**
     * @brief Slot triggered when the "Delete Input" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Delete Input" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_loadJsonButton_clicked();
    /**
     * @brief Slot triggered when the "Delete Output" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Delete Output" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_runAutomatButton_clicked();
    /**
     * @brief Slot triggered when the "Terminate Automaton" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Terminate Automaton" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_terminateAutomatButton_clicked();
    /**
     * @brief Slot triggered when the "Connect" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Connect" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_connectButton_clicked();
    /**
     * @brief Slot triggered when the "Disconnect" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Disconnect" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_renameButton_clicked();
    /**
     * @brief Slot triggered when the "Set Initial State" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Set Initial State" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void on_setInitialStateButton_clicked();

    /**
     * @brief Slot triggered when the "Disconnect" button is clicked.
     * 
     * This function handles the logic to be executed when the user interacts
     * with the "Disconnect" button in the GUI. It is connected to the button's
     * clicked() signal.
     */
    void processPendingDatagrams();

    


private:

    /**
     * @brief Represents the color used for the normal state in the application.
     * 
     * This constant defines the default color for the normal state, 
     * which is set to cyan using the QColor class from the Qt framework.
     */
    const QColor normalStateColor = Qt::cyan;
    
    /**
     * @brief Represents the color used to indicate the active state in the application.
     *
     * This constant defines the color as green using the Qt framework's QColor class.
     * It is used to visually represent an active state in the GUI.
     */
    const QColor activeStateColor = Qt::green;

    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */
    std::string portGUI;
    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */
    std::string portAutomat;
    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */
    QUdpSocket *guiSocket_ = nullptr;
    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */
    QList<QMetaObject::Connection> stateMoveConnections;
    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */
    bool waitingForAutomatonInfo = false;
    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */ 
    QString connectedAutomatonName = "";
    /**
     * @brief Initializes the GUI components and sets up the graphics scene.
     */  
    /**
     * @brief Timer for handling connection timeouts.
     * 
     * This timer is used to detect and handle connection timeouts
     * when attempting to establish or maintain communication with
     * the automaton.
     */
    QTimer *connectionTimeoutTimer = nullptr;

    /**
     * @brief Updates the display of a specific output with a new value.
     * 
     * This function is used to update the GUI display for a given output
     * identified by its name. It replaces the current value with the provided
     * new value.
     * 
     * @param outputName The name of the output to be updated.
     * @param newValue The new value to display for the specified output.
     */
    void updateOutputDisplay(const std::string& outputName, const std::string& newValue);


    /**
     * @brief Updates the color of a specified QGraphicsItemGroup.
     * 
     * This function changes the color of all items within the given 
     * QGraphicsItemGroup to the specified QColor. It is typically used 
     * to visually indicate a change in state or status of the item group.
     * 
     * @param item A pointer to the QGraphicsItemGroup whose items' colors 
     *             will be updated.
     * @param color The QColor to apply to the items in the group.
     */
    void updateStateItemColor(QGraphicsItemGroup* item, const QColor& color);

    /**
     * @brief Resets the layout of the specified QGroupBox.
     * 
     * This function removes all widgets and layouts from the given QGroupBox,
     * effectively clearing its contents and resetting it to an empty state.
     * 
     * @param groupBox A pointer to the QGroupBox whose layout needs to be reset.
     */
    void resetGroupBoxLayout(QGroupBox* groupBox);

    /**
     * @brief Updates the display of a variable in the GUI.
     * 
     * This function updates the displayed value of a variable identified by its name.
     * It is typically used to reflect changes in the variable's value in the user interface.
     * 
     * @param varName The name of the variable to update.
     * @param newValue The new value to display for the variable.
     */
    void updateVariableDisplay(const std::string& varName, const std::string& newValue);

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
    


    /**
     * @brief Edits the transition represented by the specified graphics item group.
     * 
     * This function allows modifications to a transition object that is visually
     * represented by a QGraphicsItemGroup. The specific details of the transition
     * and how it is edited depend on the implementation.
     * 
     * @param item A pointer to the QGraphicsItemGroup representing the transition to be edited.
     */
    void editTransition(QGraphicsItemGroup *item);

    /**
     * @brief Edits the state of the specified QGraphicsItemGroup.
     * 
     * This function allows modifications to the state or properties of the given
     * QGraphicsItemGroup. The exact behavior depends on the implementation details.
     * 
     * @param item A pointer to the QGraphicsItemGroup to be edited.
     */
    void editState(QGraphicsItemGroup *item);


    /**
     * @brief Adds a new variable row to the GUI.
     * 
     * This function dynamically creates and adds a new row to the GUI
     * to represent a variable with the specified name, type, and value.
     * 
     * @param name The name of the variable to be displayed.
     * @param type The type of the variable (e.g., int, string, etc.).
     * @param value The initial value of the variable.
     */
    void addVarRowToGUI(const std::string& name, const std::string& type, const std::string& value);

    /**
     * @brief Adds an input row to the GUI with the specified name.
     * 
     * This function dynamically creates and adds a new input row to the
     * graphical user interface. The input row is labeled with the provided
     * name, allowing users to interact with it.
     * 
     * @param name The name to label the input row.
     */
    void addInputRowToGUI(const std::string& name);
   
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

    /**
     * @brief Creates a variable based on the user-provided input.
     * 
     * This function processes the input string provided by the user and 
     * creates a corresponding variable. The exact behavior and type of 
     * the variable depend on the implementation details and the format 
     * of the input string.
     * 
     * @param userInput A string containing the user's input to define the variable.
     */
    void CreateVarFromUserInput(const std::string& userInput);

    /**
     * @brief Opens a dialog to process multiple input arguments with predefined and new values.
     * 
     * This function displays a dialog with two input fields, allowing the user to edit or provide
     * new values for the inputs. The dialog is customized with a title and labels for the input fields.
     * 
     * @param title The title of the dialog window.
     * @param label1Text The label text for the first input field.
     * @param label2Text The label text for the second input field.
     * @param predefinedInput1 The predefined value for the first input field (modifiable reference).
     * @param predefinedInput2 The predefined value for the second input field (modifiable reference).
     * @param newInput1 Pointer to store the new value entered for the first input field (can be null).
     * @param newInput2 Pointer to store the new value entered for the second input field (can be null).
     */
    void ProccessMultipleArgsInputEditDialog(const std::string& title, const std::string& label1Text, const std::string& label2Text, 
        std::string& predefinedInput1, std::string& predefinedInput2, 
        std::string* newInput1, std::string* newInput2);

    void setElipseText(QGraphicsEllipseItem* ellipseItem, const std::string& text);

    
    
    /**
     * @brief Establishes the connection between the GUI and the underlying socket.
     * 
     * This function is responsible for binding the GUI components to the socket
     * interface, enabling communication between the graphical user interface and
     * the backend or network layer. It ensures that the necessary signals and slots
     * are connected for seamless data exchange.
     */
    void bindGuiSocket();

    /**
     * @brief Clears all widgets and items from the given layout.
     * 
     * This function iterates through all the items in the specified layout,
     * removes them, and deletes the associated widgets to free memory.
     * 
     * @param layout A pointer to the QLayout to be cleared. Must not be nullptr.
     */
    void clearLayout(QLayout *layout);
    /**
     * @brief Clears the variable list in the GUI.
     * 
     * This function removes all variable-related widgets and items from the GUI,
     * effectively resetting the variable list to its initial state.
     */
    void clearVariableList();
    /**
     * @brief Clears the input list in the GUI.
     * 
     * This function removes all input-related widgets and items from the GUI,
     * effectively resetting the input list to its initial state.
     */
    void clearInputList();
    /**
     * @brief Clears the output list in the GUI.
     * 
     * This function removes all output-related widgets and items from the GUI,
     * effectively resetting the output list to its initial state.
     */
    void clearOutputList();
    /**
     * @brief Clears the state list in the GUI.
     * 
     * This function removes all state-related widgets and items from the GUI,
     * effectively resetting the state list to its initial state.
     */
    void redrawAutomatonFromModel();
    /**
     * @brief Clears the state list in the GUI.
     * 
     * This function removes all state-related widgets and items from the GUI,
     * effectively resetting the state list to its initial state.
     */
    void populateUIFromModel();
    /**
     * @brief Clears the transition list in the GUI.
     * 
     * This function removes all transition-related widgets and items from the GUI,
     * effectively resetting the transition list to its initial state.
     */
    void setInputFieldsEnabled(bool enabled);

    /**
     * @brief Pointer to the user interface object for the MainWindow.
     * 
     * This member is automatically generated by the Qt framework and is used
     * to access and manipulate the UI elements defined in the associated .ui file.
     */
    Ui::MainWindow *ui;
    /**
     * @brief Pointer to the graphics view used for displaying the automaton.
     * 
     * This member is used to manage the graphical representation of the automaton
     * and handle user interactions with the graphics scene.
     */    
    QGraphicsScene *scene;
    /**
     * @brief Pointer to the graphics view used for displaying the automaton.
     * 
     * This member is used to manage the graphical representation of the automaton
     * and handle user interactions with the graphics scene.
     */ 
    std::unique_ptr<Machine> currentAutomaton; 
    

    // --- Variables for Transition Drawing ---
    /**
     * @brief Pointer to the graphics item group representing the start state of a transition.
     * 
     * This member is used to identify the starting point of a transition in the graphics scene.
     */
    QGraphicsItemGroup *startItemForTransition = nullptr;
    /**
     * @brief Pointer to the graphics item group representing the end state of a transition.
     * 
     * This member is used to identify the ending point of a transition in the graphics scene.
     */ 
    QGraphicsItemGroup *endItemForTransition = nullptr;
    /**
     * @brief Pointer to the graphics line item representing the transition line.
     * 
     * This member is used to visually represent the transition line in the graphics scene.
     */ 
    bool addingTransitionMode = false;

    /**
     * @brief Pointer to the Machine object.
     * 
     * This pointer is used to reference the Machine instance associated with
     * the main window. It is initialized to nullptr by default and should be
     * properly assigned before use to avoid dereferencing a null pointer.
     */
    Machine *machine = nullptr;
    /**
     * @brief Pointer to the starting state for a transition.
     * 
     * This member is used to reference the starting state of a transition
     * in the automaton. It helps in identifying the source state when
     * creating or managing transitions in the graphics scene.
     */
    State *startStateForTransition = nullptr;
    /**
     * @brief Pointer to the ending state for a transition.
     * 
     * This member is used to reference the ending state of a transition
     * in the automaton. It helps in identifying the destination state when
     * creating or managing transitions in the graphics scene.
     */
    State *endStateForTransition = nullptr; 
    /**
     * @brief Unique identifier for transitions in the graphics scene.
     * 
     * This member is used to assign a unique ID to each transition object
     * in the graphics scene, ensuring proper identification and management
     * of transitions within the automaton.
     */
    int objectTransitionId = 0;
    /**
     * @brief Unique identifier for states in the graphics scene.
     * 
     * This member is used to assign a unique ID to each state object
     * in the graphics scene, ensuring proper identification and management
     * of states within the automaton.
     */
    int objectStateId = 0;
    
};

#endif // MAINWINDOW_H
