#pragma once

#include <QWidget>
#include <QVector>
#include <QString>

struct SignalDrawingInfo         // shared values across drawing functions 
{
    int titleHeight;
    QRect signalRect;
    int leftX;
    int bottomY;
    double scaleX;
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

    SignalDrawingInfo signalElements;
    QPixmap drawingCache;            // off-screen (not directly visible) buffer

public:
    SignalWidget();
    void parseData(const QString &filepath);

protected:                                           // protected in QWidget class so it must be protected or public in the derived class
    void paintEvent(QPaintEvent *event) override;
};