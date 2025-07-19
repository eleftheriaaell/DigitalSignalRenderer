#include <QApplication>
#include "SignalWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SignalWidget widget;
    widget.show();                      // show widget on screen

    return app.exec();
}