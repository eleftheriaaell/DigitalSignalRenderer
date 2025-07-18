#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QVector>
#include <QWidget>
#include <QPainter>

class SignalWidget : public QWidget{        // custom widget, inherits from QWidget for GUI rendering

    QVector<int> signalData;         // waveform data 
    int timestep;
    QString units;

public:
    SignalWidget() : QWidget(nullptr)           // constructor with no parameter, widget is always a top-level window (no parent)
    {
        parseData("../data.txt");
        setMinimumSize(800, 100);               // minimum size of window
    };

    void parseData(const QString &filename)         
    {
        QFile file(filename);
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

protected:                                  // protected in QWidget class so it must be protected or public in the derived class
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);        // fill the whole widget with white

        int signalHeight = height() / 3;            // take 1/3 of the widget to display the signal
        int margin = 20;                            // margin between main window and signal rectangle 
        QRect signalRect(margin, margin, width() - 2 * margin, signalHeight);       // create a new rectangle for the signal 
        painter.fillRect(signalRect, Qt::black);    // fill the signal rectangle with black
        painter.setPen(Qt::green);

        // adjust borders with padding so the signal is not drawn onto them
        int padding = 20;  

        int leftX = signalRect.left() + padding;
        int rightX = signalRect.right() - padding;
        int topY = signalRect.top() + padding;
        int bottomY = signalRect.bottom() - padding;  // since (0,0) is top-left in Qt, bottomY is used as Y = 0 line for the signal (inverted Y-axis)

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
    }

};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SignalWidget widget;
    widget.show();                      // show widget on screen

    return app.exec();
}