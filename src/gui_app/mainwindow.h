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

    void handleVariableValueEdited(); // Spoločný slot pre všetky QLineEdits

    void handleInputValueEdited(); // Spoločný slot pre všetky QLineEdits
   
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

    void on_runAutomatButton_clicked();
    void on_terminateAutomatButton_clicked();
    void on_connectButton_clicked();
    void on_renameButton_clicked();
    

    QGraphicsItemGroup* drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene, QVariant *actualStartPos, QVariant *actualEndPos);


    // If NOT using custom GraphicsView with stateItemClicked signal,
    // use this slot connected to scene's selectionChanged signal instead:
    // void on_scene_selectionChanged();

    void processPendingDatagrams(); // Slot na spracovanie UDP správ

    


private:

    std::string portGUI;
    std::string portAutomat;
    QUdpSocket *guiSocket_ = nullptr;

    bool waitingForAutomatonInfo = false; // Flag for connection state
    QString connectedAutomatonName = "";   // Store name once connected
    QTimer *connectionTimeoutTimer = nullptr; // Optional: For timeout

    // Definuj farby (zostáva)
    const QColor normalStateColor = Qt::cyan;
    const QColor activeStateColor = Qt::green;

    void updateStateItemColor(QGraphicsItemGroup* item, const QColor& color);

    void updateVariableDisplay(const std::string& varName, const std::string& newValue);
    void updateOutputDisplay(const std::string& outputName, const std::string& newValue);


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
    


    void editTransition();

    void editState(QGraphicsItemGroup *item);


    void addVarRowToGUI(const std::string& name, const std::string& type, const std::string& value);

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

    void CreateVarFromUserInput(const std::string& userInput);

    void ProccessMultipleArgsInputEditDialog(const std::string& title, const std::string& label1Text, const std::string& label2Text, 
        std::string& predefinedInput1, std::string& predefinedInput2, 
        std::string* newInput1, std::string* newInput2);

    void setElipseText(QGraphicsEllipseItem* ellipseItem, const std::string& text);

    //void createTransition(const std::string& startStateName, const std::string& endStateName);
    
    void bindGuiSocket();

    void clearVariableList();
    void clearInputList();
    void clearOutputList();   // ak používaš
    void redrawAutomatonFromModel();
    void populateUIFromModel();

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
    int objectStateId = 0; // Counter for unique transition IDs
};

#endif // MAINWINDOW_H
