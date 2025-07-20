#include <gtest/gtest.h>
#include <QApplication>
#include "SignalWidget.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);   // app must exist before constructing widget

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();        // run all tests and exit
}

// widget construction test
TEST(SignalWidgetTests, WidgetConstruction)
{
    SignalWidget widget;    // widget creation
    SUCCEED();  
}

// valid file loading test
TEST(SignalWidgetTests, ParseValidFile)
{
    SignalWidget widget;
    widget.parseData("../data.txt");    // file parsing
    SUCCEED(); 
}

// invalid file loading test
TEST(SignalWidgetTests, ParseInvalidFileGracefully)
{
    SignalWidget widget;
    widget.parseData("non_existing_file.txt");  // handle missing file
    SUCCEED(); 
}

// widget creation test
TEST(SignalWidgetTests, CreateAndRepaint)
{
    for (int i = 0; i < 5; i++)
    {
        SignalWidget widget;   // create widget
        widget.repaint();      // trigger repaint
    }
    SUCCEED();
}

// repaint event test
TEST(SignalWidgetTests, RepaintStability)
{
    SignalWidget widget;
    for (int i = 0; i < 10; i++)
        widget.repaint();         // multiple repaints

    SUCCEED();  // stable repainting
}

// zoom scroll event test
TEST(SignalWidgetTests, ZoomEvent)
{
    SignalWidget widget;

    QWheelEvent event(
        QPointF(100, 50),                  // mouse position (local)
        QPointF(100, 50),                  // mouse position (global)
        QPoint(0, 0),                      // pixel scroll (unused)
        QPoint(0, 120),                    // vertical scroll amount
        Qt::NoButton,
        Qt::ControlModifier,               // Ctrl key pressed
        Qt::ScrollUpdate,
        false
    );

    QApplication::sendEvent(&widget, &event);

    SUCCEED();  // no crash on zoom scroll
}