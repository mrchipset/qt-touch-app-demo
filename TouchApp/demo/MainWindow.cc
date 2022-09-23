#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QTouchEvent>
#include <QGestureEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    installEventFilter(this);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *event)
{
    qDebug() << "event->type():" << event->type();
    switch(event->type()) {
    case QEvent::Gesture:
    {
        QGestureEvent* gestureEvent = dynamic_cast<QGestureEvent*>(event);
        for (auto gesture :gestureEvent->gestures()) {
            qDebug() << gesture->gestureType();

        }
    }
        break;
    default:
        break;
    }
    return QMainWindow::event(event);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
//    if (object == this) {
//            qDebug() << "event->type():" << event->type();
        switch(event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchCancel:
        case QEvent::TouchEnd:
        {
            qDebug() << "One event";
            QTouchEvent* touchEvent = dynamic_cast<QTouchEvent*>(event);
            if (touchEvent) {
                auto touchPoints = touchEvent->touchPoints();
                for (auto& point : touchPoints) {
                    qDebug() << point.uniqueId().numericId() << point.id() << point.state() << point.pos();
                }
            }
            return true;
        }
            break;
        default:
            break;
        }
//    }
    return QMainWindow::eventFilter(object, event);
}
