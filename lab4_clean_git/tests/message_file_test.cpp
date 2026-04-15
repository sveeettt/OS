#include "common/message_file.h"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <cstdio>
#include <sys/stat.h>
#include <mutex>

const std::string TEST_FILENAME = "test_messages.bin";
const size_t TEST_NUM_RECORDS = 50;

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void removeFile(const std::string& filename) {
    std::remove(filename.c_str());
}

class MessageFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (fileExists(TEST_FILENAME)) {
            removeFile(TEST_FILENAME);
        }
        MessageFile::Create(TEST_FILENAME, TEST_NUM_RECORDS);
    }

    void TearDown() override {
        if (fileExists(TEST_FILENAME)) {
            removeFile(TEST_FILENAME);
        }
    }
};

TEST_F(MessageFileTest, CreateAndOpen) {
    MessageFile* messageFile = MessageFile::Open(TEST_FILENAME);
    ASSERT_TRUE(messageFile != nullptr);
    EXPECT_TRUE(messageFile->IsEmpty());
    EXPECT_FALSE(messageFile->IsFull());
    delete messageFile;
}

TEST_F(MessageFileTest, WriteAndReadMessage) {
    MessageFile* messageFile = MessageFile::Open(TEST_FILENAME);
    std::string testMessage = "Test message";
    EXPECT_TRUE(messageFile->WriteMessage(testMessage));
    EXPECT_FALSE(messageFile->IsEmpty());
    std::string readMessage = messageFile->ReadMessage();
    EXPECT_EQ(testMessage, readMessage);
    EXPECT_TRUE(messageFile->IsEmpty());
    delete messageFile;
}

TEST_F(MessageFileTest, FillingFile) {
    MessageFile* messageFile = MessageFile::Open(TEST_FILENAME);
    for (size_t i = 0; i < TEST_NUM_RECORDS; ++i) {
        std::string message = "Message " + std::to_string(static_cast<int>(i));
        EXPECT_TRUE(messageFile->WriteMessage(message));
    }
    EXPECT_TRUE(messageFile->IsFull());
    for (size_t i = 0; i < TEST_NUM_RECORDS; ++i) {
        std::string message = messageFile->ReadMessage();
        EXPECT_FALSE(message.empty());
    }
    EXPECT_TRUE(messageFile->IsEmpty());
    delete messageFile;
}

TEST_F(MessageFileTest, MultiThreaded) {
    MessageFile* messageFile = MessageFile::Open(TEST_FILENAME);
    std::vector<std::thread> threads;
    std::mutex fileMutex;

    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([messageFile, i, &fileMutex]() {
            for (int j = 0; j < 10; ++j) {
                std::string message = "Thread " + std::to_string(i) + " message " + std::to_string(j);
                {
                    std::lock_guard<std::mutex> lock(fileMutex);
                    messageFile->WriteMessage(message);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            });
    }

    for (int i = 0; i < 3; ++i) {
        threads.emplace_back([messageFile, &fileMutex]() {
            int successfulReads = 0;
            while (successfulReads < 10) {
                std::string message;
                {
                    std::lock_guard<std::mutex> lock(fileMutex);
                    message = messageFile->ReadMessage();
                }
                if (!message.empty()) {
                    ++successfulReads;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_TRUE(messageFile->IsEmpty());
    delete messageFile;
}