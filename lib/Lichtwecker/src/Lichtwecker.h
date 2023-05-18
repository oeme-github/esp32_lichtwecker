#pragma once

#include <iostream>
#include <string>
#include <SPIFFS.h>

#include <MDispatcher.h>
#include <SimpleSun.h>
#include <NextionDisplay.h>
#include <MyConfigServer.h>
#include <libConfig.h>

#ifndef LICHTWECKER_H
#define LICHTWECKER_H


class Lichtwecker
{
private:
    /* data */
    NextionDisplay nextionDisplay;
    NexRtc nexRtc;
    SimpleSun simpleSun;
    MDispatcher<String, EventEnum> dispatcher;
    
    MyConfigServer *configServer;
    
    boolean loadConfig();
    void createConfigJson();

    void setNextValue( const char * cNameElemet );
    void getNextValue( const char * cNameElemet );

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
    /**
     * @brief start sequence
     * 
     */
    void start();
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
     * @brief get MDispatcher object
     * 
     * @return MDispatcher<String, EventEnum>& 
     */
    MDispatcher<String, EventEnum> &getMDispatcher()
    {
        return dispatcher;
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

    void saveToConfigfile( const char *ptr);

    bool configContainsKey(const char *_index){ return this->configServer->containsKey(_index); };
    std::string configGetElement(const char *_index){ return this->configServer->getElement(_index); }
};


#endif