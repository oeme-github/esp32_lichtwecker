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


typedef struct
{ 
    int iRet; 
    String msg; 
} RetCode;


class MyWebServer
{
private:
    /* data */
    WebServer *server;
    MyConfigServer *configServer;

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

};

