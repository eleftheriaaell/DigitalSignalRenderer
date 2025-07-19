#include "SignalWidget.h"

#include <QPainter>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QFont>

SignalWidget:: SignalWidget() : QWidget(nullptr)           // constructor with no parameter, widget is always a top-level window (no parent)
{
    parseData("../data.txt");
    setMinimumSize(800, 100);               // minimum size of window
};

void SignalWidget::parseData(const QString &filepath)         
{
    filename = QFileInfo(filepath).fileName();    // save just the file name, not full path
    
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))       // open file only for reading and in text mode (otherwise raw bytes)
    {
        qDebug() << "Cannot open file!";            
        return;
    }

    bool dataSet = false;        
    while(!file.atEnd())
    {
        QString line = file.readLine().trimmed();           // read line and remove leading/ending whitespace
        //qDebug() << line;

        if(line.startsWith("TIMESTEP;"))
        {
            QString value = line.mid(QString("TIMESTEP;").length());        // find substring after "TIMESTEP;" 
            timestep = (value.remove(';')).toInt();                         // remove ; and convert to integer
            //qDebug() << timestep;
        }
        else if(line.startsWith("UNITS;"))
        {
            units = line.mid(QString("UNITS;").length()).remove(';');        // find substring after "UNITS;" and remove ;
            //qDebug() << units;
        }
        else if(line == "DATA;")                   // line contains only "DATA;"
        {
            dataSet = true;
            continue;
        }
        else if(dataSet)
        {
            QChar c = line.at(0);               // find value at index 0
            if (c == '0')
                signalData.append(0);
            else if(c == '1')
                signalData.append(1);
        }
    }
}

void SignalWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);        // fill the whole widget with white

    drawTitle(painter);
    drawSignalRect(painter); 
    drawSignal(painter);  
    drawTimeLabels(painter);
}

void SignalWidget::drawTitle(QPainter &painter)
{
    signalElements.titleHeight = 30;                               
    QRect titleRect(0, 0, width(), signalElements.titleHeight);        // draw full-width title background
    painter.fillRect(titleRect, Qt::lightGray);         // light gray background
    painter.setFont(QFont("", 12, QFont::Bold));        // default font
    
    // draw title text left-aligned, vertically centered and add 10px padding from the left
    painter.drawText(titleRect.adjusted(10, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, filename);      
}

void SignalWidget::drawSignalRect(QPainter &painter)
{  
    int signalMargin = 20;                      // margin between main window and signal rectangle, applied on both left and right
    int signalTop = signalElements.titleHeight + 10;           // signal rectangle below the title
    int signalHeight = height() / 3;            // take 1/3 of the widget to display the signal

    QRect signalRect(signalMargin, signalTop, width() - 2 * signalMargin, signalHeight);       // create a new rectangle for the signal 
    painter.fillRect(signalRect, Qt::black);    // fill the signal rectangle with black
    painter.setPen(Qt::green);
   
    // store value to SignalDrawingInfo struct for reuse
    signalElements.signalRect = signalRect;
}

void SignalWidget::drawSignal(QPainter &painter)
{

    int padding = 20;   // adjust borders with padding so the signal is not drawn onto them

    int leftX = signalElements.signalRect.left() + padding;
    int rightX = signalElements.signalRect.right() - padding;
    int topY = signalElements.signalRect.top() + padding;
    int bottomY = signalElements.signalRect.bottom() - padding;  // since (0,0) is top-left in Qt, bottomY is used as Y = 0 line for the signal (inverted Y-axis)

    // available space to be drawn on X/Y scales
    int drawableWidth = rightX - leftX;
    int drawableHeight = bottomY - topY;

    int points;
    if((points = signalData.size()) < 2) return;           // 1 point cannot form a line

    // calculation of space between points so data fit evenly across X scale
    double scaleX = static_cast<double>(drawableWidth) / (points - 1);       // points - 1 = lines to be drawn between points

    for (int i = 1; i < points; i++) 
    {
        int x1 = leftX + (i - 1) * scaleX;
        int x2 = leftX + i * scaleX;
        // in Qt, (0,0) is top-left, Y increases downwards
        // to draw the signal with Y = 0 at bottom and increase it upwards,
        // Y is inverted by subtracting from bottomY
        int y1 = bottomY - (signalData[i - 1] * drawableHeight);
        int y2 = bottomY - (signalData[i] * drawableHeight);

        painter.drawLine(x1, y1, x2, y2);
    }

    // store values to SignalDrawingInfo struct for reuse
    signalElements.leftX = leftX;
    signalElements.bottomY = bottomY;
    signalElements.scaleX = scaleX;
}

void SignalWidget::drawTimeLabels(QPainter &painter)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont("", 8)); 

    // retrieve info from SignalDrawingInfo struct
    QRect signalRect = signalElements.signalRect;

    for (int i = 0; i < signalData.size(); i++) {
        int x = signalElements.leftX + i * signalElements.scaleX;

        QString label = QString::number(i * timestep) + units;        // form label text: time (i * timestep) in units

        QRect textRect = painter.fontMetrics().boundingRect(label);   // pixel size (width and height) of the label text using the default font
        int textX = x - textRect.width() / 2;                         // centered alignment horizontally 
        int textY = signalElements.bottomY + textRect.height();                      // vertical position just below the signal line, inside the signal rectangle

        // adjust label inside signalRect bounds if needed
        if (textX < signalRect.left()) textX = signalRect.left();
        if ((textX + textRect.width()) > signalRect.right()) textX = signalRect.right() - textRect.width();

        painter.drawText(textX, textY, label);
    }
}