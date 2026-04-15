#include <gtest/gtest.h>
#include "sender.h"
#include "../common/message_file.h"

using namespace std;

class SenderTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_filename = "test_message_file.bin";
        MessageFile::Create(test_filename, 10);
    }

    void TearDown() override {
        remove(test_filename.c_str());
    }

    string test_filename;
};

TEST_F(SenderTest, OpenSuccess) {
    auto sender = Sender::Open(test_filename, 0);
    EXPECT_NE(sender, nullptr);
}

TEST_F(SenderTest, OpenFailure) {
    auto sender = Sender::Open("nonexistent_file.bin", 0);
    EXPECT_EQ(sender, nullptr);
}

TEST_F(SenderTest, SignalReady) {
    auto sender = Sender::Open(test_filename, 0);
    EXPECT_TRUE(sender->SignalReady());
}

TEST_F(SenderTest, SendMessageSuccess) {
    auto sender = Sender::Open(test_filename, 0);
    string test_msg = "Hello";
    EXPECT_TRUE(sender->SendMsg(test_msg));
}

TEST_F(SenderTest, SendMessageTooLong) {
    auto sender = Sender::Open(test_filename, 0);
    string long_msg(MAX_MESSAGE_LENGTH * 2, 'a');
    EXPECT_TRUE(sender->SendMsg(long_msg));
}