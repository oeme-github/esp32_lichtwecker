#pragma once

#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <libFile.h>
#include <libDebug.h>
#include <MyConfigServer.h>
#include <libConfig.h>

class MyWifiServer
{
private:
    /* data */
    WiFiManager wm;
    MyConfigServer *configServer;

    boolean loadConfig();

public:
    MyWifiServer(){};
    ~MyWifiServer(){};

    boolean connectWifi();
    boolean regToMDNS();
    void resetWifi();
};

