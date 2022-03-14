#include "ll_protocol.h"


static uint8_t message[MESSAGE_SIZE];


void start_deserializing()
{
    bool message_opened = false;
    bool early_start = true;
    bool reject = false;
    size_t message_iter = 0;
    uint8_t byte = 0;
    uint8_t previous_byte = 0;

    while(true)
    {
        if(message_iter > MESSAGE_SIZE)
        {
            message_opened = false;
            reject = false;
            message_iter = 0;
            error_cb(LL_PROTOCOL_ERR_MESSAGE_TOO_LONG);
        }

        previous_byte = byte;
        byte = get_next_byte_cb();

        if(message_opened)
        {
            if(reject)
            {
                message[message_iter++] = byte;
                reject = false;
                continue;
            }

            if(      (byte != REJECT_BYTE
                   && byte != BEGIN_BYTE
                   && byte != END_BYTE
                   && previous_byte != REJECT_BYTE)
               ||
                    ((byte == REJECT_BYTE
                   || byte == BEGIN_BYTE
                   || byte == END_BYTE)
                   && previous_byte == REJECT_BYTE
                   && !reject))
            {
                message[message_iter++] = byte;
                if(byte == REJECT_BYTE)
                {
                    reject = true;
                }
            }
            else if(      (byte == REJECT_BYTE
                        && previous_byte != REJECT_BYTE)
                    ||
                          (byte == REJECT_BYTE
                        && reject))
            {
                if(reject)
                {
                    reject = false;
                }
                continue;
            }
            else if(     (byte == END_BYTE
                       && previous_byte != REJECT_BYTE)
                    ||
                         (byte == END_BYTE
                       && previous_byte == REJECT_BYTE
                       && reject))
            {
                message_opened = false;
                reject = false;
                if(message_iter == MESSAGE_SIZE)
                {
                    message_ready_cb(message);
                }
                else
                {
                    error_cb(LL_PROTOCOL_ERR_MESSAGE_TOO_SHORT);
                }
                message_iter = 0;
            }
        }
        else if(!message_opened
                && early_start   == false
                && byte          == BEGIN_BYTE
                && previous_byte != REJECT_BYTE)
        {
            message_opened = true;
        }
        else if(early_start)
        {
            early_start = false;
        }
    }
}

bool serialize_data(uint8_t* const data_in, uint8_t** data_out, size_t* size_out)
{
    if(!data_in || !data_out || !*data_out || !size_out)
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

