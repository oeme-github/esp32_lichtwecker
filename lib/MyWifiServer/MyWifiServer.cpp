#include <MyWifiServer.h>

/**
 * @brief  MyWifiServer::loadConfig() reads /config_wifi.json
 * @note   we need host (name), ap (name of accesspoint), appw (ap-password), port (port)
 * @retval true|false
 */
boolean MyWifiServer::loadConfig()
{
    dbSerialPrintln("*** MyWifiServer::loadConfig() ***");
    this->configServer = new MyConfigServer();
    this->configServer->loadConfig(&SPIFFS, WIFI_CONFIG_FILE, FileFormat::MAP);
    if( !this->configServer->isConfigLoaded() )
    {
        return false;
    }
    return true;
}

/**
 * @brief  MyWifiServer::regToMDNS() - register hostname to DNS  
 * @note   
 * @retval true|false
 */
boolean MyWifiServer::regToMDNS()
{
    dbSerialPrintln("*** MyWifiServer::regToMDNS() ***");
    String host = "lichtwecker";
    if( this->configServer->containsKey("host") )
        host = this->configServer->getElement("host").c_str();
    int port = 80;
    if( this->configServer->containsKey("port") )
        port = std::stoi(this->configServer->getElement("port"));
    
    if(MDNS.begin(host.c_str())) 
    {
        MDNS.addService("http", "tcp", port);
        dbSerialPrint("connect to http://");
        dbSerialPrint(host);
        dbSerialPrintln(".localdomain");

        this->isRegToMDNS = true;
    }
    else
    {
        dbSerialPrintln("Could not register to MDNS!!");
        this->isRegToMDNS = false;
    }
    return this->isRegToMDNS;
}

/**
 * @brief  MyWifiServer::connectWifi()  
 * @note   load config, connect to Wifi or start ap, reg to DNS
 * @retval ture|false
 */
boolean MyWifiServer::connectWifi()
{
    dbSerialPrintln("*** MyWifiServer::connectWifi() ***");
    if(!this->loadConfig())
    {
        dbSerialPrintln("WIFI-ERROR: Can not load config.");
        return false;
    }
    if(!wm.autoConnect( this->configServer->getElement("ap").c_str(), this->configServer->getElement("appw").c_str()) )
    {
        dbSerialPrintln("WIFI-ERROR: Can not connect to wifi.");
        return false;
    }
    if(!this->regToMDNS())
    {
        dbSerialPrintln("WIFI-ERROR: Can not register to DNS.");
        return false;
    }

    return true; 
}


void MyWifiServer::resetWifi()
{
    wm.resetSettings();
}
