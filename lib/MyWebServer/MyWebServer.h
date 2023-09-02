#include <libConfig.h>

#include <FS.h>
#ifdef WITH_SPIFFS
    #include <SPIFFS.h>
#endif // DEBUG
#ifdef WITH_SD
    #include <SD.h>
#endif // DEBUG

#include <WebServer.h>
#include <libDebug.h>
#include <libFile.h>
#include <MyConfigServer.h>
#include <MyQueueHandler.h>

class MyWebServer
{
private:
    /* data */
    WebServer *server;
    MyConfigServer *configServer;

    MyQueueHandler myQueueHandler;

    FS *fs;
    boolean hasFS;
    int port;
    File uploadFile;
public:
    MyWebServer(/* args */);
    ~MyWebServer();

    RetCode loadWebServer(FS *fs);
    RetCode loadConfig();
    RetCode begin();

    void returnOK(String msg);
    void returnFail( uint16_t iRet, String type, String msg);

    void printDirectory();
    void handleDelete();
    void handleCreate();
    void handleFileUpload();
    void handleGetValue();
    void handleNotFound();
    bool loadFromFS(String path);

    void handleClient(){this->server->handleClient();}

    void setQueue(xQueueHandle hQueue_){this->myQueueHandler.setQueue(hQueue_);}
    void sendToQueue(std::string msg_){this->myQueueHandler.sendToQueue(msg_);}
};

