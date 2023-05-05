#include "MyWebServer.h"

#define DEBUG 1

int fileSize = 0;

/**
 * @brief  humanReadableSize(const size_t bytes)
 * @note   returns a Sting with humanreadable size of memory
 * @param  bytes: const size 
 * @retval String
 */
String humanReadableSize(const size_t bytes) 
{
    dbSerialPrintln("** humanReadableSize() **");
    if (bytes < 1024) 
        return String(bytes) + " B";
    else if (bytes < (1024 * 1024)) 
        return String(bytes / 1024.0) + " kB";
    else if (bytes < (1024 * 1024 * 1024)) 
        return String(bytes / 1024.0 / 1024.0) + " MB";
    else 
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

/**
 * @brief  processor(const String& var)
 * @note   returns a String to a given const String& var
 * @param  var: const String& reference
 * @retval String
 */
String processor(const String& var) 
{
    dbSerialPrint("processor("); 
    dbSerialPrint(var); 
    dbSerialPrintln(")");
    
    String retString = "";
    if( var == "TITLE" )
    {
        retString = (String)WB_TITLE;
    }
    if (var == "FIRMWARE") 
    {
        retString = "Firmware: " + (String)FIRMWARE_VERSION;
    }
    if (var == "SPIFFS_FREE") 
    {
        retString = humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
    }
    if (var == "SPIFFS_USED") 
    {
        retString = humanReadableSize(SPIFFS.usedBytes());
    }
    if (var == "SPIFFS_TOTAL") 
    {
        retString = humanReadableSize(SPIFFS.totalBytes());
    }

    Serial.print("-> "); Serial.println(retString);
    return retString;
}

/**
 * @brief  listFiles(AsyncWebServerRequest * request)
 * @note   returns html code and integer
 * @param  request: pointer to AsyncWebServerRequest
 * @retval RetCode.msg - Text, RetCode.iRet - Errorcode 
 */
RetCode listFiles(AsyncWebServerRequest * request) 
{
    dbSerialPrintln("** listFiles()");
#ifdef DEBUG
    Serial.println("listFiles()");
    int args = request->args();
    for(int i=0;i<args;i++)
    {
        Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
#endif
    RetCode retCode;

    if (!request->hasArg("dir")) 
    {
        retCode.iRet = 500;
        retCode.msg  = "missing dir";
        return retCode;
    }
    if (!request->hasArg("fs"))
    {
        retCode.iRet = 500;
        retCode.msg  = "missing fs";
        return retCode;
    }

    String fs   = request->arg("fs"); 
    String path = request->arg("dir");
#ifdef DEBUG
    Serial.print("fs:  "); Serial.println(fs);
    Serial.print("dir: "); Serial.println(path);
#endif
    boolean bRoot = false;
    if( path == "" )
    {
        bRoot = true;
        String output;

        output = "[";
        output += "{\"type\":\"";
        output += "dir";
        output += "\",\"name\":\"";
        output += "/";
        output += "\"";
        output += "}";
        output += "]";
        
        retCode.iRet = 200;
        retCode.msg  = output;

        Serial.println(retCode.msg);
        return retCode;        
    }

    File dir;
    if (path != "/" && !SPIFFS.exists((char *)path.c_str())) 
    {
        retCode.iRet = 500;
        retCode.msg  = "BAD PATH";
        return retCode;
    }
    dir = SPIFFS.open((char *)path.c_str());

    path = String();
    if (!dir.isDirectory()) 
    {
        dir.close();
        retCode.iRet = 500;
        retCode.msg  = "NOT DIR";
    }
    dir.rewindDirectory();
    
    retCode.iRet = 200;
    retCode.msg  = "[";

    for (int cnt = 0; true; ++cnt) 
    {
        File entry = dir.openNextFile();
        if (!entry){
            break;
        }

        String output;
        if (cnt > 0) {
            output = ',';
        }

        output += "{\"type\":\"";
        output += (entry.isDirectory()) ? "dir" : "file";
        output += "\",\"name\":\"";
        output += entry.path();
        output += "\"";
        output += "}";

        retCode.msg += output;
        entry.close();
    }
    retCode.msg += "]";
    dir.close();
#ifdef DEBUG
    Serial.println(retCode.msg);
#endif
    return retCode;
}

/**
 * @brief  handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) 
 * @note   handler for file upload
 * @param  *request: AsyncWebServerRequest
 * @param  filename: String
 * @param  index: size_t
 * @param  *data: uint8_t
 * @param  len: size_t
 * @param  final: bool
 * @retval None
 */
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) 
{
#ifdef DEBUG
    Serial.println("handleFileUpload()");

    int args = request->args();
    for(int i=0;i<args;i++)
    {
        Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
    int params = request->params();
    for(int i=0;i<params;i++)
    {
        AsyncWebParameter* p = request->getParam(i);
        if( p->isFile() )
        { 
            //p->isPost() is also true
            Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } 
        else if( p->isPost() )
        {
            Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } 
        else 
        {
            Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }    
#endif
    String fsName    = request->getParam("fs")->value();
    fs::FS fs        = SPIFFS;
    size_t freespace = SPIFFS.totalBytes()-SPIFFS.usedBytes();

    if (!index) 
    {
      request->_tempFile = fs.open("/" + filename, "w");
    }

    if (len) {
      request->_tempFile.write(data, len);
    }

    if (final) {
      request->_tempFile.close();
      request->redirect("/");
    }
 }

/**
 * @brief  MyWebServer() constructor
 * @note   
 * @retval 
 */
MyWebServer::MyWebServer()
{
}

/**
 * @brief  MyWebServer() destructor
 * @note   
 * @retval 
 */
MyWebServer::~MyWebServer()
{
}

/**
 * @brief  startWebServer()
 * @note   startup for webserver -> loading config, connect to wifi, start webserver, register to MDNS
 * @retval None
 */
void MyWebServer::startWebServer()
{
    dbSerialPrintln("*** MyWebServer::startWebServer() ***");
    /*-----------------------------------------------------*/
    /* load config                                         */
    if(!this->loadConfig())
    {
        Serial.println("Failed to load config -> restart....");
        ESP.restart();
    }
    /*-----------------------------------------------------*/
    /* check if we should start the webserver              */
    if( this->configServer->getElement("websrv") == "0" )
    {
        Serial.println("start with no webserver!!!");
        vTaskDelete(NULL);
    }
    /*-----------------------------------------------------*/
    /* start web-server                                    */
    if(!this->begin())
    {
        Serial.println("Failed to start web-server -> restart....");
        ESP.restart();
    }
}

/**
 * @brief  loadConfig()
 * @note   creats a MyConfigServer and loads the config file
 * @retval true|false
 */
boolean MyWebServer::loadConfig()
{
    dbSerialPrintln("*** MyWebServer::loadConfig() ***");
    /*-----------------------------------------------------*/
    /* start filesystem                                    */
    if(!SPIFFS.begin())
    {
        dbSerialPrintln("An Error has occurred while mounting SPIFFS");
        return false;
    }
    else
    {
        this->configServer   = new MyConfigServer();
        this->configServer->loadConfig(&SPIFFS, WS_CONFIG_FILE, FileFormat::MAP);

        if( !this->configServer->isConfigLoaded() )
        {
            dbSerialPrintln("WARNING: config not loaded -> create one...");            
            this->createConfigJson();
        }
        this->isConfigLoaded = true;
    }
    return this->isConfigLoaded;
}

/**
 * @brief  begin()
 * @note   starts the webserver services
 * @retval true|false
 */
boolean MyWebServer::begin()
{
    dbSerialPrintln("*** MyWebServer::begin() ***");
    /*-----------------------------------------------------*/
    /* start the web-server                                */
    int port = std::stoi(this->configServer->getElement("port"));
    server = new AsyncWebServer(port);
    /*-----------------------------------------------------*/
    /* configure the server                                */
    server->onFileUpload(handleFileUpload);

    server->on( "/upload", 
                HTTP_POST, 
                [](AsyncWebServerRequest *request) { request->send(200);}, 
                handleFileUpload
    );
    server->on( "/", 
                HTTP_GET, 
                [&](AsyncWebServerRequest *request)
                {
                    request->send(SPIFFS, "/index.html", String(), false, processor);
                }
    );
    server->on( "/style.css", 
                HTTP_GET, 
                [&](AsyncWebServerRequest *request)
                {
                    request->send(SPIFFS, "/style.css","text/css");
                }
    );       
    server->on( "/logged-out", 
                HTTP_GET, 
                [&](AsyncWebServerRequest * request) 
                {
                    request->send(SPIFFS, "/logout.html", String(), false, processor);
                }
    );           
    server->on( "/list", 
                HTTP_GET, 
                [&](AsyncWebServerRequest * request)
                {
                    if (this->checkWebAuth(request)) 
                    {
                        RetCode retCode = listFiles(request);
                        request->send(retCode.iRet, "text/plain", retCode.msg );
                    } else {
                        return request->requestAuthentication();
                    }
                }
    );
    server->on( "/file", 
                HTTP_POST, 
                [&](AsyncWebServerRequest * request) 
                {
                    if (this->checkWebAuth(request)) 
                    {
                        if( request->hasArg("name") && request->hasArg("action") && request->hasArg("fs") ) 
                        {
                            String fsName     = request->arg("fs");
                            String fileName   = request->arg("name");
                            String fileAction = request->arg("action");
                            
                            fs::FS fs = SPIFFS;

                            if( fileAction == "delete" ) 
                            {
                                RetCode retCode = this->deleteRecursive(&fs, fileName);
                                request->send(retCode.iRet, "text/plain", retCode.msg);
                            } 
                            else if (fileAction == "create") 
                            {
                                RetCode retCode = this->createDir(&fs, fileName);
                                request->send(retCode.iRet, "text/plain", retCode.msg);
                            } 
                            else if(fileAction == "download") 
                            {
                                request->send(fs, fileName, "application/octet-stream");
                            }
                            else if(fileAction == "upload2nextion")
                            {
                                RetCode retCode = this->upload2nextion(&fs, fileName);
                                request->send(retCode.iRet, "text/plain", retCode.msg);
                            }
                            else 
                            {
                                request->send(400, "text/plain", "ERROR: invalid action param supplied");
                            }
                        }
                        else 
                        {
                            if( !request->hasParam("name") )
                                request->send(400, "text/plain", "ERROR: name required");
                            if( !request->hasParam("action") ) 
                                request->send(400, "text/plain", "ERROR: action required");
                            if( !request->hasParam("fs") )
                                request->send(400, "text/plain", "ERROR: fs required");
                        }
                    } 
                    else 
                    {
                        return request->requestAuthentication();
                    }
                }
    );
    server->onNotFound( [&](AsyncWebServerRequest *request) 
                        {
                            if (request->method() == HTTP_OPTIONS) 
                            {
                                request->send(200);
                            } 
                            else 
                            {
                                /* try to load from fs, else return 404 */
                                if( !this->loadFromFS(request) )
                                    request->send(404);
                            }
                        }

    );
    /*-----------------------------------------------------*/
    /* start web-server                                    */
    server->begin();
    return true;
}

/**
 * @brief  checkWebAuth(AsyncWebServerRequest * request) 
 * @note   check if the user is authenticated
 * @param  request: pointer to AsyncWebServerRequest
 * @retval 
 */
bool MyWebServer::checkWebAuth(AsyncWebServerRequest * request) 
{
    dbSerialPrintln("*** MyWebServer::checkWebAuth() ***");
    bool isAuthenticated = false;
    if( request->authenticate( this->configServer->getElement("user").c_str(), this->configServer->getElement("passwd").c_str() )) 
    {
        isAuthenticated = true;
    }
    return isAuthenticated;
}

/**
 * @brief  loadFromFS(AsyncWebServerRequest *request)
 * @note   loads file from filesystem and sends it to the browser
 * @param  *request: pointer to AsyncWebServerRequest
 * @retval true|false
 */
bool MyWebServer::loadFromFS(AsyncWebServerRequest *request) 
{
    dbSerialPrintln("*** MyWebServer::loadFromFS() ***");
    String path     = request->url();
    String dataType = "text/plain";
    
    if (path.endsWith("/")) 
    {
        path += "index.html";
    }

    if (path.endsWith(".src")){ path = path.substring(0, path.lastIndexOf(".")); } 
    else if (path.endsWith(".htm")) { dataType = "text/html"; } 
    else if (path.endsWith(".css")) { dataType = "text/css";  } 
    else if (path.endsWith(".js"))  { dataType = "application/javascript"; } 
    else if (path.endsWith(".png")) { dataType = "image/png"; } 
    else if (path.endsWith(".gif")) { dataType = "image/gif"; } 
    else if (path.endsWith(".jpg")) { dataType = "image/jpeg"; } 
    else if (path.endsWith(".ico")) { dataType = "image/x-icon"; } 
    else if (path.endsWith(".xml")) { dataType = "text/xml"; } 
    else if (path.endsWith(".pdf")) { dataType = "application/pdf"; } 
    else if (path.endsWith(".zip")) { dataType = "application/zip"; }
    else if (path.endsWith(".mp3")) { dataType = "audio/mpeg"; }
    else if (path.endsWith(".mp4")) { dataType = "audio/mpeg"; }
    else if (path.endsWith(".ogg")) { dataType = "audio/ogg"; }
    else if (path.endsWith(".wav")) { dataType = "audio/wav"; }

    File dataFile;
    dataFile = SPIFFS.open(path.c_str());
    if (dataFile.isDirectory()) 
    {
        path += "/index.html";
        dataType = "text/html";
        dataFile = SPIFFS.open(path.c_str());
    }
    /* if we havn't found any */
    if (!dataFile) 
    {
        return false;
    }

    if( request->hasArg("download") )
        dataType = "application/octet-stream";

    /* we send the file */
    request->send(SPIFFS, path);
    /* clean up */
    dataFile.close();
    return true;
}

/**
 * @brief  printAllParams(AsyncWebServerRequest *request)
 * @note   debug function, prints request parameters
 * @param  *request: pointer to AsyncWebServerRequest
 * @retval None
 */
void MyWebServer::printAllParams(AsyncWebServerRequest *request)
{
#ifdef DEBUG
    Serial.println("printAllParams()");

    int params = request->params();
    for(int i=0;i<params;i++)
    {
        AsyncWebParameter* p = request->getParam(i);
        if( p->isFile() )
        { 
            //p->isPost() is also true
            Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } 
        else if( p->isPost() )
        {
            Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } 
        else 
        {
            Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }    
#endif // DEBUG    
}

/**
 * @brief  printAllArgs(AsyncWebServerRequest *request)
 * @note   debug function, prints request args
 * @param  *request: pointer to AsyncWebServerRequest
 * @retval None
 */
void MyWebServer::printAllArgs(AsyncWebServerRequest *request)
{
#ifdef DEBUG
    Serial.println("printAllArgs()");
    //List all parameters
    int args = request->args();
    for(int i=0;i<args;i++)
    {
        Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    }
#endif // DEBUG    
}

/**
 * @brief  deleteRecursive(fs::FS *fs_, String path)
 * @note   function to delete files / directories
 * @param  *fs_: 
 * @param  path: 
 * @retval RetCode.msg - text, RetCode.iRet - errorcode 
 */
RetCode MyWebServer::deleteRecursive(fs::FS *fs_, String path) 
{
    dbSerialPrintln("*** MyWebServer::deleteRecursive() ***");
    RetCode retCode;
    retCode.iRet = 200;
    retCode.msg  = path+" deleted.";

    if( !fs_->exists( path ) ) 
    {
        retCode.iRet = 400;
        retCode.msg  = "ERROR: file does not exist";
        return retCode;
    } 
    File file = fs_->open((char *)path.c_str());
    if (!file.isDirectory()) 
    {
        file.close();
        fs_->remove((char *)path.c_str());
        return retCode;
    }

    file.rewindDirectory();
    while (true) 
    {
        File entry = file.openNextFile();
        if (!entry) 
        {
            break;
        }
        String entryPath = path + "/" + entry.name();
        if (entry.isDirectory()) 
        {
            entry.close();
            deleteRecursive(fs_, entryPath);
        } 
        else 
        {
            entry.close();
            fs_->remove((char *)entryPath.c_str());
        }
        yield();
    }
    fs_->rmdir((char *)path.c_str());
    file.close();
    return retCode;
}

/**
 * @brief  createDir(fs::FS *fs_, String path)
 * @note   creats a directory
 * @param  *fs_: pointer to filesystem
 * @param  path: String
 * @retval RetCode.msg - text, RetCode.iRet - errorcode
 */
RetCode MyWebServer::createDir(fs::FS *fs_, String path)
{
    dbSerialPrintln("*** MyWebServer::createDir() ***");
    RetCode retCode;
    retCode.iRet = 200;
    retCode.msg  = path+" created.";

    if( path.substring(path.length()-1) == "/" )
    {
        path = path.substring(0, path.length()-1);
    }

    if( fs_->exists(path) )
    {
        retCode.iRet = 400;
        retCode.msg  = path+" allready exists.";
    }
    else if(!fs_->mkdir(path))
    {
        retCode.iRet = 400;
        retCode.msg  = "Couldn't create "+path;
    } 
    return retCode;
}

/**
 * @brief  createConfigJson()
 * @note   creates the json config and saves it to file (config.json)
 * @retval None
 */
void MyWebServer::createConfigJson()
{
    dbSerialPrintln("*** MyWebServer::createConfigJson() ***");
    this->configServer->putElement("version", "v4.0.1"     );
    this->configServer->putElement("port"   , "80"         );
    this->configServer->putElement("user"   , "admin"      );
    this->configServer->putElement("passwd" , "*admin!"    );
    this->configServer->putElement("title"  , "Lichtwecker");
    this->configServer->putElement("websrv" , "1"          );

    this->configServer->saveToConfigfile();
}

RetCode MyWebServer::upload2nextion(fs::FS *fs_, String path)
{
    RetCode retCode;
    bool updated = false;
    /*-----------------------------------------------------*/
    /* get size of file                                    */
    File file = fs_->open(path, "r");
    if(file)
    {
        dbSerialPrintln("File opened. Update Nextion...");
        bool result;
        ESPNexUpload nextion(115200);
        
        nextion.setUpdateProgressCallback([](){ Serial.print("."); });

        int fileSize = file.size();
        result = nextion.prepareUpload(fileSize);
        /* -------------------------------------------------- */
        /* handle send errors                                 */  
        if(!result)
        {
            /* ---------------------------------------------- */
            retCode.msg  = nextion.statusMessage;
            retCode.iRet = 303;
            return retCode;
        }
        else
        {
            dbSerialPrintf( "Start upload. File size is: %i bytes", fileSize);
            result = nextion.upload(file);   
            if(result)
            {
                retCode.iRet = 200;
                retCode.msg  = "Succesfully send file to Nextion!";
            }
            else
            {
                retCode.iRet = 303;
                retCode.msg  = "Error updating Nextion: " + nextion.statusMessage;
            }
            // end: wait(delay) for the nextion to finish the update process, send nextion reset command and end the serial connection to the nextion
            nextion.end();
        }
        // close the file:
        file.close();
    }
    else
    {
        // if the file didn't open, print an error:
        retCode.msg  = "ERROR: can not open " + path;
        retCode.iRet = 303;
    }
    return retCode;
}