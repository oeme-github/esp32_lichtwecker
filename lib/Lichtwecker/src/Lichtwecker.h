#include <iostream>
#include <string>
#include <map>
#include <esp_log.h>

#include <Nextion.h>
#include <genericstate.h>
#include <SimpleSun.h>
#include <NextionDisplay.h>
#include <ESPNexUpload.h>
#include <SoftwareSerial.h>
#include <MyDFPlayer.h>
#include <WebServer.h>

#include <FS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#ifndef LICHTWECKER_H
#define LICHTWECKER_H

#ifndef SWS_RX
#define SWS_RX 18
#endif
#ifndef SWS_TX
#define SWS_TX 19
#endif
#ifndef WEB_SRV_PORT
#define WEB_SRV_PORT 80
#endif
#ifndef OFFSET_SUN
#define OFFSET_SUN 10
#endif
#ifndef HOST_NAME
#define HOST_NAME "nextion"
#endif
#ifndef SSID_NAME
#define SSID_NAME "KatzeUndHase2"
#endif
#ifndef WIFI_PWD
#define WIFI_PWD "*P4rar#012345678"
#endif
#ifndef GPIO_MP3
#define GPIO_MP3 34
#endif
#ifndef BL_DELAY
#define BL_DELAY 500
#endif

class Lichtwecker
{
private:
    /* data */
    MyDFPlayer myDFPlayer;
    MyDFPlayer *ptrMyDFPlayer = &myDFPlayer;
    NextionDisplay nextionDisplay;
    NexRtc nexRtc;
    SimpleSun simpleSun;
    
    // ESPNexUpload espNexUpload;
    // WebServer webServer;

    /* WIFI */
    const char* ssid      = SSID_NAME;
    const char* password  = WIFI_PWD;
    const char* host      = HOST_NAME;     
    /* SUN */
    int iOffsetSun        = OFFSET_SUN; 
    /* Upload Server */
    int fileSize  = 0;
    bool result   = true;

private:
    static void print(const std::string &str, bool bNewLine = true) {
        if( bNewLine )
            dbSerialPrintln(str.c_str());
        else
            dbSerialPrint(str.c_str());
    }
    static void print(int iNum, bool bNewLine = false) { 
        if( bNewLine )
            dbSerialPrintln(iNum);
        else
            dbSerialPrintln(iNum);
    }

public:
    Lichtwecker(/* args */);
    ~Lichtwecker();
    
private:
    bool startFS();
    bool startWifi(const char *ssid_, const char *password_, const char *host_ );

    String getContentType(String filename);
    bool handleFileRead(String path);
    bool handleFileUpload();

public:
    /* start functions */
    void start();
    /* getter and setter */
    MyDFPlayer *getDFPlayer(){ return &myDFPlayer; }
    //SimpleSun *getSimpleSun(){ return &simpleSun; }
    NextionDisplay *getNextionDisplay(){ return &nextionDisplay; }
    NexRtc *getNexRtc(){ return &nexRtc; }

    void setOffsetSun( int iOffset_) { iOffsetSun = iOffset_; }
    int  getOffsetSun( ) { return iOffsetSun; }

};


#endif