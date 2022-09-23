#ifndef TOUCHEVENTDISPATCHER_H
#define TOUCHEVENTDISPATCHER_H

#include <QObject>
#include <QTouchEvent>
#include <QTouchEvent>
#include <QMap>

class TouchEventDispatcher : public QObject
{
    Q_OBJECT
public:
    struct TouchPointDispactherMapping {
        int pressId1{-1};
        QPointF pressId1Pos;
        int pressId2{-1};
        QPointF pressId2Pos;
        int id{-1};
        bool isReleased1{false};
        bool isReleased2{false};
    };
public:
    explicit TouchEventDispatcher(QObject *parent = nullptr);

Q_SIGNALS:


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    QTouchEvent* mTouchEvent;
    QMap<int, QTouchEvent::TouchPoint> mTouchPoints;
    QList<TouchPointDispactherMapping> mTouchMap;
    void processTouchPointPressEvent(QTouchEvent::TouchPoint* point);
    void processTouchPointReleaseEvent(QTouchEvent::TouchPoint* point);
    void processTouchPointMoveEvent(QTouchEvent::TouchPoint* point);
    void processTouchPointStationEvent(QTouchEvent::TouchPoint* point);

    int findId(QTouchEvent::TouchPoint* point);
    bool checkPointsTooNear(const QPointF& pt1, const QPointF& pt2, const double threshold=1.0);
    int createMapping(QTouchEvent::TouchPoint* point);
    Qt::TouchPointStates getTouchPointStates();
};

#endif // TOUCHEVENTDISPATCHER_H
