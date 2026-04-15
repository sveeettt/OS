#pragma once

#include <string>
#include "common/message_file.h"

class Sender {
public:
    static Sender* Open(const std::string& fileName, int senderIndex);
    ~Sender();

    bool SignalReady();
    bool SendMsg(const std::string& message);
    void RunCommandLoop();

private:
    Sender(const std::string& fileName, int senderIndex, MessageFile* messageFile);

    std::string m_fileName;
    int m_senderIndex;
    MessageFile* m_messageFile;
};