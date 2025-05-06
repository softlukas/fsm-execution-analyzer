#include <string>
#include <QGraphicsItemGroup>

class MainWindowUtils {


public: 
    static std::string ProccessOneArgumentDialog(const std::string& textToDisplay);

    static QGraphicsItemGroup* drawArrow(const QPointF &startPos, const QPointF &endPos, const QString &label, int transitionId, QGraphicsScene *scene, QPointF *actualStartPos, QPointF *actualEndPos);

    //static static QPointF adjustPointToEdge(const QGraphicsItemGroup* stateGroup, const QPointF& targetCenter, const QPointF& otherPoint, qreal padding = 5.0)
    static QGraphicsItemGroup* findItemGroupByIdAndType(QGraphicsScene* scene, int id, const QString& type);
};