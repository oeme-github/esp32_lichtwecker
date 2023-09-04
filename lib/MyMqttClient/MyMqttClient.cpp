#include <MyMqttClient.h>

/**
 * @brief  MyMqttClient() constructor
 * @note   
 * @retval 
 */
MyMqttClient::MyMqttClient()
{
}

/**
 * @brief  MyMqttClient() destructor
 * @note   
 * @retval 
 */
MyMqttClient::~MyMqttClient()
{
}

/**
 * @brief  loadMqttClient
 * @note   init MQTT client
 * @param  *fs: 
 * @retval std::pair<int, std::string>
 */
RetCode MyMqttClient::loadMqttClient(FS *fs)
{
  RetCode retCode;
  /*-----------------------------------------------------*/
  /* init                                                */
  this->fs = fs;
  if( existsOnFs(fs, MQTT_CONFIG_FILE) )
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
  if( this->configServer->containsKey("server") && this->configServer->getElement("server") == "0" )
  {
      dbSerialPrintln("WARNING: mqtt config server = 0 !!!");
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
 * @brief  loadConfig
 * @note   load the config from file
 * @retval std::pair<int, std::string>
 */
RetCode MyMqttClient::loadConfig()
{
  RetCode retCode;
  retCode.first  = 0;
  retCode.second = "";
  /*-----------------------------------------------------*/
  /* check if configfile exists                          */
  if(!this->fs->exists(MQTT_CONFIG_FILE))
  {
    retCode.first  = -1;
    retCode.second = "ERROR: Can not Load config.";
  }
  else
  {
    /*---------------------------------------------------*/
    /* load config                                       */
    this->configServer = new MyConfigServer();
    
    if(!this->configServer->loadConfig(this->fs, MQTT_CONFIG_FILE))
    {
      retCode.first = -1;
      retCode.second = "ERROR: Can not Load config.";
    }
    else
    {
        this->configServer->printConfig();
    }
  }
  return retCode;
}

/**
 * @brief  begin
 * @note   connect to server
 * @retval std::pair<int, std::string>
 */
RetCode MyMqttClient::begin()
{
    RetCode retCode;
    retCode.first  = 0;
    retCode.second = "";

// PubSubClient(const char* domain, uint16_t port, Client& client)
    this->mqttClient = new PubSubClient(  this->configServer->getElement("server").c_str()
                                        , stoul(this->configServer->getElement("port"))
                                        , this->espClient );
    if( this->configServer->containsKey("keepalive") )
    {
        this->mqttClient->setKeepAlive( stoul(this->configServer->getElement("keepalive")) );
        this->mqttClient->setSocketTimeout( stoul(this->configServer->getElement("keepalive")) );
    }
    
// connect(const char *id, const char *user, const char *pass)
    if( !this->mqttClient->connect(   this->configServer->getElement("id").c_str()
                                    , this->configServer->getElement("user").c_str()
                                    , this->configServer->getElement("password").c_str()) ) 
    {
        // connection failed
        retCode.first  = this->mqttClient->state();
        retCode.second = "Connection failed";
    }
    else
    {
        // connection succeeded
        Serial.println("Connected now subscribing");
        if( !this->mqttClient->subscribe(this->configServer->getElement("id").c_str()) )
        {
            // connection failed
            retCode.first  = this->mqttClient->state();
            retCode.second = "Subscribing failed";
        }
        else
        {
            this->sendMsg("MQTT Client gestartet...");
        }
    }

    return retCode;
}

/**
 * @brief  sendMsg
 * @note   send message to server
 * @param  msg: std::string containign the message
 * @retval std::pair<int, std::string>
 */
RetCode MyMqttClient::sendMsg(std::string msg)
{
    RetCode retCode;

    if( !this->mqttClient->connected() ) 
    {
        this->reconnect();
    }

    if( this->mqttClient->publish(this->configServer->getElement("id").c_str(), (this->currentDateTime() + " - " + msg).c_str() ) )
    {
        retCode.first  = 0;
        retCode.second = "msg send";
    }
    else
    {
        retCode.first  = this->mqttClient->state();
        retCode.second = "Connection failed";
    }
    return retCode;
}


void MyMqttClient::reconnect() 
{
    // Loop until we're reconnected
    while( !this->mqttClient->connected() ) 
    {
        dbSerialPrintln("Attempting MQTT (re)connection...");
        dbSerialPrint("server:"); dbSerialPrintln(this->configServer->getElement("server").c_str());
        dbSerialPrint("port  :"); dbSerialPrintln(this->configServer->getElement("port"  ).c_str());
        dbSerialPrint("id    :"); dbSerialPrintln(this->configServer->getElement("id"    ).c_str());
        // Attempt to connect 

        this->mqttClient->setServer(  this->configServer->getElement("server").c_str()
                                    , stoul(this->configServer->getElement("port")));

        if( !this->mqttClient->connect(this->configServer->getElement("id").c_str()
                                    , this->configServer->getElement("user").c_str()
                                    , this->configServer->getElement("password").c_str()) ) 
        {
            dbSerialPrint("failed, rc=");
            dbSerialPrintln(this->mqttClient->state());
            dbSerialPrintln("-> try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string MyMqttClient::currentDateTime() 
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
