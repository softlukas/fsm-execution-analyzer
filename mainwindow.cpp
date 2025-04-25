/**
 * @file MainWindow.cpp
 * @brief Implementation of the MainWindow class.
 * @authors Your Authors (xname01, xname02, xname03)
 * @date 2025-04-25
 */

#include "MainWindow.h"
#include "ui_mainwindow.h" // Include the header generated from mainwindow.ui

#include <QGraphicsEllipseItem> // Include for drawing ellipses
#include <QBrush>              // To set fill color
#include <QPen>                // To set outline color/style

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) // Create the UI object
{
    ui->setupUi(this); // Set up the UI defined in the .ui file

    // --- Initial setup ---
    setWindowTitle("IFA Automaton Tool");

    // Create the graphics scene
    scene = new QGraphicsScene(this); // 'this' makes MainWindow the parent for memory management

    // Set the scene for the graphics view defined in the .ui file
    // Make sure the QGraphicsView in your .ui file has the objectName "graphicsView"
    ui->graphicsView->setScene(scene);

    // Optional: Set scene boundaries or background
    // scene->setSceneRect(-200, -150, 400, 300); // Example rectangle
    // ui->graphicsView->setBackgroundBrush(Qt::lightGray);

    // Connect signals and slots manually (alternative to auto-connection)
    // connect(ui->addStateButton, &QPushButton::clicked, this, &MainWindow::on_addStateButton_clicked);
    // Auto-connection based on objectName should work if names match.
}

MainWindow::~MainWindow() {
    // scene is deleted automatically because MainWindow is its parent
    delete ui; // Clean up the UI object
}

// --- Slot Implementation ---

void MainWindow::on_addStateButton_clicked() {
    // This function is called when ui->addStateButton is clicked

    // 1. Define properties for the new state ellipse
    qreal x = 0; // Position X (relative to scene origin)
    qreal y = 0; // Position Y
    qreal width = 50;
    qreal height = 50;
    // Let's add states slightly offset so they don't overlap perfectly
    // A simple counter or random position could be used.
    // For now, just use a fixed offset based on number of items.
    int itemCount = scene->items().count();
    x = (itemCount % 5) * (width + 10); // Example positioning logic
    y = (itemCount / 5) * (height + 10);

    // 2. Create the ellipse item
    QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(x, y, width, height);

    // 3. Set appearance (optional)
    ellipse->setBrush(QBrush(Qt::cyan)); // Fill color
    ellipse->setPen(QPen(Qt::black));    // Outline color

    // 4. Make the item movable and selectable (optional)
    ellipse->setFlag(QGraphicsItem::ItemIsMovable);
    ellipse->setFlag(QGraphicsItem::ItemIsSelectable);

    // 5. Add the ellipse to the scene
    scene->addItem(ellipse);

    // Optional: Log to console or status bar
    qDebug("Add State button clicked, ellipse added."); // Requires #include <QDebug>
    // ui->statusbar->showMessage("State added.", 2000); // Show message for 2 seconds
}

