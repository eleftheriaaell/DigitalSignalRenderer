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

    if(drawingCache.size() != size())         // if widget was resized, regenerate the cached content
    {
        drawingCache = QPixmap(size());      // create a new pixmap matching the current widget
        drawingCache.fill(Qt::white);        // clear the pixmap with a white background

        // use cachePainter to draw all elements onto the off-screen cache
        QPainter cachePainter(&drawingCache);
        drawTitle(cachePainter);
        drawSignalRect(cachePainter); 
        drawSignal(cachePainter);  
        drawTimeLabels(cachePainter);
    }

    painter.drawPixmap(0, 0, drawingCache);      // draw cached content onto the widget
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

    // store values to SignalDrawingInfo struct for reuse
    signalElements.leftX = leftX;
    signalElements.bottomY = bottomY;
    signalElements.scaleX = scaleX;

    /* small dataset handling 
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
    */
    
    /* optimization for large datasets */
    // calculate the downsampling step size:
    // - if there are more data points than horizontal pixels available (drawableWidth), skip some points to avoid drawing too many and overlapping pixels
    // - if we have fewer points than pixels, step will be 1, meaning no skipping
    // - using qMax ensures step is at least 1, to avoid zero or negative steps
    int step = qMax(points / drawableWidth, 1);        // downsampling step size

    QPainterPath path;          // batch lines instead of calling painter.drawLine() repeatedly
    bool started = false;       // indicate if first point is set

    painter.save();             // save painter state before applying clipping
    painter.setClipRect(signalElements.signalRect);  // restrict drawing to signal area, signal line inside signal rectangle

    for (int i = 0; i < points; i += step) 
    {
        int x = leftX + (i * signalElements.scaleX * signalElements.zoom) + signalElements.offsetX;   // apply zoom scaling and horizontal panning to compute X position
        // in Qt, (0,0) is top-left, Y increases downwards
        // to draw the signal with Y = 0 at bottom and increase it upwards,
        // Y is inverted by subtracting from bottomY
        int y = bottomY - (signalData[i] * drawableHeight);

        if(!started)
        {
            path.moveTo(x,y);   // move path's starting position to first point
            started = true;     // starting point added
        }
        else
            path.lineTo(x,y);   // connect next point to the path with a line
    }
    painter.drawPath(path);    // draw entire path in a single operation — more efficient than drawing each line separately

    painter.restore();  // remove clipping
}

void SignalWidget::drawTimeLabels(QPainter &painter)
{
    painter.setPen(Qt::white);
    painter.setFont(QFont("", 8)); 

    // retrieve info from SignalDrawingInfo struct
    QRect signalRect = signalElements.signalRect;

    // convert time units used in signal waveforms to milliseconds
    int conversion = 1;                          // default unit ms
    if(units == "ns") 
        conversion = 1000000;                    // 1 ms = 1000000 ns
    else if(units == "μs" || units == "us")      // us = μs, keyboard friendly substitute 
        conversion = 1000;                       // 1 ms = 1000 μs

    for (int ms = 1; ; ms++) {                 // loop for every 1 ms, 2 ms, ..., until data ends             
        int index = (ms * conversion) / timestep;      // calculate the index in signalData that corresponds to this millisecond
        if (index >= signalData.size()) break;         // stop if index is beyond the signal data

        int x = signalElements.leftX + (index * signalElements.scaleX * signalElements.zoom) + signalElements.offsetX;
        if (x < signalRect.left())
            continue;                   // skip labels that are offscreen to the left
        if (x > signalRect.right())
            break;                      // stop drawing labels beyond right edge

        QString label = QString::number(ms) + "ms";        // form label text: time in milliseconds

        QRect textRect = painter.fontMetrics().boundingRect(label);   // pixel size (width and height) of the label text using the default font
        int textX = x - textRect.width() / 2;                         // centered alignment horizontally 
        int textY = signalElements.bottomY + textRect.height();       // vertical position just below the signal line, inside the signal rectangle

        // adjust label inside signalRect bounds if needed
        if (textX < signalRect.left()) textX = signalRect.left();
        if ((textX + textRect.width()) > signalRect.right()) textX = signalRect.right() - textRect.width();

        painter.drawText(textX, textY, label);
    }
}

void SignalWidget::wheelEvent(QWheelEvent* event)
{
    /* ctrl + scroll to zoom */
    if(event->modifiers() & Qt::ControlModifier)   // check for pressed modifier keys and if ctrl is one of them
    {
        double oldZoom = signalElements.zoom;      // zoom factor before scrolling

        if (event->angleDelta().y() > 0)        // vertical scroll direction
            signalElements.zoom *= 1.1;         // scroll up, zoom in
        else
            signalElements.zoom /= 1.1;         // scroll down, zoom out

        signalElements.zoom = qBound(0.1, signalElements.zoom, 40.0);        // restrict zoom value between limits, prevent signal to vanish or zoom in excessively

        /* zoom centering around mouse pointer */
        double zoomChange = signalElements.zoom / oldZoom;          // zoom level change as ratio, i.e. zoom from 1.0x to 1.1x makes zoomChange = 1.1 (zoomed in by 10%)
        double mouseToSignal = event->x() - signalElements.leftX;   // mouse position on X scale, relative to the start of the signal 
        signalElements.offsetX = (signalElements.offsetX - mouseToSignal) * zoomChange + mouseToSignal;  // scale offset relative to the mouse position, zoom centered around cursor

        drawingCache = QPixmap();          // clear cached content
        update();                          // trigger repaint
    }
    else
        QWidget::wheelEvent(event);       // default behavior
}

void SignalWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = true;
        lastDragPos = event->pos();         // track mouse position
        setCursor(Qt::ClosedHandCursor);    // change cursor icon while dragging
    }
}

void SignalWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (dragging)
    {
        int distanceX = event->x() - lastDragPos.x();       // horizontal distance moved by the mouse after dragging
        signalElements.offsetX += distanceX;                // shift signal horizontaly based on drag

        lastDragPos = event->pos();         // update last position for next drag

        drawingCache = QPixmap();           // clear cached content
        update();                           // trigger repaint
    }
}

void SignalWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = false;
        unsetCursor();      // restore default cursor
    }
}

void SignalWidget::resetView()
{
    signalElements.zoom = 1.0;        // default zoom
    signalElements.offsetX = 0.0;     // default pan (no offset)

    drawingCache = QPixmap();         // clear cached content
    update();                         // trigger repaint
}

void SignalWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    resetView();
}