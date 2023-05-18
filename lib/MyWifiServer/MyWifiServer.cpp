#include <MyWifiServer.h>

/**
 * @brief  MyWifiServer::loadConfig() reads /config_wifi.json
 * @note   we need host (name), ap (name of accesspoint), appw (ap-password), port (port)
 * @retval true|false
 */
boolean MyWifiServer::loadConfig()
{
    /*-----------------------------------------------------*/
    /* check if configfile exists                          */
    if(SPIFFS.exists(WIFI_CONFIG_FILE))
    {
        /*---------------------------------------------------*/
        /* load config                                       */
        this->configServer = new MyConfigServer();
        return this->configServer->loadConfig(&SPIFFS, WIFI_CONFIG_FILE);
    }
    return false;
}

/**
 * @brief  MyWifiServer::regToMDNS() - register hostname to DNS  
 * @note   
 * @retval true|false
 */
boolean MyWifiServer::regToMDNS()
{
    std::string host = "lichtwecker";
    if( this->configServer->containsKey("host") )
        host = this->configServer->getElement("host");
    int port = 80;
    if( this->configServer->containsKey("port") )
        port = std::stoi(this->configServer->getElement("port"));
    std::string domain = "local";
    if( this->configServer->containsKey("domain") )
        domain = this->configServer->getElement("domain");

    host.append(".");
    host.append(domain);

    if(MDNS.begin(host.c_str())) 
    {
        MDNS.addService("http", "tcp", port);
        dbSerialPrintf("\nconnect to http://%s\n", host.c_str());
    }
    else
    {
        dbSerialPrintln("Could not register to MDNS!!");
        return false;
    }
    return true;
}

/**
 * @brief  MyWifiServer::connectWifi()  
 * @note   load config, connect to Wifi or start ap, reg to DNS
 * @retval ture|false
 */
boolean MyWifiServer::connectWifi()
{
    if(!this->loadConfig())
    {
        return false;
    }
    if(!wm.autoConnect( this->configServer->getElement("ap").c_str(), this->configServer->getElement("appw").c_str()) )
    {
        return false;
    }
    if(!this->regToMDNS())
    {
        return false;
    }
    return true; 
}


void MyWifiServer::resetWifi()
{
    wm.resetSettings();
}
