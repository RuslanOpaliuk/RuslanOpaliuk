#include <QCoreApplication>
#include <QtSerialPort>
#include "ll_protocol.h"
#include "math.h"
#include "mainwindow.h"

#define OFFSET_CAPTURES    1000  //quantity of captures for root mean square
#define SHOW_GRAPH         0     //set to 1 to show graph after impact
#define NUMBER_OF_CAPTURES 200   //quantity of captures after impact
#define PREVIOUS_CAPTURES  10    //quantity of captures before impact
#define THRESHOLD          50.0  //threshold which must be achieved by impact to cause capturing


static QQueue<uint8_t> byte_stream;
static QMutex byte_stream_mutex;

typedef struct
{
    uint8_t message[MESSAGE_SIZE];
} message_t;

static QQueue<message_t> messages;
static QMutex messages_mutex;


void ll_message_ready_cb(uint8_t* const message)
{
    message_t msg;
    memcpy(msg.message, message, MESSAGE_SIZE);
    messages_mutex.lock();
    messages.enqueue(msg);
    messages_mutex.unlock();
}

size_t error_short_msg = 0;
size_t error_long_msg = 0;
void ll_error_cb(ll_protocol_err_t error)
{
    if(error == LL_PROTOCOL_ERR_MESSAGE_TOO_SHORT)
    {
        error_short_msg++;
    }
    if(error == LL_PROTOCOL_ERR_MESSAGE_TOO_LONG)
    {
        error_long_msg++;
    }
}

static double convert(uint8_t* const data)
{
    static double tmp[3];
    tmp[0] = (int16_t)(((((uint16_t)data[1]) << 8) | data[0])) * 0.004f * 9.80954f;
    tmp[1] = (int16_t)(((((uint16_t)data[3]) << 8) | data[2])) * 0.004f * 9.80954f;
    tmp[2] = (int16_t)(((((uint16_t)data[5]) << 8) | data[4])) * 0.004f * 9.80954f;

    return sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]);
}

class UartReceive : public QThread
{
public:
    void run();
};

class Deserialize : public QThread
{
public:
    void run();
};

void UartReceive::run()
{
    QSerialPort* serial = new QSerialPort();
    serial->setPortName("ttyUSB0");
    serial->setBaudRate(2000000);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    while(true)
    {
        if(serial->open(QIODevice::ReadWrite))
        {
            printf("Connected to ttyUSB0.\n");
            break;
        }
        printf("Can't connect to ttyUSB0. Retrying...\n");
        QThread::sleep(1);
    }

    while(true)
    {
        QByteArray arr = serial->readAll();
        while(serial->waitForReadyRead(0))
        {
            arr += serial->readAll();
        }

        if(arr.size() == 0)
        {
            continue;
        }

        byte_stream_mutex.lock();
        for(int i = 0; i < arr.size(); i++)
        {
            byte_stream.enqueue((uint8_t)arr[i]);
        }
        byte_stream_mutex.unlock();
    }
}

void Deserialize::run()
{
    QByteArray data;
    size_t remainder = SIZE_MAX;

    while(true)
    {
        if(remainder < (size_t)data.size())
        {
            data.remove(0, remainder);
        }
        else
        {
            data.clear();
        }

        byte_stream_mutex.lock();
        while(!byte_stream.isEmpty())
        {
            uint8_t tmp = byte_stream.dequeue();
            data.push_back(tmp);
        }
        byte_stream_mutex.unlock();

        ll_deserialize((uint8_t*)data.data(), data.size(), &remainder);
    }
}

double _root_mean_square(QVector<double>* vect)
{
    double tmp = 0.0;
    for(int i = 0; i < vect->size(); i++)
    {
        tmp += (*(vect))[i] * (*(vect))[i];
    }
    return sqrt(tmp / vect->size());
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UartReceive b;
    b.start();
    Deserialize c;
    c.start();

    QQueue<double> previous1;
    QQueue<double> previous2;
    QVector<double> impact1;
    QVector<double> impact2;
    QVector<double> impactx;

    bool start_capturing = false;
    bool offset_ready = false;
    double offset = 0.0;

    message_t msg;

    while(true)
    {
        messages_mutex.lock();
        if(messages.isEmpty())
        {
            messages_mutex.unlock();
            continue;
        }

        msg = messages.dequeue();
        messages_mutex.unlock();

        double tmp1 = convert(msg.message);
        double tmp2 = convert(msg.message + 6);

        if(impact1.size() == OFFSET_CAPTURES && !start_capturing && !offset_ready)
        {
            offset = _root_mean_square(&impact1) - _root_mean_square(&impact2);
            offset_ready = true;
            impact1.erase(impact1.begin(), impact1.end());
            impact2.erase(impact2.begin(), impact2.end());
            printf("offset: %.2f\n", offset);
        }

        if(!offset_ready)
        {
            impact1.append(abs(tmp1));
            impact2.append(abs(tmp2));
            continue;
        }

        if(!start_capturing)
        {
            previous1.push_front(abs(tmp1));
            previous2.push_front(abs(tmp2) + offset);
            if(previous1.size() > PREVIOUS_CAPTURES)
            {
                previous1.removeLast();
                previous2.removeLast();
            }
        }

        if(!start_capturing && (tmp1 > THRESHOLD || tmp2 > THRESHOLD))
        {
            start_capturing = true;
        }

        if(start_capturing && impact1.size() < NUMBER_OF_CAPTURES)
        {
            impact1.append(abs(tmp1));
            impact2.append(abs(tmp2) + offset);
        }

        if(start_capturing && impact1.size() == NUMBER_OF_CAPTURES)
        {
            start_capturing = false;

            while(!previous1.isEmpty())
            {
                impact1.prepend(previous1.dequeue());
                impact2.prepend(previous2.dequeue());
            }

            for(int i = 0; i < impact1.size(); i++)
            {
                impactx.append(i);
            }

            printf("%.2f\n", _root_mean_square(&impact1) - _root_mean_square(&impact2));

            if(SHOW_GRAPH)
            {
                break;
            }
            else
            {
                impact1.erase(impact1.begin(), impact1.end());
                impact2.erase(impact2.begin(), impact2.end());
                impactx.erase(impactx.begin(), impactx.end());
                offset_ready = false;
                //need to wait before oscillations get reduced
                QThread::sleep(2);
            }
        }
    }

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);
    MainWindow mw(0, &impactx, &impact1, &impact2);
    mw.setAutoFillBackground(true);
    mw.setPalette(pal);
    mw.show();

    return a.exec();
}

