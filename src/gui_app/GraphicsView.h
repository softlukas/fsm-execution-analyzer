#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsItemGroup> // For casting clicked items

/**
 * @brief Custom QGraphicsView to handle mouse clicks for adding transitions.
 * Emits a signal when a state item (QGraphicsItemGroup) is clicked.
 */
class GraphicsView : public QGraphicsView {
    Q_OBJECT // Required for signals and slots

public:
    /**
     * @brief Constructor.
     * @param parent Optional parent widget.
     */
    explicit GraphicsView(QWidget *parent = nullptr);
    /**
     * @brief Constructor allowing scene initialization.
     * @param scene The scene to associate with this view.
     * @param parent Optional parent widget.
     */
    explicit GraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);


signals:
    /**
     * @brief Signal emitted when a QGraphicsItemGroup representing a state is clicked.
     * @param item Pointer to the clicked QGraphicsItemGroup. Nullptr if background is clicked.
     */
    void stateItemClicked(QGraphicsItemGroup *item1, QGraphicsLineItem *item2);
    

protected:
    /**
     * @brief Reimplemented mouse press event handler.
     * Detects clicks on state items (QGraphicsItemGroup) or the background
     * and emits the stateItemClicked signal.
     * @param event The mouse event details.
     */
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event);
};

#endif // GRAPHICSVIEW_H
