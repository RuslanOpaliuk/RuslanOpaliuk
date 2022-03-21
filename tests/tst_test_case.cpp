
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


extern QQueue<uint8_t> byte_stream;
extern QQueue<message_t> messages;
extern size_t error_short_msg ;
extern size_t error_long_msg ;
extern QMutex byte_stream_mutex;
extern QMutex messages_mutex;

//class LL_Protocol:public ::Test
//{
//protected:
//     void SetUp() override
//     {
//         thread_1 = QThread::create(start_deserializing);

//     }

//    void TearDown()override
//    {
//        free(output_data);
//        byte_stream.clear();
//        error_short_msg = 0;
//        error_long_msg = 0;
//        serialized_data_size = 0;
//    }

//    QThread* thread_1;

//    size_t serialized_data_size;
//    uint8_t* output_data;

//    uint8_t input_data_2[12] = {0xF3, 0x77, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD};
//    uint8_t check_data_2[18] = {0xAA, 0xF3, 0x77, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};

// };

//TEST(ll_protocol_tests, nullptr_serialize_data_test)
//{
//    EXPECT_FALSE(serialize_data(nullptr, nullptr, nullptr));
//}

//TEST_F(LL_Protocol, serialize_data_size_test)
//{

//    uint8_t input_data_1[12] = {0xBB, 0xAA, 0x95, 0xCC, 0x8B, 0x12, 0xCC, 0x34, 0xDD, 0xAA, 0x77, 0xBB};

//    EXPECT_TRUE(serialize_data(input_data_1, &output_data, &serialized_data_size));
//    EXPECT_EQ(20UL, serialized_data_size);
//}

//TEST_F(LL_Protocol, serialize_data_test)
//{

//    ASSERT_TRUE(serialize_data(input_data_2, &output_data, &serialized_data_size));

//    for(size_t i = 0; i< serialized_data_size; i++)
//    {
//        EXPECT_EQ(check_data_2[i], output_data[i]);
//    }
//}

//TEST_F(LL_Protocol, serialize_data_CC_test)
//{
//    uint8_t input_data_CC[12] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
//    uint8_t check_data_CC[26] = {0xAA, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xBB};

//    EXPECT_TRUE(serialize_data(input_data_CC, &output_data, &serialized_data_size));

//    for(size_t i = 0; i< serialized_data_size; i++)
//    {
//        EXPECT_EQ(check_data_CC[i], output_data[i]);
//    }
//}

//TEST_F(LL_Protocol, deserializing_test)
//{
//    for(size_t i = 0; i < 18; i++)
//    {
//        byte_stream.enqueue(check_data_2[i]);
//    }

//    thread_1->start();
//    while(!byte_stream.empty())
//    {
//        byte_stream_mutex.unlock();
//        messages_mutex.unlock();
//        thread_1->quit();
//    }

//    EXPECT_EQ(error_short_msg, 0UL);
//    EXPECT_EQ(error_long_msg, 0UL);

//}

//TEST_F(LL_Protocol, deserializing_to_long_test)
//{
//    uint8_t check_data_4[19] = {0xDD, 0xBB, 0xAA, 0xF3, 0x77, 0x20, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};


//    for(size_t i = 0; i < 19; i++)
//    {
//        byte_stream.enqueue(check_data_4[i]);
//    }
//    thread_1->start();

//    while(!byte_stream.empty())
//    {
//        byte_stream_mutex.unlock();
//        messages_mutex.unlock();
//        thread_1->quit();
//    }

//    EXPECT_EQ(error_short_msg, 0UL);
//    EXPECT_EQ(error_long_msg, 1UL);
//}

//TEST_F(LL_Protocol, deserializing_to_short_test)
//{
//    uint8_t check_data_5[19] = {0xDD, 0xBB, 0xAA, 0xF3, 0x56, 0xC4, 0x95, 0x94, 0x76, 0x8B, 0x12, 0x88, 0x34, 0xDD, 0xBB};

//    for(size_t i = 0; i < 17; i++)
//    {
//        byte_stream.enqueue(check_data_5[i]);
//    }

//    thread_1->start();

//    while(!byte_stream.empty())
//    {
//        byte_stream_mutex.unlock();
//        messages_mutex.unlock();
//        thread_1->quit();
//    }

//    EXPECT_EQ(error_short_msg, 1UL);
//    EXPECT_EQ(error_long_msg, 0UL);
//}

//TEST_F(LL_Protocol, deserializing_check_message_test)
//{
//    uint8_t check_data_6[80] = {0xDD, 0xBB,
//     0xAA, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xCC, 0xBB, 0xBB,
//     0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xCC, 0xAA, 0xBB,
//     0xAA, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xBB};

//    for(size_t i = 0; i < 80; i++)
//    {
//        byte_stream.enqueue(check_data_6[i]);
//    }

//    thread_1->start();

//    while(!byte_stream.empty())
//    {
//        byte_stream_mutex.unlock();
//        messages_mutex.unlock();
//        thread_1->quit();
//    }

//     EXPECT_EQ(error_short_msg, 0UL);
//     EXPECT_EQ(error_long_msg, 1UL);
//     EXPECT_EQ(messages.size(), 2UL);
//}




TEST(LL_Protocol_testi, deserializing_check_message_test)
{
    uint8_t check_data_6[] = {0xDD, 0xBB,

     0xAA, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xBB};

    for(size_t i = 0; i < 28; i++)
    {
        byte_stream.enqueue(check_data_6[i]);
    }

    start_deserializing();

    EXPECT_EQ(error_short_msg, 0UL);
    EXPECT_EQ(error_long_msg, 0UL);
    // EXPECT_EQ(messages.size(), 3);
}



