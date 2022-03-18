
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "ll_protocol.h"
#include <QQueue>
#include <QThread>

using namespace testing;


extern QQueue<uint8_t> byte_stream;
extern size_t error_short_msg;
extern size_t error_long_msg ;

class LL_Protocol:public ::Test
{
protected:
     void SetUp() override
     {
         thread_1 = QThread::create(start_deserializing);
     }

    void TearDown()override
    {
        free(output_data);

    }
    QThread* thread_1;
    size_t serialized_data_size;
    uint8_t* output_data;
    uint8_t input_data_1[12] = {0xBB, 0xAA, 0x95, 0xCC, 0x8B, 0x12, 0xCC, 0x34, 0xDD, 0xAA, 0x77, 0xBB};
    uint8_t check_data_1[20] = {0xAA, 0xCC, 0xBB, 0xCC, 0xAA, 0x95, 0xCC, 0xCC, 0x8B, 0x12, 0xCC, 0xCC, 0x34, 0xDD, 0xCC, 0xAA, 0x77, 0xCC, 0xBB, 0xBB};
    uint8_t input_data_2[12] = {0xF3, 0x77, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD};
    uint8_t check_data_2[18] = { 0xDD, 0xBB, 0xAA, 0xF3, 0x77, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};
 };

TEST(ll_protocol_tests, nullptr_serialize_data_test)
{
    EXPECT_FALSE(serialize_data(nullptr, nullptr, nullptr));
}

TEST_F(LL_Protocol, serialize_data_size_test)
{
    EXPECT_TRUE(serialize_data(input_data_1, &output_data, &serialized_data_size));
    EXPECT_EQ(20UL, serialized_data_size);
}

TEST_F(LL_Protocol, serialize_data_test)
{
    EXPECT_TRUE(serialize_data(input_data_1, &output_data, &serialized_data_size));

    for(size_t i = 0; i< serialized_data_size; i++)
    {
        EXPECT_EQ(check_data_1[i], output_data[i]);
    }
}

TEST_F(LL_Protocol, deserialize_test)
{
    for(uint i = 0; i < 18; i++)
    {
        byte_stream.enqueue(check_data_2[i]);
    }
    thread_1->start();
    while(!byte_stream.empty())
    {
        thread_1->quit();
        EXPECT_EQ(error_short_msg, 0UL);
        EXPECT_EQ(error_long_msg, 0UL);
    }
}





