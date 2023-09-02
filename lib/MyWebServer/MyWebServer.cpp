#include <MyWebServer.h>

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
 * @brief  loadWebServer()
 * @note   startup for webserver -> loading config, connect to wifi, start webserver, register to MDNS
 * @retval std::pair<int, String>
 */
RetCode MyWebServer::loadWebServer(FS *fs)
{
  RetCode retCode;
  /*-----------------------------------------------------*/
  /* init                                                */
  this->fs = fs;
  if( existsOnFs(fs, "/index.html") )
    this->hasFS = true;
  else
    this->hasFS = false;
  /*-----------------------------------------------------*/
  /* load config                                         */
  retCode = this->loadConfig();
  if(retCode.first != 0)
  {
      return retCode;
  }
  /*-----------------------------------------------------*/
  /* check if we should start the webserver              */
  if( this->configServer->containsKey("websrv") && this->configServer->getElement("websrv") == "0" )
  {
      dbSerialPrintln("WARNING: webser config websrv = 0 !!!");
      vTaskDelete(NULL);
  }
  /*-----------------------------------------------------*/
  /* start web-server                                    */ 
  retCode = this->begin();
  if(retCode.first != 0)
  {
      return retCode;
  }
  return retCode;
}

/**
 * @brief  loadConfig()
 * @note   creats a MyConfigServer and loads the config file
 * @retval std::pair<int, String>
 */
RetCode MyWebServer::loadConfig()
{
  RetCode retCode;
  retCode.first  = 0;
  retCode.second = "";
  /*-----------------------------------------------------*/
  /* check if configfile exists                          */
  if(!this->fs->exists(WS_CONFIG_FILE))
  {
    retCode.first  = -1;
    retCode.second = "ERROR: Can not Load config.";
  }
  else
  {
    /*---------------------------------------------------*/
    /* load config                                       */
    this->configServer = new MyConfigServer();
    
    if(!this->configServer->loadConfig(this->fs, WS_CONFIG_FILE))
    {
      retCode.first = -1;
      retCode.second = "ERROR: Can not Load config.";
    }
  }
  return retCode;
}

/**
 * @brief  begin()
 * @note   starts the webserver services
 * @retval std::pair<int, String>
 */
RetCode MyWebServer::begin()
{
    RetCode retCode;
    retCode.first  = 0;
    retCode.second = "";
    /*-----------------------------------------------------*/
    /* start the web-server                                */
    int port = std::stoi(this->configServer->getElement("port"));
    this->server = new WebServer(port);
    /*-----------------------------------------------------*/
    /* configure the server                                */
    this->server->on("/list"    , HTTP_GET   , [&]() {this->printDirectory();}                                   );
    this->server->on("/edit"    , HTTP_DELETE, [&]() {this->handleDelete();  }                                   );
    this->server->on("/edit"    , HTTP_PUT   , [&]() {this->handleCreate();  }                                   );
    this->server->on("/edit"    , HTTP_POST  , [&]() {this->returnOK("");    }, [&]() {this->handleFileUpload();});
    this->server->on("/getValue",HTTP_GET    , [&]() {this->handleGetValue();}                                   );
    this->server->onNotFound( [&]() { this->handleNotFound(); });
    /*-----------------------------------------------------*/
    /* start web-server                                    */
    server->begin();
    return retCode;
}


void MyWebServer::returnOK(String msg) {
  this->server->send(200, "text/plain", msg.c_str());
}

// 404, "text/plain", message
void MyWebServer::returnFail( uint16_t iRet, String type, String msg) 
{
  this->server->send(iRet, type, msg + "\r\n");
}


void MyWebServer::printDirectory() 
{
  if (!this->server->hasArg("dir")) {
    return returnFail(500, "text/plain", "BAD ARGS");
  }
  String path = this->server->arg("dir");
  if (path != "/" && !this->fs->exists((char *)path.c_str())) {
    return returnFail(500, "text/plain", "BAD PATH");
  }
  File dir = this->fs->open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail(500, "text/plain", "NOT DIR");
  }
  dir.rewindDirectory();
  this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
  this->server->send(200, "text/json", "");
  WiFiClient client = this->server->client();

  this->server->sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
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
    this->server->sendContent(output);
    entry.close();
  }
  this->server->sendContent("]");
  dir.close();
}

void MyWebServer::handleDelete() 
{
  if (this->server->args() == 0) 
  {
    return returnFail(500, "text/plain", "BAD ARGS");
  }
  String path = this->server->arg(0);
  if (path == "/" || !this->fs->exists((char *)path.c_str())) 
  {
    return returnFail(500, "text/plain", "BAD PATH");
  }
  deleteRecursive(this->fs, path.c_str());
  return returnOK( path + " deleted");
} 

void MyWebServer::handleCreate() 
{
  if (this->server->args() == 0) 
  {
    return returnFail(500, "text/plain", "BAD ARGS");
  }
  String path = this->server->arg(0);
  if (path == "/" || this->fs->exists((char *)path.c_str())) 
  {
    return returnFail(500, "text/plain", "BAD PATH");
  }

  if (path.indexOf('.') > 0) 
  {
    writeFile(this->fs, path.c_str());
  } 
  else 
  {
    makeDir(this->fs, path.c_str());
  }
  return returnOK(path+" created");
}


void MyWebServer::handleFileUpload() 
{
  if (this->server->uri() != "/edit") 
  {
    return returnFail(500, "text/plain", "BAD PARAM, should be /edit");
  }
  HTTPUpload& upload = this->server->upload();
  if(upload.status == UPLOAD_FILE_START) 
  {
    if(existsOnFs( this->fs, upload.filename.c_str())) 
    {
      deleteRecursive(this->fs, upload.filename.c_str());
    }
    this->uploadFile = this->fs->open(upload.filename.c_str(), FILE_WRITE);
    dbSerialPrintf("Upload: START, filename: %s", upload.filename.c_str());
  } 
  else if(upload.status == UPLOAD_FILE_WRITE) 
  {
    if(this->uploadFile)
    {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    dbSerialPrintf("Upload: WRITE, Bytes: %i", upload.currentSize);
  } 
  else if(upload.status == UPLOAD_FILE_END) 
  {
    if(this->uploadFile)
    {
      uploadFile.close();
    }
    dbSerialPrintf("Upload: END, Size: %i", upload.totalSize);
  }
  return;
}


void MyWebServer::handleGetValue()
{
    String msg;

    if(!this->server->hasArg("param"))
    {
        return returnFail(500, "text/plain", "BAD PARAM - Missing param");
    }
    String var = this->server->arg("param");

    if( var == "TITLE" )
    {
        if(!this->configServer->containsKey("title"))
            msg = (String)WB_TITLE;
        else
            msg = this->configServer->getElement("title").c_str();
    }
    else if (var == "FIRMWARE") 
    {
        if(!this->configServer->containsKey("version"))
            msg = "Firmware: " + (String)FIRMWARE_VERSION;
        else
            msg = this->configServer->getElement("version").c_str();
    }
#ifdef WITH_SPIFFS
    else if (var == "SPIFFSFREE") 
    {
      msg = humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
    }
    else if (var == "SPIFFSUSED") 
    {
        msg = humanReadableSize(SPIFFS.usedBytes());
    }
    else if (var == "SPIFFSTOTAL") 
    {
        msg = humanReadableSize(SPIFFS.totalBytes());
    }
#endif // DEBUG
#ifdef WITH_SD
    else if (var == "SDFREE") 
    {
      msg = humanReadableSize((SD.totalBytes() - SD.usedBytes()));
    }
    else if (var == "SDUSED") 
    {
        msg = humanReadableSize(SD.usedBytes());
    }
    else if (var == "SDTOTAL") 
    {
        msg = humanReadableSize(SD.totalBytes());
    }
#endif // DEBUG
    else
    {
        return returnFail(500, "text/plain", "BAD PARAM - Unknown");
    }
    return returnOK(msg); 
}

void MyWebServer::handleNotFound() 
{

  if (this->hasFS && this->loadFromFS(this->server->uri())) 
  {
    return;
  }
  String message = "FILESYSTEM: Not Detected\n\n";
  message += "URI: ";
  message += this->server->uri();
  message += "\nMethod: ";
  message += (this->server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += this->server->args();
  message += "\n";
  for(uint8_t i = 0; i < this->server->args(); i++) 
  {
    message += " NAME:" + this->server->argName(i) + "\n VALUE:" + this->server->arg(i) + "\n";
  }
  return returnFail(404, "text/plain", message);
}

bool MyWebServer::loadFromFS(String path) 
{
  String dataType = "text/plain";
  if(path.endsWith("/")) 
  {
    path += "index.html";
  }

  if(path.endsWith(".src")) 
  {
    path = path.substring(0, path.lastIndexOf("."));
  }
  else if(path.endsWith(".html"))
  {
    dataType = "text/html";
  } 
  else if(path.endsWith(".htm")) 
  {
    dataType = "text/html";
  } 
  else if(path.endsWith(".css")) 
  {
    dataType = "text/css";
  } 
  else if(path.endsWith(".js")) 
  {
    dataType = "application/javascript";
  } 
  else if(path.endsWith(".png")) 
  {
    dataType = "image/png";
  } 
  else if(path.endsWith(".gif")) 
  {
    dataType = "image/gif";
  } 
  else if(path.endsWith(".jpg")) 
  {
    dataType = "image/jpeg";
  } 
  else if(path.endsWith(".ico")) 
  {
    dataType = "image/x-icon";
  } 
  else if(path.endsWith(".xml")) 
  {
    dataType = "text/xml";
  } 
  else if(path.endsWith(".pdf")) 
  {
    dataType = "application/pdf";
  } 
  else if(path.endsWith(".zip")) 
  {
    dataType = "application/zip";
  }

  File dataFile = this->fs->open(path.c_str());
  if(dataFile.isDirectory()) 
  {
    path += "/index.html";
    dataType = "text/html";
    dataFile = this->fs->open(path.c_str());
  }

  if(!dataFile) 
  {
    return false;
  }

  if(this->server->hasArg("download")) 
  {
    dataType = "application/octet-stream";
  }

  if(this->server->streamFile(dataFile, dataType) != dataFile.size()) 
  {
    dbSerialPrintf("WARNING: %s", "Sent less data than expected!");
  }
  dataFile.close();
  return true;
}
