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
    delete &myDFPlayer;
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
    /* -------------------------------------------------- */
    /* DFPlayer                                           */
    print("Start DFPlayer...");
    myDFPlayer.startDFPlayer();
    vTaskDelay(200/portTICK_PERIOD_MS);
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
    vTaskDelay(200/portTICK_PERIOD_MS);
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
    myDFPlayer.registerCB(dispatcher);
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
    /* -------------------------------------------------- */
    /* start filesystem                                   */
    if(!SPIFFS.begin()){
        print("An Error has occurred while mounting SPIFFS");
        print("Did you upload the data directory that came with this example?");
        return false;
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
        print( sent );
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
