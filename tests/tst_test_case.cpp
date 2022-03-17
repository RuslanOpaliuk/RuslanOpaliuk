
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "ll_protocol.h"

using namespace testing;

class LL_Protocol:public ::Test
{
protected:

    void TearDown()override
    {
        free(output_data);
    }

    size_t serialized_data_size;
    uint8_t* output_data;
    uint8_t input_data_1[12] = {0xBB, 0xAA, 0x95, 0xCC, 0x8B, 0x12, 0xCC, 0x34, 0xDD, 0xAA, 0x77, 0xBB};
    uint8_t check_data_1[20] = {0xAA, 0xCC, 0xBB, 0xCC, 0xAA, 0x95, 0xCC, 0xCC, 0x8B, 0x12, 0xCC, 0xCC, 0x34, 0xDD, 0xCC, 0xAA, 0x77, 0xCC, 0xBB, 0xBB};
 };

TEST(ll_protocol_tests, nullptr_serialize_data_test)
{
    EXPECT_FALSE(serialize_data(nullptr, nullptr, nullptr));
}

TEST_F(LL_Protocol, serialize_data_size_test)
{
    EXPECT_TRUE(serialize_data(input_data_1, &output_data, &serialized_data_size));
    EXPECT_EQ(20, serialized_data_size);
}

TEST_F(LL_Protocol, serialize_data_test)
{
    EXPECT_TRUE(serialize_data(input_data_1, &output_data, &serialized_data_size));

    for(size_t i = 0; i< serialized_data_size; i++)
    {
        EXPECT_EQ(check_data_1[i], output_data[i]);
    }
}



