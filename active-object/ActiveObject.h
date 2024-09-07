#pragma once
#include "IMessage.h"
#include <stdlib.h>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>

// Aktives Objekt
class ActiveObject
{
public:
    ActiveObject();
    ~ActiveObject();
    // Nachricht senden
    void send(std::unique_ptr<Message> msg);

    // Aktives Objekt beenden
    void WaitAndStop();

    bool isActive() const;

    int getThreadID() const;

private:
    void run();
    std::thread thread;
    std::queue<std::unique_ptr<Message>> messages;
    std::mutex mutex;
    std::condition_variable condition;
    std::atomic_bool running;
    std::atomic<bool> active;
    const int threadID;
};