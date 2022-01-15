#include <iostream>
#include <string>

#include <SoftwareSerial.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>

#include <SimpleSun.h>
#include <NextionDisplay.h>
#include <ESPNexUpload.h>
#include <MyDFPlayer.h>
#include <WebServer.h>
#include "MDispatcher.h"

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
    MDispatcher<String, EventEnum> dispatcher;
    
    ESPNexUpload espNexUpload;
    WebServer webServer;

    /* WIFI */
    const char* ssid      = SSID_NAME;
    const char* password  = WIFI_PWD;
    const char* host      = HOST_NAME;     

private:
    /**
     * @brief print for string
     * 
     * @param str 
     * @param bNewLine 
     */
    static void print(const std::string &str, bool bNewLine = true) 
    {
        if( bNewLine )
            dbSerialPrintln(str.c_str());
        else
            dbSerialPrint(str.c_str());
    }
    /**
     * @brief print for int
     * 
     * @param iNum 
     * @param bNewLine 
     */
    static void print(int iNum, bool bNewLine = false) 
    { 
        if( bNewLine )
            dbSerialPrintln(iNum);
        else
            dbSerialPrintln(iNum);
    }

public:
    /**
     * @brief Construct a new Lichtwecker object
     * 
     */
    Lichtwecker();
    /**
     * @brief Destroy the Lichtwecker object
     * 
     */
    ~Lichtwecker();
    
private:
    /**
     * @brief start sequence
     * 
     * @return true 
     * @return false 
     */
    bool startFS();
    /**
     * @brief start filesystem
     * 
     * @param ssid_ 
     * @param password_ 
     * @param host_ 
     * @return true 
     * @return false 
     */
    bool startWifi(const char *ssid_, const char *password_, const char *host_ );
    /**
     * @brief Get the Content Type object
     * 
     * @param filename 
     * @return String 
     */
    String getContentType(String filename);
    /**
     * @brief webserver task
     * 
     * @param pvParameters 
     */
    void taskWebServerCode( void * pvParameters );

public:
    /**
     * @brief start sequence
     * 
     */
    void start();
    /**
     * @brief get pointer to DFPlayer
     * 
     * @return MyDFPlayer* 
     */
    MyDFPlayer *getDFPlayer()
    { 
        return &myDFPlayer; 
    }
    /**
     * @brief Get the Simple Sun object
     * 
     * @return SimpleSun* 
     */
    SimpleSun *getSimpleSun()
    { 
        return &simpleSun; 
    }
    /**
     * @brief Get the Nextion Display object
     * 
     * @return NextionDisplay* 
     */
    NextionDisplay *getNextionDisplay()
    { 
        return &nextionDisplay; 
    }
    /**
     * @brief Get the Nex Rtc object
     * 
     * @return NexRtc* 
     */
    NexRtc *getNexRtc()
    { 
        return &nexRtc; 
    }
    /**
     * @brief Get the Web Server object
     * 
     * @return WebServer* 
     */
    WebServer *getWebServer()
    {
        return &webServer;
    }
    /**
     * @brief Get the Esp Nex Upload object
     * 
     * @return ESPNexUpload* 
     */
    ESPNexUpload *getEspNexUpload()
    {
        return &espNexUpload;
    }
    /**
     * @brief broadcast message
     * 
     * @param msg_ 
     */
    void broadcastMessage( const char* msg_ )
    { 
        dispatcher.broadcast(msg_, EVENT1); 
    }
    /**
     * @brief handle file read
     * 
     * @param path 
     * @return true 
     * @return false 
     */
    bool handleFileRead(String path);

};


#endif