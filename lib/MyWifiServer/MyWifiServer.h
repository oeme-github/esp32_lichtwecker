#pragma once

#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <filelib.h>
#include <debuglib.h>
#include <MyConfigServer.h>
#include <lw_config.h>

class MyWifiServer
{
private:
    /* data */
    WiFiManager wm;
    MyConfigServer *configServer;
    boolean isConfigLoaded = false;
    boolean isRegToMDNS    = false;

    boolean loadConfig();

public:
    MyWifiServer(){};
    ~MyWifiServer(){};

    boolean connectWifi();
    boolean regToMDNS();
    void resetWifi();
};

