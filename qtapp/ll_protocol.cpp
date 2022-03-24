#include "ll_protocol.h"


static uint8_t message[MESSAGE_SIZE];


void start_deserializing()
{
    bool message_opened = false;
    bool reject = false;
    bool ignore_previous = false;
    size_t message_iter = 0;
    uint8_t byte = END_BYTE;
    uint8_t previous_byte = END_BYTE;

    while(true)
    {
        previous_byte = byte;
        byte = get_next_byte_cb();

        if(  !message_opened
           && byte == BEGIN_BYTE
           && previous_byte != REJECT_BYTE)
        {
            message_opened = true;
        }

        if(!message_opened)
        {
            continue;
        }

        if(message_iter == MESSAGE_SIZE)
        {
            message_opened = false;
            if(byte == END_BYTE)
            {
                message_ready_cb(message);
            }
            else
            {
                error_cb(LL_PROTOCOL_ERR_MESSAGE_TOO_LONG);
            }
            message_iter = 0;
            continue;
        }

        if(   byte == END_BYTE
           && previous_byte != REJECT_BYTE
           && !ignore_previous
           && message_iter < MESSAGE_SIZE)
        {
            message_opened = false;
            reject = false;
            message_iter = 0;
            error_cb(LL_PROTOCOL_ERR_MESSAGE_TOO_SHORT);
        }

        //it seems that it can be optimized
        //by combining "if()" at lines 61 and 76
        if(   (byte != REJECT_BYTE
           && byte != BEGIN_BYTE
           && byte != END_BYTE
           && previous_byte != REJECT_BYTE)
           ||
             (byte != REJECT_BYTE
           && byte != BEGIN_BYTE
           && byte != END_BYTE
           && ignore_previous))
        {
            message[message_iter++] = byte;
            ignore_previous = false;
            continue;
         }

        if(reject)
        {
            message[message_iter++] = byte;
            if(byte == REJECT_BYTE)
            {
                ignore_previous = true;
            }
            reject = false;
            continue;
        }

        if(byte == REJECT_BYTE && (previous_byte != REJECT_BYTE || ignore_previous))
        {
            reject = true;
            if(ignore_previous)
            {
                ignore_previous = false;
            }
        }
    }
}

bool serialize_data(uint8_t* const data_in, uint8_t** data_out, size_t* size_out)
{
    if(!data_in || !data_out || !size_out)
    {
        return false;
    }

    //+2 is for start byte at the beginning and stop byte at the end of message
    *size_out = MESSAGE_SIZE + 2;

    for(size_t i = 0; i < MESSAGE_SIZE; i++)
    {
        if(   data_in[i] == BEGIN_BYTE
           || data_in[i] == END_BYTE
           || data_in[i] == REJECT_BYTE)
        {
            (*size_out)++;
        }
    }

    *data_out = (uint8_t*)malloc(*size_out);
    if(!(*data_out))
    {
        return false;
    }

    uint8_t* tmp_out = *data_out;

    *tmp_out = BEGIN_BYTE;
    tmp_out++;

    for(size_t i = 0; i < MESSAGE_SIZE; i++)
    {
        if(   data_in[i] == BEGIN_BYTE
           || data_in[i] == END_BYTE
           || data_in[i] == REJECT_BYTE)
        {
            *tmp_out = REJECT_BYTE;
            tmp_out++;
            *tmp_out = data_in[i];
            tmp_out++;
        }
        else
        {
            *tmp_out = data_in[i];
            tmp_out++;
        }
    }
    *tmp_out = END_BYTE;
    return true;
}

