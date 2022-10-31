#include <Lichtwecker.h>

/**
 * @brief constructor
 * 
 */
Lichtwecker::Lichtwecker()
{
    print("Lichtwecker constructor");
}

/**
 * @brief Destroy the Lichtwecker:: Lichtwecker object
 * 
 */
Lichtwecker::~Lichtwecker()
{
}

/**
 * @brief start()
 * 
 */
void Lichtwecker::start()
{
    print("\n --- Startup Lichtwecker --- \n");
    /* -------------------------------------------------- */
    /* start filesystem                                   */
    print("Start FS");
    startFS();
    vTaskDelay(200/portTICK_PERIOD_MS);
    /* ---------------------------------------------- */
    /* debug the config                               */
    this->printConfig();   
    /* -------------------------------------------------- */
    /* start WIFI                                         */
    print("Start WIFI");
    startWifi(ssid, password, host );
    vTaskDelay(100/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* start nextion                                      */
    print("Init Nextion...");
    nextionDisplay.InitDisplay();
    vTaskDelay(100/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* RTC                                                */
    if( nexRtc.updateDateTime() )
    {
      print("Date time updated.");
    }
    else
    {
      print("updateDateTime went wrong!");
    }
    /* ---------------------------------------------- */
    /* set parameter                                  */
    getNextionDisplay()->getNexPageByName("page1")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nHour1" )->setValue((uint32_t)this->config.getMember("Hour1") );
    this->getNextionDisplay()->getNexNumberByName("nMin1"  )->setValue((uint32_t)this->config.getMember("Min1")  );
    this->getNextionDisplay()->getNexChkbxByName("cbOnOff1")->setValue((uint32_t)this->config.getMember("OnOff1"));
    this->getNextionDisplay()->getNexChkbxByName("cbMo1"   )->setValue((uint32_t)this->config.getMember("Mo1")   );
    this->getNextionDisplay()->getNexChkbxByName("cbDi1"   )->setValue((uint32_t)this->config.getMember("Di1")   );
    this->getNextionDisplay()->getNexChkbxByName("cbMi1"   )->setValue((uint32_t)this->config.getMember("Mi1")   );
    this->getNextionDisplay()->getNexChkbxByName("cbDo1"   )->setValue((uint32_t)this->config.getMember("Do1")   );
    this->getNextionDisplay()->getNexChkbxByName("cbFr1"   )->setValue((uint32_t)this->config.getMember("Fr1")   );
    this->getNextionDisplay()->getNexChkbxByName("cbSa1"   )->setValue((uint32_t)this->config.getMember("Sa1")   );
    this->getNextionDisplay()->getNexChkbxByName("cbSo1"   )->setValue((uint32_t)this->config.getMember("So1")   );

    this->getNextionDisplay()->getNexNumberByName("nHour2" )->setValue((uint32_t)this->config.getMember("Hour2") );
    this->getNextionDisplay()->getNexNumberByName("nMin2"  )->setValue((uint32_t)this->config.getMember("Min2")  );
    this->getNextionDisplay()->getNexChkbxByName("cbOnOff2")->setValue((uint32_t)this->config.getMember("OnOff2"));
    this->getNextionDisplay()->getNexChkbxByName("cbMo2"   )->setValue((uint32_t)this->config.getMember("Mo2")   );
    this->getNextionDisplay()->getNexChkbxByName("cbDi2"   )->setValue((uint32_t)this->config.getMember("Di2")   );
    this->getNextionDisplay()->getNexChkbxByName("cbMi2"   )->setValue((uint32_t)this->config.getMember("Mi2")   );
    this->getNextionDisplay()->getNexChkbxByName("cbDo2"   )->setValue((uint32_t)this->config.getMember("Do2")   );
    this->getNextionDisplay()->getNexChkbxByName("cbFr2"   )->setValue((uint32_t)this->config.getMember("Fr2")   );
    this->getNextionDisplay()->getNexChkbxByName("cbSa2"   )->setValue((uint32_t)this->config.getMember("Sa2")   );
    this->getNextionDisplay()->getNexChkbxByName("cbSo2"   )->setValue((uint32_t)this->config.getMember("So2")   );

    this->getNextionDisplay()->getNexNumberByName("nHour3" )->setValue((uint32_t)this->config.getMember("Hour3") );
    this->getNextionDisplay()->getNexNumberByName("nMin3"  )->setValue((uint32_t)this->config.getMember("Min3")  );
    this->getNextionDisplay()->getNexChkbxByName("cbOnOff3")->setValue((uint32_t)this->config.getMember("OnOff3"));
    this->getNextionDisplay()->getNexChkbxByName("cbMo3"   )->setValue((uint32_t)this->config.getMember("Mo3")   );
    this->getNextionDisplay()->getNexChkbxByName("cbDi3"   )->setValue((uint32_t)this->config.getMember("Di3")   );
    this->getNextionDisplay()->getNexChkbxByName("cbMi3"   )->setValue((uint32_t)this->config.getMember("Mi3")   );
    this->getNextionDisplay()->getNexChkbxByName("cbDo3"   )->setValue((uint32_t)this->config.getMember("Do3")   );
    this->getNextionDisplay()->getNexChkbxByName("cbFr3"   )->setValue((uint32_t)this->config.getMember("Fr3")   );
    this->getNextionDisplay()->getNexChkbxByName("cbSa3"   )->setValue((uint32_t)this->config.getMember("Sa3")   );
    this->getNextionDisplay()->getNexChkbxByName("cbSo3"   )->setValue((uint32_t)this->config.getMember("So3")   );

    vTaskDelay(10/portTICK_PERIOD_MS);

    /* ---------------------------------------------- */
    /* set parameter                                  */
    getNextionDisplay()->getNexPageByName("page2")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nOffsetSun")->setValue((uint32_t)this->config.getMember("OffsetSun"));
    this->getNextionDisplay()->getNexNumberByName("nSnooze"   )->setValue((uint32_t)this->config.getMember("Snooze")   );
    this->getNextionDisplay()->getNexNumberByName("nTimeout"  )->setValue((uint32_t)this->config.getMember("Timeout")  );
    this->getNextionDisplay()->getNexNumberByName("nDim"      )->setValue((uint32_t)this->config.getMember("Dim")      );
    this->getNextionDisplay()->getNexNumberByName("nVolume"   )->setValue((uint32_t)this->config.getMember("Volume")  );
    vTaskDelay(10/portTICK_PERIOD_MS);
    /* ---------------------------------------------- */
    /* set color mode                                 */
    getNextionDisplay()->getNexPageByName("page4")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexRadioByName("r0")->setValue((uint32_t)this->config.getMember("r0"));
    this->getNextionDisplay()->getNexRadioByName("r1")->setValue((uint32_t)this->config.getMember("r1"));
    this->getNextionDisplay()->getNexRadioByName("r2")->setValue((uint32_t)this->config.getMember("r2"));
    vTaskDelay(10/portTICK_PERIOD_MS);
    /* ---------------------------------------------- */
    /* set color level                                */
    getNextionDisplay()->getNexPageByName("page5")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nDisBright" )->setValue((uint32_t)this->config.getMember("DisBright") );
    this->getNextionDisplay()->getNexNumberByName("nBrightness")->setValue((uint32_t)this->config.getMember("Brightness"));
    this->getNextionDisplay()->getNexNumberByName("nMaxDimSR"  )->setValue((uint32_t)this->config.getMember("MaxDimSR")  );
    vTaskDelay(10/portTICK_PERIOD_MS);
    /* ---------------------------------------------- */
    /* set color level                                */
    this->getNextionDisplay()->getNexPageByName("page6")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nWhite")->setValue((uint32_t)this->config.getMember("white"));
    this->getNextionDisplay()->getNexNumberByName("nRed"  )->setValue((uint32_t)this->config.getMember("red")  );
    this->getNextionDisplay()->getNexNumberByName("nGreen")->setValue((uint32_t)this->config.getMember("green"));
    this->getNextionDisplay()->getNexNumberByName("nBlue" )->setValue((uint32_t)this->config.getMember("blue") );
    vTaskDelay(50/portTICK_PERIOD_MS);
    getNextionDisplay()->getNexPageByName("page0")->show();
    vTaskDelay(10/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* LED                                                */
    if( !simpleSun.init_ledDriver() )
    {
      print("ERROR: could not init ledDriver!! rc := " + simpleSun.getRc() );
    }
    else
    {
      vTaskDelay(100/portTICK_PERIOD_MS);
      print("LED initialized. RC:" + simpleSun.getRc());
    }
    /* -------------------------------------------------- */
    /* register dispatcher                                */
    simpleSun.registerCB(dispatcher);
    dispatcher.broadcast("Test", EVENT1);
}

/**
 * @brief start up the filesystem (SPIFFS)
 * 
 * @return true 
 * @return false 
 */
bool Lichtwecker::startFS()
{
    print("startFS()");
    /* -------------------------------------------------- */
    /* start filesystem                                   */
    if(!SPIFFS.begin())
    {
        print("An Error has occurred while mounting SPIFFS");
        print("Did you upload the data directory that came with this example?");
        return false;
    }
    /* ------------------------------------------------- */
    /* filesystem mounted                                */
    if (SPIFFS.exists("/config.json")) 
    {
      /* ----------------------------------------------- */ 
      /* file exists, reading and loading                */
      print("reading config file");
      File configFile = SPIFFS.open("/config.json", FILE_READ);
      if (configFile) 
      {
        print("config file open");
        DeserializationError error = deserializeJson(this->config, configFile);
        if(error) 
        {
            print("Failed to read file:", false);
            print(error.c_str());
        }
      }
      print("close config file.");
      configFile.close();
    }
    return true;
}

/**
 * @brief get the content typ of file depending on extention
 * 
 * @param filename 
 * @return String 
 */
String Lichtwecker::getContentType(String filename)
{
    if(webServer.hasArg(F("download"))) return F("application/octet-stream");
    else if(filename.endsWith(F(".htm"))) return F("text/html");
    else if(filename.endsWith(".html")) return F("text/html");
    else if(filename.endsWith(F(".css"))) return F("text/css");
    else if(filename.endsWith(F(".js"))) return F("application/javascript");
    else if(filename.endsWith(F(".png"))) return F("image/png");
    else if(filename.endsWith(F(".gif"))) return F("image/gif");
    else if(filename.endsWith(F(".jpg"))) return F("image/jpeg");
    else if(filename.endsWith(F(".ico"))) return F("image/x-icon");
    else if(filename.endsWith(F(".xml"))) return F("text/xml");
    else if(filename.endsWith(F(".pdf"))) return F("application/x-pdf");
    else if(filename.endsWith(F(".zip"))) return F("application/x-zip");
    else if(filename.endsWith(F(".gz"))) return F("application/x-gzip");
    return F("text/plain");
}

/**
 * @brief handle file read
 * 
 * @param path 
 * @return true 
 * @return false 
 */
bool Lichtwecker::handleFileRead(String path) 
{
    /* -------------------------------------------------- */
    /* send the right file to the client (if it exists)   */
    print( "handleFileRead: " );
    print( path.c_str() );
    /* -------------------------------------------------- */
    /* If a folder is requested, send the index file      */
    if (path.endsWith("/")) path += "index.html";
    /* -------------------------------------------------- */
    /* Get the MIME type                                  */
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    /* -------------------------------------------------- */
    /* If the file exists, either as a compressed archive,*/
    /* or normal                                          */
    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
        /* ---------------------------------------------- */
        /* If there's a compressed version available, Use */
        /* the compressed verion                          */
        if (SPIFFS.exists(pathWithGz))
          path += ".gz";
        /* ---------------------------------------------- */
        /* open the file & Send it to the client          */
        File file = SPIFFS.open(path, "r");
        size_t sent = webServer.streamFile(file, contentType);
        /* ---------------------------------------------- */
        /* debug info                                     */
        print( (int)sent );
        print( "data send." );
        /* ---------------------------------------------- */
        /* close the file and return                      */
        file.close();
        print( "Sent file: " );
        print( path.c_str() );
        return true;
    }
    /* -------------------------------------------------- */
    /* if the is no file return false                     */
    print( (String("\tFile Not Found: ") + path).c_str() );
    return false;
}


/**
 * @brief start connection to wifi and register DNS
 * 
 * @param ssid_ 
 * @param password_ 
 * @param host_ 
 * @return true 
 * @return false 
 */
bool Lichtwecker::startWifi(const char *ssid_, const char *password_, const char *host_)
{
    int iCounter = 0;
    print("Connecting to ");
    print( ssid_ );
    /* -------------------------------------------------- */
    /* connect to WIFI                                    */
    if (String(WiFi.SSID()) != String(ssid_)) {
        WiFi.begin(ssid_, password_);
    }
    while (WiFi.status() != WL_CONNECTED) 
    {
        vTaskDelay(500/portTICK_PERIOD_MS);
        print(".", false);
        iCounter++;
        if(iCounter>100)
        {
            print("Error connecting to Wifi"); 
            return false;
        }
    }
    print( (String("Connected! IP address: ") + WiFi.localIP().toString() ).c_str() );
    /* -------------------------------------------------- */
    /* DNS register host by name                          */
    MDNS.begin(host_);
    print( ( String("http://") + String(host_) + String(".local") ).c_str() ); 
    return true;
}

void Lichtwecker::saveToConfigfile( const char *ptr)
{
  print("Lichtwecker::saveToConfigfile(", false);
  print(ptr, false);
  print(")");

  uint32_t value;
  /* -------------------------------------------------- */
  /* save values from page 1                            */
  if( (strncmp("save_1", (char *)ptr, 6 ) == 0))  
  {
    /* ---------------------------------------------- */
    /* save parameter                                  */
    getNextionDisplay()->getNexPageByName("page1")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nHour1" )->getValue(&value);
    this->config["Hour1"] = value;
    this->getNextionDisplay()->getNexNumberByName("nMin1"  )->getValue(&value);
    this->config["Min1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbOnOff1")->getValue(&value);
    this->config["OnOff1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbMo1"   )->getValue(&value);
    this->config["Mo1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbDi1"   )->getValue(&value);
    this->config["Di1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbMi1"   )->getValue(&value);
    this->config["Mi1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbDo1"   )->getValue(&value);
    this->config["Do1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbFr1"   )->getValue(&value);
    this->config["Fr1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbSa1"   )->getValue(&value);
    this->config["Sa1"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbSo1"   )->getValue(&value);
    this->config["So1"] = value;

    this->getNextionDisplay()->getNexNumberByName("nHour2" )->getValue(&value);
    this->config["Hour2"] = value;
    this->getNextionDisplay()->getNexNumberByName("nMin2"  )->getValue(&value);
    this->config["Min2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbOnOff2")->getValue(&value);
    this->config["OnOff2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbMo2"   )->getValue(&value);
    this->config["Mo2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbDi2"   )->getValue(&value);
    this->config["Di2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbMi2"   )->getValue(&value);
    this->config["Mi2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbDo2"   )->getValue(&value);
    this->config["Do2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbFr2"   )->getValue(&value);
    this->config["Fr2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbSa2"   )->getValue(&value);
    this->config["Sa2"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbSo2"   )->getValue(&value);
    this->config["So2"] = value;

    this->getNextionDisplay()->getNexNumberByName("nHour3" )->getValue(&value);
    this->config["Hour3"] = value;
    this->getNextionDisplay()->getNexNumberByName("nMin3"  )->getValue(&value);
    this->config["Min3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbOnOff3")->getValue(&value);
    this->config["OnOff3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbMo3"   )->getValue(&value);
    this->config["Mo3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbDi3"   )->getValue(&value);
    this->config["Di3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbMi3"   )->getValue(&value);
    this->config["Mi3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbDo3"   )->getValue(&value);
    this->config["Do3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbFr3"   )->getValue(&value);
    this->config["Fr3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbSa3"   )->getValue(&value);
    this->config["Sa3"] = value;
    this->getNextionDisplay()->getNexChkbxByName("cbSo3"   )->getValue(&value);
    this->config["So3"] = value;

  }
  /* -------------------------------------------------- */
  /* save values from page 2                            */
  if( (strncmp("save_2", (char *)ptr, 6 ) == 0))  
  {
    /* ---------------------------------------------- */
    /* set parameter                                  */
    getNextionDisplay()->getNexPageByName("page2")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nOffsetSun")->getValue(&value);
    this->config["OffsetSun"] = value;
    this->getNextionDisplay()->getNexNumberByName("nSnooze"   )->getValue(&value);
    this->config["Snooze"] = value;
    this->getNextionDisplay()->getNexNumberByName("nTimeout"  )->getValue(&value);
    this->config["Timeout"] = value;
    this->getNextionDisplay()->getNexNumberByName("nDim"      )->getValue(&value);
    this->config["Dim"] = value;
    this->getNextionDisplay()->getNexNumberByName("nVolume"   )->getValue(&value);
    this->config["Volume"] = value;
  }
  /* -------------------------------------------------- */
  /* save values from page 4                            */
  if( (strncmp("save_4", (char *)ptr, 6 ) == 0))  
  {
    /* ---------------------------------------------- */
    /* set color mode                                 */
    getNextionDisplay()->getNexPageByName("page4")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexRadioByName("r0")->getValue(&value);
    this->config["r0"] = value;
    this->getNextionDisplay()->getNexRadioByName("r1")->getValue(&value);
    this->config["r1"] = value;
    this->getNextionDisplay()->getNexRadioByName("r2")->getValue(&value);
    this->config["r2"] = value;
  }
  /* -------------------------------------------------- */
  /* save values from page 5                            */
  if( (strncmp("save_5", (char *)ptr, 6 ) == 0))  
  {
    this->getNextionDisplay()->getNexPageByName("page5")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nDisBright")->getValue(&value);
    this->config["DisBright"] = value;
    this->getNextionDisplay()->getNexNumberByName("nBrightness")->getValue(&value);
    this->config["Brightness"] = value;
    this->getNextionDisplay()->getNexNumberByName("nMaxDimSR")->getValue(&value);
    this->config["MaxDimSR"]   = value;
  }
  /* -------------------------------------------------- */
  /* save values from page 6                            */
  if( (strncmp("save_6", (char *)ptr, 6 ) == 0))  
  {
    /* ---------------------------------------------- */
    /* set color level                                */
    this->getNextionDisplay()->getNexPageByName("page6")->show();
    vTaskDelay(50/portTICK_PERIOD_MS);
    this->getNextionDisplay()->getNexNumberByName("nWhite")->getValue(&value);
    this->config["white"] = value;
    this->getNextionDisplay()->getNexNumberByName("nRed")->getValue(&value);
    this->config["red"]   = value;
    this->getNextionDisplay()->getNexNumberByName("nGreen")->getValue(&value);
    this->config["green"] = value;
    this->getNextionDisplay()->getNexNumberByName("nBlue")->getValue(&value);
    this->config["blue"]  = value;
  }
  vTaskDelay(10/portTICK_PERIOD_MS);
  this->getNextionDisplay()->getNexPageByName("page0")->show();

  this->printConfig();
  /* ----------------------------------------------- */ 
  /* file exists, reading and loading                */
  File configFile = SPIFFS.open("/config.json", FILE_WRITE);
  if (configFile) 
  {
    print("config file open");
    int ret = serializeJson(this->config, configFile);
    print(ret, false ); print(" written");
  }
  print("close config file.");
  configFile.close();
}

void Lichtwecker::printConfig()
{
#ifdef DEBUG_SERIAL_ENABLE
  char buffer[1030]; 
  serializeJsonPretty(this->config, buffer);
  dbSerialPrintln(buffer);
#endif
}