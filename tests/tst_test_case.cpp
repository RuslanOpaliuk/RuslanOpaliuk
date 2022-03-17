
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "ll_protocol.h"

using namespace testing;

TEST(ll_protocol_tests, nullptr_serialize_data_test)
{
    EXPECT_FALSE(serialize_data(nullptr, nullptr, nullptr));
}

