/*
    This code is written in C language to make it portable on
STM32 platform. Purpose of this code is to catch separate messages from the
byte stream. That is done by serializing message on the
transmitter node and deserializing byte stream on the receiver node.
Serializing means adding some extra bytes to message which
help to detect begin and the end of message in the byte stream. These
bytes are BEGIN_BYTE, END_BYTE and REJECT_BYTE. They are called
"control bytes". BEGIN_BYTE is added at the beginning of message,
END_BYTE at the end of message, REJECT_BYTE is used to differentiate
control bytes from message bytes.

    You can define control bytes as you want but they must be
different from each other. For example, BEGIN_BYTE can be equal to
0x56, END_BYTE 0x65 and REJECT_BYTE 0xFF and so on. Defining
your own values for control bytes can be useful when the message
that you must serialize contains a lot of bytes which values are
the same as values of control bytes. That means that you can
decrease the redundancy. You can understand it better in the
examples below.

    NOTE. There is also control of message size. That means that
you can't serialize/deserialize a message with size (quantity of bytes)
that is not equal to MESSAGE_SIZE. MESSAGE_SIZE can be defined by you.
MESSAGE_SIZE must be equal both on transmitter and receiver nodes.

Examples (for MESSAGE_SIZE 16 bytes, BEGIN_BYTE 0xAA, END_BYTE 0xBB
and REJECT_BYTE 0xCC):

1. Message that we need to serialize and transmit:
   F3 77 56 C4 95 94 76 8B 12 88 34 DD 44 77 51 31

   Byte stream after serializing:
   AA F3 77 56 C4 95 94 76 8B 12 88 34 DD 44 77 51 31 BB

   As you can see, resulting byte stream that will be transmitted
   contains 18 bytes. More than MESSAGE_SIZE. So, the rendundancy for
   the message in this example is 2 bytes. As you can see BEGIN_BYTE
   added at the beginning of message and END_BYTE at the end of message.

2. Message that we need to serialize and transmit (mention that
   second byte has the same value as END_BYTE):
   F3 BB 56 C4 95 94 76 8B 12 88 34 DD 44 77 51 31

   Byte stream after serializing:
   AA F3 CC BB 56 C4 95 94 76 8B 12 88 34 DD 44 77 51 31 BB

   Resulting byte stream is 19 bytes. Rendundancy is 3 bytes. REJECT_BYTE
   is added before second byte of message to inform deserializator that
   this byte is not control byte. Without REJECT_BYTE deserializator
   would interpret that byte as the end of message.

3. Message that we need to serialize and transmit (mention that
   second byte has the same value as REJECT_BYTE):
   F3 CC 56 C4 95 94 76 8B 12 88 34 DD 44 77 51 31

   Byte stream after serializing:
   AA F3 CC CC 56 C4 95 94 76 8B 12 88 34 DD 44 77 51 31 BB

   Resulting byte stream is 19 bytes. Rendundancy is 3 bytes. REJECT_BYTE
   is added before second byte of message to inform deserializator that
   this byte is not control byte. Without REJECT_BYTE deserializator
   would interpret that byte as control byte.

4. Other examples.
   input:  F3 BB AA C4 95 CC 76 8B 12 CC 34 DD AA 77 51 BB
   output: AA F3 CC BB CC AA C4 95 CC CC 76 8B 12 CC CC 34 DD CC AA 77 51 CC BB BB
   byte stream 24 bytes, redundancy 8 bytes

   input:  CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC
   output: AA CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC BB
   byte stream 34 bytes, redundancy 18 bytes

   input:  AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA AA
   output: AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA CC AA BB
   byte stream 34 bytes, redundancy 18 bytes

   input:  BB BB BB BB BB BB BB BB BB BB BB BB BB BB BB BB
   output: AA CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB CC BB BB
   byte stream 34 bytes, redundancy 18 bytes

   As you can see, the more bytes, which values equal to values of control bytes,
are in message, the more redundancy it causes. The redundancy can't be more
than (MESSAGE_SIZE * 2) + 2.
*/

#ifndef LL_PROTOCOL_H
#define LL_PROTOCOL_H

#include <stdint.h>
#include <stdlib.h>


#define MESSAGE_SIZE  16U

#define BEGIN_BYTE  0xAAU
#define END_BYTE    0xBBU
#define REJECT_BYTE 0xCCU

typedef enum
{
    LL_PROTOCOL_ERR_MESSAGE_TOO_SHORT,
    LL_PROTOCOL_ERR_MESSAGE_TOO_LONG
} ll_protocol_err_t;

//don't forget to free data_out if that function returned true
//because there is dynamic allocation inside
bool serialize_data(
    uint8_t* const data_in, //array of bytes which must be serialized
    uint8_t** data_out,     //result array of serializing
    size_t* size_out        //size of result array
);

//this function must work in another thread
void start_deserializing();

//next functions are used by start_deserializing() and must be defined by you

//this function must return next byte from byte stream
//and is called repeatedly by start_deserializing() function
//you must provide blocking behavior of this function
uint8_t get_next_byte_cb();

//this function is called by start_deserializing() function
//every time when message is ready to read
//as example, you can implement adding this message to some queue
void message_ready_cb(
    uint8_t* const message //incoming message to process with size of MESSAGE_SIZE
);

//this function is called by start_deserializing() function
//every time when error happens
void error_cb(
    ll_protocol_err_t error //error code
);

#endif // LL_PROTOCOL_H
