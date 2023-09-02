#pragma once

#include <deflib.h>
#include <libDebug.h>

class MyQueueHandler
{
private:
    /* data */
    xQueueHandle hQueue;

public:
    MyQueueHandler();
    ~MyQueueHandler();

    void setQueue(xQueueHandle hQueue_);
    RetCode sendToQueue(std::string msg_);
    RetCode readFromQueue();
};
