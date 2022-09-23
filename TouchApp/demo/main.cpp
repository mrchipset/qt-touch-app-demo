#include "MainWindow.h"

#include <QApplication>
#include "TouchEventDispatcher.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TouchEventDispatcher dispatcher;
    MainWindow w;
    w.setAttribute(Qt::WA_AcceptTouchEvents);
    w.show();
    w.installEventFilter(&dispatcher);
    return a.exec();
}
