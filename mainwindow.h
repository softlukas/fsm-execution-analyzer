#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene> // Include for the graphics scene

// Forward declaration for the UI class generated from .ui file
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT // Macro required for classes using signals and slots

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * @brief Slot executed when the 'Add State' button is clicked.
     * The name follows the convention on_<objectName>_<signalName>().
     */
    void on_addStateButton_clicked(); // Slot for the button click

private:
    Ui::MainWindow *ui; // Pointer to the UI definition
    QGraphicsScene *scene; // Pointer to the graphics scene where items will be drawn
    // std::unique_ptr<Automaton> currentAutomaton; // Keep this for later
};

#endif // MAINWINDOW_H
