
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "ll_protocol.h"
#include <QQueue>
#include <QThread>
#include <QMutex>

using namespace testing;

typedef struct
{
    uint8_t message[MESSAGE_SIZE];
} message_t;



extern QQueue<message_t> messages;
extern size_t error_short_msg ;
extern size_t error_long_msg ;
extern QMutex messages_mutex;

class LL_Protocol:public ::Test
{
protected:

    void TearDown()override
    {
        free(output_data);        
        error_short_msg = 0;
        error_long_msg = 0;
        reminder_start = 0;
        messages.clear();
    }

    size_t reminder_start =0 ;
    uint8_t* output_data = NULL;
    uint8_t input_data_2[12] = {0xF3, 0x77, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD};
    uint8_t check_data_2[18] = {0xAA, 0xF3, 0x77, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};

 };

TEST(ll_protocol_tests, nullptr_serialize_data_test)
{
    EXPECT_FALSE(ll_serialize(nullptr, nullptr));
}

TEST_F(LL_Protocol, serialize_data_size_test)
{

    uint8_t input_data_1[12] = {0xBB, 0xAA, 0x95, 0xCC, 0x8B, 0x12, 0xCC, 0x34, 0xDD, 0xAA, 0x77, 0xBB};


    size_t size =ll_sizeof_serialized(input_data_1);
    output_data = (uint8_t*)malloc(size);
    EXPECT_EQ(size, 20);
    EXPECT_TRUE(ll_serialize(input_data_1, output_data));

}

TEST_F(LL_Protocol, serialize_data_test)
{
    size_t size =ll_sizeof_serialized(input_data_2);
    output_data = (uint8_t*)malloc(size);
    EXPECT_TRUE(ll_serialize(input_data_2, output_data));

    for(size_t i = 0; i< size; i++)
    {
        EXPECT_EQ(check_data_2[i], output_data[i]);
    }
}

TEST_F(LL_Protocol, serialize_data_CC_test)
{
    uint8_t input_data_CC[12] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
    uint8_t check_data_CC[26] = {0xAA, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xBB};
    size_t size =ll_sizeof_serialized(input_data_CC);
    output_data = (uint8_t*)malloc(size);

    EXPECT_EQ(size, 26);

    EXPECT_TRUE(ll_serialize(input_data_CC, output_data));

    for(size_t i = 0; i< size; i++)
    {
        EXPECT_EQ(check_data_CC[i], output_data[i]);
    }
}

TEST_F(LL_Protocol, deserializing_test)
{
    uint8_t stream_data[] = {0xAA, 0xF3, 0x77, 0x20, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xBB};

    EXPECT_TRUE(ll_deserialize(stream_data, sizeof(stream_data), &reminder_start));
    EXPECT_EQ(error_short_msg, 0UL);
    EXPECT_EQ(error_long_msg, 0UL);

}

TEST_F(LL_Protocol, deserializing_to_long_test)
{
    uint8_t long_stream_data[] = {0xDD, 0xBB, 0xAA, 0xF3, 0x77, 0x20, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};

    EXPECT_TRUE(ll_deserialize(long_stream_data, sizeof(long_stream_data), &reminder_start));

    EXPECT_EQ(error_short_msg, 0UL);
    EXPECT_EQ(error_long_msg, 1UL);
}

TEST_F(LL_Protocol, deserializing_to_short_test)
{
    uint8_t short_stream_data[] = {0xDD, 0xBB, 0xAA, 0xF3, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};

    EXPECT_TRUE(ll_deserialize(short_stream_data, sizeof(short_stream_data), &reminder_start));

    EXPECT_EQ(error_short_msg, 1UL);
    EXPECT_EQ(error_long_msg, 0UL);
}

TEST_F(LL_Protocol, deserializing_check_message_test)
{
    uint8_t multi_stream_data[] = {0xDD, 0xBB,
     0xAA, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xBB,
     0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xBB,
     0xAA, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xBB};

    EXPECT_TRUE(ll_deserialize(multi_stream_data, sizeof(multi_stream_data), &reminder_start));

     EXPECT_EQ(error_short_msg, 0UL);
     EXPECT_EQ(error_long_msg, 0UL);
     EXPECT_EQ(messages.size(), 3);
}








