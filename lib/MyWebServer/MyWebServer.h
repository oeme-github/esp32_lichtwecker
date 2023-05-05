#pragma once

#include <lw_config.h>

#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>

#include <MyConfigServer.h>
#include <filelib.h>
#include <debuglib.h>

#include <ESPNexUpload.h>

typedef struct
{ 
    String msg; 
    int iRet; 
} RetCode;

class MyWebServer
{
private:
    /* data */
    AsyncWebServer *server;
    MyConfigServer *configServer;
    boolean hasSD = false;
    fs::FS *fs;
    boolean isConfigLoaded = false;
    int port;

    void createConfigJson();

public:
    MyWebServer();
    ~MyWebServer();

    void startWebServer();
    boolean loadConfig();
    boolean loadSD(fs::FS *fs_);
    boolean begin();
    bool checkWebAuth(AsyncWebServerRequest * request); 
    bool loadFromFS(AsyncWebServerRequest *request);

    void printAllParams(AsyncWebServerRequest *request);
    void printAllArgs(AsyncWebServerRequest *request);

    RetCode deleteRecursive(fs::FS *fs_, String path); 
    RetCode createDir(fs::FS *fs_, String path); 
    RetCode upload2nextion(fs::FS *fs_, String path);

};

