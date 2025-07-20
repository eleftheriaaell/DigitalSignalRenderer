#pragma once

#include <QWidget>
#include <QVector>
#include <QString>
#include <QWheelEvent>

struct SignalDrawingInfo         // shared values across drawing functions 
{
    int titleHeight;
    QRect signalRect;
    int leftX;
    int bottomY;
    double scaleX;
    double zoom = 1.0;            // zoom feature, default zoom
    double offsetX = 0.0;         // horizontal offset, updated by user interactions
};                                     

class SignalWidget : public QWidget                      // custom widget, inherits from QWidget for GUI rendering
{
    QVector<int> signalData;           // waveform data 
    int timestep;
    QString units;
    QString filename;

    void drawTitle(QPainter &painter);
    void drawSignalRect(QPainter &painter);
    void drawSignal(QPainter &painter);
    void drawTimeLabels(QPainter &painter);
    void resetView();

    SignalDrawingInfo signalElements;
    QPixmap drawingCache;            // off-screen (not directly visible) buffer

    bool dragging = false;
    QPoint lastDragPos;       // last mouse position while dragging

public:
    SignalWidget();
    void parseData(const QString &filepath);

protected:                                           // protected in QWidget class so it must be protected or public in the derived class
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
};