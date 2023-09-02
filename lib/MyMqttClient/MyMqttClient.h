#include <libConfig.h>

#include <FS.h>
#ifdef WITH_SPIFFS
    #include <SPIFFS.h>
#endif // DEBUG
#ifdef WITH_SD
    #include <SD.h>
#endif // DEBUG

#include <libDebug.h>
#include <libFile.h>
#include <MyConfigServer.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <MDispatcher.h>

#include <MyQueueHandler.h>

class MyMqttClient
{
private:
    /* data */
    MyConfigServer *configServer;
    PubSubClient *mqttClient;
    WiFiClient espClient;

    FS *fs;
    boolean hasFS;

    MyQueueHandler myQueueHandler;

public:
    MyMqttClient(/* args */);
    ~MyMqttClient();

    RetCode loadMqttClient(FS *fs);
    RetCode loadConfig();
    RetCode begin();

    boolean connected(){ return this->mqttClient->connected(); } ;
    void reconnect();
    void setQueue(xQueueHandle hQueue_){this->myQueueHandler.setQueue(hQueue_);}
    void sendToQueue(std::string msg_){this->myQueueHandler.sendToQueue(msg_);}
    RetCode readFromQueue(){ return this->myQueueHandler.readFromQueue();}
    RetCode sendMsg(std::string);

    const std::string currentDateTime();

};
