#include "receiver/receiver.h"
#include "sender/sender.h"
#include <iostream>
#include <string>
#include <cstring>
#include <windows.h>

enum ProgramMode {
    MODE_RECEIVER,
    MODE_SENDER
};

void RunAsReceiver(int argc, char* argv[]) {
    std::string fileName;
    size_t numRecords;
    int numSenders;

    std::cout << "=== RECEIVER MODE ===" << std::endl;
    std::cout << "Enter binary file name: ";
    std::cin >> fileName;
    std::cout << "Enter number of records in file: ";
    std::cin >> numRecords;

    Receiver* receiver = Receiver::Create(fileName, numRecords);
    if (!receiver) {
        std::cerr << "Failed to create receiver" << std::endl;
        return;
    }

    std::cout << "Enter number of sender processes: ";
    std::cin >> numSenders;

    if (!receiver->LaunchSenders(numSenders)) {
        std::cerr << "Failed to launch sender processes" << std::endl;
        delete receiver;
        return;
    }

    if (!receiver->WaitForSenders()) {
        std::cerr << "Failed to wait for sender processes" << std::endl;
        delete receiver;
        return;
    }

    receiver->RunCommandLoop();
    delete receiver;
}

void RunAsSender(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " sender <filename> <sender_index>" << std::endl;
        return;
    }

    std::string fileName = argv[2];
    int senderIndex = std::stoi(argv[3]);

    std::cout << "=== SENDER " << senderIndex << " MODE ===" << std::endl;
    std::cout << "Sender " << senderIndex << " started with file: " << fileName << std::endl;

    Sender* sender = Sender::Open(fileName, senderIndex);
    if (!sender) {
        std::cerr << "Failed to open sender" << std::endl;
        return;
    }

    if (!sender->SignalReady()) {
        std::cerr << "Failed to signal ready" << std::endl;
        delete sender;
        return;
    }

    std::string dummy;
    std::getline(std::cin, dummy);

    sender->RunCommandLoop();
    delete sender;
}

int main(int argc, char* argv[]) {
    ProgramMode mode = MODE_RECEIVER;

    if (argc > 1) {
        if (strcmp(argv[1], "sender") == 0) {
            mode = MODE_SENDER;
        }
    }

    switch (mode) {
    case MODE_RECEIVER:
        RunAsReceiver(argc, argv);
        break;
    case MODE_SENDER:
        RunAsSender(argc, argv);
        break;
    }

    return 0;
}