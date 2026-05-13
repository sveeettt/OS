#include <gtest/gtest.h>
#include "../include/protocol.h"
#include <cstring>

class ProtocolTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(ProtocolTest, InitializeMessage) {
    Message msg;
    initialize_message(msg);

    EXPECT_EQ(msg.type, MSG_ERROR);
    EXPECT_EQ(msg.employee_id, -1);
    EXPECT_EQ(msg.client_pid, -1);
}

TEST_F(ProtocolTest, SetErrorMessage) {
    Message msg;
    std::string error = "Test error message";

    set_error_message(msg, error);

    EXPECT_EQ(msg.type, MSG_ERROR);
    EXPECT_STREQ(msg.error_msg, error.c_str());
}

TEST_F(ProtocolTest, SetLongErrorMessage) {
    Message msg;
    std::string long_error(300, 'x');

    set_error_message(msg, long_error);

    EXPECT_EQ(msg.type, MSG_ERROR);
    EXPECT_EQ(strlen(msg.error_msg), sizeof(msg.error_msg) - 1);
    EXPECT_EQ(msg.error_msg[sizeof(msg.error_msg) - 1], '\0');
}