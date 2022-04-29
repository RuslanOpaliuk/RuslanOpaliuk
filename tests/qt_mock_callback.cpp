#include "ll_protocol.h"

#include <qmutex.h>
#include <qqueue.h>



QQueue<uint8_t> byte_stream;
QMutex byte_stream_mutex;

typedef struct
{
    uint8_t message[MESSAGE_SIZE];
} message_t;

QQueue<message_t> messages;
QMutex messages_mutex;

void ll_message_ready_cb(uint8_t* const message)
{
    message_t msg;
    memcpy(msg.message, message, MESSAGE_SIZE);
    messages_mutex.lock();
    messages.enqueue(msg);
    messages_mutex.unlock();
}

size_t error_short_msg  = 0;
size_t error_long_msg  = 0;

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
