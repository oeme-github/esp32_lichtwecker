#pragma once

#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <libFile.h>
#include <libDebug.h>
#include <MyConfigServer.h>
#include <libConfig.h>
#include <MyQueueHandler.h>


class MyWifiServer
{
private:
    /* data */
    WiFiManager wm;
    MyConfigServer *configServer;

    MyQueueHandler myQueueHandler;

    boolean loadConfig();

public:
    MyWifiServer(){};
    ~MyWifiServer(){};

    boolean connectWifi();
    boolean regToMDNS();
    void resetWifi();
    boolean connected();
    boolean reconnect();

    void setQueue(xQueueHandle hQueue_){this->myQueueHandler.setQueue(hQueue_);}
    void sendToQueue(std::string msg_){this->myQueueHandler.sendToQueue(msg_);}

};

