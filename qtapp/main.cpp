#include <QCoreApplication>
#include <QtSerialPort>
#include "ll_protocol.h"


static QQueue<uint8_t> byte_stream;
static QMutex byte_stream_mutex;

typedef struct
{
    uint8_t message[MESSAGE_SIZE];
} message_t;

static QQueue<message_t> messages;
static QMutex messages_mutex;


uint8_t get_next_byte_cb()
{
    uint8_t tmp;
    while(true)
    {
        byte_stream_mutex.lock();
        if(byte_stream.empty())
        {
            byte_stream_mutex.unlock();
            continue;
        }
        tmp = byte_stream.dequeue();
        byte_stream_mutex.unlock();
        break;
    }
    return tmp;
}

void message_ready_cb(uint8_t* const message)
{
    message_t msg;
    memcpy(msg.message, message, MESSAGE_SIZE);
    messages_mutex.lock();
    messages.enqueue(msg);
    messages_mutex.unlock();
}

size_t error_short_msg = 0;
size_t error_long_msg = 0;
void error_cb(ll_protocol_err_t error)
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

static float convert(uint8_t* const data)
{
    static float tmp[3];
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
            byte_stream.enqueue(arr[i]);
        }
        byte_stream_mutex.unlock();
    }
}

void Deserialize::run()
{
    printf("deserializing started\n");
    start_deserializing();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    UartReceive b;
    b.start();
    Deserialize c;
    c.start();

    while(true)
    {
        messages_mutex.lock();
        if(messages.isEmpty())
        {
            messages_mutex.unlock();
            continue;
        }
        message_t msg = messages.dequeue();
        printf("%.1f\t%.1f\tErr_too_short: % 2lu\tErr_too_long: % 2lu\n",
               convert(msg.message),
               convert(msg.message + 6),
               error_short_msg,
               error_long_msg);
        messages_mutex.unlock();
    }

    return a.exec();
}

