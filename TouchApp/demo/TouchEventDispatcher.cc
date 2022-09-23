#include "TouchEventDispatcher.h"
#include <QDebug>
#include <QApplication>

TouchEventDispatcher::TouchEventDispatcher(QObject *parent) : QObject(parent)
{

}

bool TouchEventDispatcher::eventFilter(QObject *watched, QEvent *event)
{
//    if (object == this) {
//            qDebug() << "event->type():" << event->type();
        switch(event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchCancel:
        case QEvent::TouchEnd:
        {
//            qDebug() << "One event";
            QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
            if (touchEvent) {

                auto touchPoints = touchEvent->touchPoints();
                for (auto& point : touchPoints) {
                    switch(point.state()) {
                    case Qt::TouchPointPressed:
                        processTouchPointPressEvent(&point);
                        break;
                    case Qt::TouchPointMoved:
                        processTouchPointMoveEvent(&point);
                        break;
                    case Qt::TouchPointReleased:
                        processTouchPointReleaseEvent(&point);
                        break;
                    case Qt::TouchPointStationary:
                        processTouchPointStationEvent(&point);
                        break;
                    }
                }
                if (!mTouchPoints.isEmpty()) {
                    mTouchEvent = new QTouchEvent(touchEvent->type(),
                                                  touchEvent->device(),
                                                  touchEvent->modifiers());
                    mTouchEvent->setTouchPoints(mTouchPoints.values());
                    mTouchEvent->setTouchPointStates(getTouchPointStates());
                    qDebug() << mTouchPoints.values();
                    mTouchPoints.clear();
                    bool ret = QApplication::sendEvent(watched, mTouchEvent);
                    qDebug() << ret;
                    return true;
                }
            }

            return false;
        }
            break;
        default:
            break;
        }
//    }
        return QObject::eventFilter(watched, event);
}

void TouchEventDispatcher::processTouchPointPressEvent(QTouchEvent::TouchPoint *point)
{
    if (point->uniqueId().numericId() >= 0) {
        return;
    }
    int id = findId(point);
    if (id < 0) {
        // only add no data;
        id = createMapping(point);
        QTouchEvent::TouchPoint touchPoint(*point);
        touchPoint.setId(id);
        touchPoint.setUniqueId(id);
        mTouchPoints.insert(id, touchPoint);
    }

}

void TouchEventDispatcher::processTouchPointReleaseEvent(QTouchEvent::TouchPoint *point)
{
    if (point->uniqueId().numericId() >= 0) {
        return;
    }

    int id = findId(point);
    if (id < 0) {
        return;
    } else {
        // only add existed data
        mTouchMap.removeAt(id);
        QTouchEvent::TouchPoint touchPoint(*point);
        touchPoint.setId(id);
        touchPoint.setUniqueId(id);
        if (mTouchPoints.contains(id)) {
            mTouchPoints[id] = touchPoint;
        } else {
            mTouchPoints.insert(id, touchPoint);
        }
    }
}

void TouchEventDispatcher::processTouchPointMoveEvent(QTouchEvent::TouchPoint *point)
{
    if (point->uniqueId().numericId() >= 0) {
        return;
    }
    int id = findId(point);
    if (id < 0) {
        return;
    } else {
        // only add existed data
        QTouchEvent::TouchPoint touchPoint(*point);
        touchPoint.setId(id);
        touchPoint.setUniqueId(id);
        if (mTouchPoints.contains(id)) {
            mTouchPoints[id] = touchPoint;
        } else {
            mTouchPoints.insert(id, touchPoint);
        }
    }
}

void TouchEventDispatcher::processTouchPointStationEvent(QTouchEvent::TouchPoint *point)
{
    if (point->uniqueId().numericId() > 0) {
        return;
    }
    int id = findId(point);
    if (id < 0) {
        return;
    } else {
        // only add existed data
        QTouchEvent::TouchPoint touchPoint(*point);
        touchPoint.setId(id);
        touchPoint.setUniqueId(id);
        if (mTouchPoints.contains(id)) {
            mTouchPoints[id] = touchPoint;
        } else {
            mTouchPoints.insert(id, touchPoint);
        }
    }
}

int TouchEventDispatcher::findId(QTouchEvent::TouchPoint *point)
{
    constexpr double THRESHOLD = 1.0;
    if (mTouchMap.isEmpty()) {
        return -1;
    } else {
        for (auto it = mTouchMap.begin(); it != mTouchMap.end(); ++it) {
            if (point->state() == Qt::TouchPointPressed) {
                if (checkPointsTooNear(point->pos(), it->pressId1Pos, THRESHOLD)) {
                    it->pressId2 = point->id();
                    it->pressId2Pos = point->pos();
                    return it->id;
                }
            }
            else {
                if (point->id() == it->pressId1 || point->id() == it->pressId2) {
                    return it->id;
                }
            }
        }
        return -1;
    }
}

bool TouchEventDispatcher::checkPointsTooNear(const QPointF &pt1, const QPointF &pt2, const double threshold)
{
    return qAbs(pt1.x() - pt2.x()) < threshold && qAbs(pt1.y() - pt2.y()) < threshold;
}

int TouchEventDispatcher::createMapping(QTouchEvent::TouchPoint *point)
{
    TouchPointDispactherMapping mapping;
    mapping.id = mTouchMap.size();
    mapping.pressId1 = point->id();
    mapping.pressId1Pos = point->pos();
    mTouchMap.append(mapping);
    return mapping.id;
}

Qt::TouchPointStates TouchEventDispatcher::getTouchPointStates()
{
    Qt::TouchPointStates states;
    for (auto it = mTouchPoints.begin(); it != mTouchPoints.end(); ++it) {
        states.setFlag(it.value().state());
    }
    return states;
}


