#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QVector>
#include <QWidget>

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

    // void printData()
    // {
    //     qDebug() << signalData;         
    // }

};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SignalWidget widget;
    widget.show();                      // show widget on screen

    return app.exec();
}