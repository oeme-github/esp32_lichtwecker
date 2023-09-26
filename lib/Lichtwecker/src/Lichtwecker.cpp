#include <Lichtwecker.h>

/**
 * @brief constructor
 * 
 */
Lichtwecker::Lichtwecker()
{
}

/**
 * @brief Destroy the Lichtwecker:: Lichtwecker object
 * 
 */
Lichtwecker::~Lichtwecker()
{
}

/**
 * @brief  void Lichtwecker::setNextValue( NextType typ, const char *cNameElemet, const char *configElement)
 * @note   helper to the values from config to nextion display
 * @param  typ: 
 * @param  *cNameElemet: 
 * @param  *configElement: 
 * @retval None
 */
void Lichtwecker::setNextValue( const char *cNameElemet)
{
  char c = cNameElemet[0];
  if( c == 'n' )
  {
    this->getNextionDisplay()->getNexNumberByName(cNameElemet)->setValue(std::stoi(this->configServer->getElement(cNameElemet).c_str()));
  }
  else if( c == 'c' )
  {
    this->getNextionDisplay()->getNexChkbxByName(cNameElemet)->setValue(std::stoi(this->configServer->getElement(cNameElemet).c_str()));
  }
  else if( c == 'r' )
  {
    this->getNextionDisplay()->getNexRadioByName(cNameElemet)->setValue(std::stoi(this->configServer->getElement(cNameElemet).c_str()));
  } 
  else
  {
    dbSerialPrintln("WARNING: did not found type of element!!");
  }
}

/**
 * @brief start()
 * 
 */
void Lichtwecker::start()
{
  this->myQueueHandler.sendToQueue("*** START LICHTWECKER ***");
  /*-----------------------------------------------------*/
  /* load config                                         */
  if(!this->loadConfig())
  {
      this->myQueueHandler.sendToQueue("ERROR: Failed to load config -> create on and restart...");
      this->createConfigJson();
      ESP.restart();
  }
  /* -------------------------------------------------- */
  /* start nextion                                      */
  this->nextionDisplay.InitDisplay();
  vTaskDelay(200/portTICK_PERIOD_MS);
  /* -------------------------------------------------- */
  /* RTC                                                */
  if(!nexRtc.updateDateTime())
  {
    this->myQueueHandler.sendToQueue("ERROR: updateDateTime went wrong -> restart...");
    ESP.restart();
  }
  /* -------------------------------------------------- */
  /* set offset and brightness                          */
  this->simpleSun.setWakeDelay( std::stoi(this->configGetElement("nOffsetSun" ).c_str()));
  this->simpleSun.setBrightness(std::stoi(this->configGetElement("nBrightness").c_str()));
  /* -------------------------------------------------- */
  /* set parameter                                      */
  while( !this->nextionDisplay.getNexPageByName("page 1")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  vTaskDelay(100/portTICK_PERIOD_MS);
  this->setNextValue( "nHour1"  );
  this->setNextValue( "nMin1"   );
  this->setNextValue( "cbOnOff1");
  this->setNextValue( "cbMo1"   );
  this->setNextValue( "cbDi1"   );
  this->setNextValue( "cbMi1"   );
  this->setNextValue( "cbDo1"   );
  this->setNextValue( "cbFr1"   );
  this->setNextValue( "cbSa1"   );
  this->setNextValue( "cbSo1"   );

  this->setNextValue( "nHour2"  );
  this->setNextValue( "nMin2"   );
  this->setNextValue( "cbOnOff2");
  this->setNextValue( "cbMo2"   );
  this->setNextValue( "cbDi2"   );
  this->setNextValue( "cbMi2"   );
  this->setNextValue( "cbDo2"   );
  this->setNextValue( "cbFr2"   );
  this->setNextValue( "cbSa2"   );
  this->setNextValue( "cbSo2"   );

  this->setNextValue( "nHour3"  );
  this->setNextValue( "nMin3"   );
  this->setNextValue( "cbOnOff3");
  this->setNextValue( "cbMo3"   );
  this->setNextValue( "cbDi3"   );
  this->setNextValue( "cbMi3"   );
  this->setNextValue( "cbDo3"   );
  this->setNextValue( "cbFr3"   );
  this->setNextValue( "cbSa3"   );
  this->setNextValue( "cbSo3"   );
  /* ---------------------------------------------- */
  /* set parameter                                  */
  while( !this->nextionDisplay.getNexPageByName("page 2")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  vTaskDelay(100/portTICK_PERIOD_MS);    
  this->setNextValue( "nOffsetSun");
  this->setNextValue( "nSnooze"   );
  this->setNextValue( "nTimeout"  );
  this->setNextValue( "nDim"      );
  this->setNextValue( "nVolume"   );
  /* ---------------------------------------------- */
  /* page 4                                         */
  while( !this->nextionDisplay.getNexPageByName("page 4")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  vTaskDelay(100/portTICK_PERIOD_MS);
  this->setNextValue( "r0");
  this->setNextValue( "r1");
  this->setNextValue( "r2");
  /* ---------------------------------------------- */
  /* page 5                                         */
  while( !this->nextionDisplay.getNexPageByName("page 5")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  vTaskDelay(100/portTICK_PERIOD_MS);
  this->setNextValue( "nDisBright" );
  this->setNextValue( "nBrightness");
  this->setNextValue( "nMaxDimSR"  );
  /* ---------------------------------------------- */
  /* page 6                                         */
  while( !this->nextionDisplay.getNexPageByName("page 6")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  vTaskDelay(100/portTICK_PERIOD_MS);
  this->setNextValue( "nWhite");
  this->setNextValue( "nRed"  );
  this->setNextValue( "nGreen");
  this->setNextValue( "nBlue" );
  /* ---------------------------------------------- */
  /* page 0                                         */
  while( !this->nextionDisplay.getNexPageByName("page 0")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  vTaskDelay(100/portTICK_PERIOD_MS);
  /* -------------------------------------------------- */
  /* LED                                                */
  if( !this->simpleSun.init_ledDriver() )
  {
    this->myQueueHandler.sendToQueue( "ERROR: could not init ledDriver!! rc :=[" + std::to_string(this->simpleSun.getRc()) + "]");
  }
  /* -------------------------------------------------- */
  /* register dispatcher                                */
  this->simpleSun.registerCB(dispatcher);
  this->dispatcher.broadcast("Test", EVENT1);
}


/**
 * @brief  loadConfig()
 * @note   creats a MyConfigServer and loads the config file
 * @retval true|false
 */
boolean Lichtwecker::loadConfig()
{
  this->myQueueHandler.sendToQueue("*** Lichtwecker::loadConfig() ***");
  /*-----------------------------------------------------*/
  /* check if configfile exists                          */
  if(SPIFFS.exists(LW_CONFIG_FILE))
  {
    /*---------------------------------------------------*/
    /* load config                                       */
    this->configServer = new MyConfigServer();
    return this->configServer->loadConfig(&SPIFFS, LW_CONFIG_FILE);
  }
  return false;
}

/**
 * @brief  void Lichtwecker::getNextValue( const char *cNameElemet )
 * @note   get value from nextion element by name
 * @param  *cNameElemet: 
 * @retval None
 */
void Lichtwecker::getNextValue( const char *cNameElemet )
{
  uint32_t value = -1;

  while(true)
  {
    if( String(cNameElemet).startsWith("n") )
    {
      this->getNextionDisplay()->getNexNumberByName(cNameElemet)->getValue(&value);
    }
    else if( String(cNameElemet).startsWith("r") )
    {
      this->getNextionDisplay()->getNexRadioByName(cNameElemet)->getValue(&value);
    }
    else if( String(cNameElemet).startsWith("c") )
    {
      this->getNextionDisplay()->getNexChkbxByName(cNameElemet)->getValue(&value);
    }
    if( value >= 0 ) 
      break;
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
  this->configServer->putElement(cNameElemet, value);
}

/**
 * @brief  void Lichtwecker::saveToConfigfile( const char *ptr)
 * @note   get values from page of nextion display and save values in config
 * @param  *ptr: 
 * @retval None
 */
void Lichtwecker::saveToConfigfile( const char *ptr)
{
  this->sendToQueue( "Lichtwecker::saveToConfigfile(" + std::string(ptr) + ")" );

  while( !getNextionDisplay()->getNexPageByName(std::string(ptr).c_str())->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  /*----------------------------------------------------*/
  /* need to wait for the page to show u                */
  vTaskDelay(100/portTICK_PERIOD_MS);
  /*----------------------------------------------------*/
  /* save values from page 1                            */
  if(std::string("page 1") == std::string(ptr))  
  {
    this->getNextValue( "nHour1"  );
    this->getNextValue( "nMin1"   );
    this->getNextValue( "cbOnOff1");
    this->getNextValue( "cbMo1"   );
    this->getNextValue( "cbDi1"   );
    this->getNextValue( "cbMi1"   );
    this->getNextValue( "cbDo1"   );
    this->getNextValue( "cbFr1"   );
    this->getNextValue( "cbSa1"   );
    this->getNextValue( "cbSo1"   );

    this->getNextValue( "nHour2"  );
    this->getNextValue( "nMin2"   );
    this->getNextValue( "cbOnOff2");
    this->getNextValue( "cbMo2"   );
    this->getNextValue( "cbDi2"   );
    this->getNextValue( "cbMi2"   );
    this->getNextValue( "cbDo2"   );
    this->getNextValue( "cbFr2"   );
    this->getNextValue( "cbSa2"   );
    this->getNextValue( "cbSo2"   );

    this->getNextValue( "nHour3"  );
    this->getNextValue( "nMin3"   );
    this->getNextValue( "cbOnOff3");
    this->getNextValue( "cbMo3"   );
    this->getNextValue( "cbDi3"   );
    this->getNextValue( "cbMi3"   );
    this->getNextValue( "cbDo3"   );
    this->getNextValue( "cbFr3"   );
    this->getNextValue( "cbSa3"   );
    this->getNextValue( "cbSo3"   );
  }
  /* -------------------------------------------------- */
  /* save values from page 2                            */
  else if(std::string("page 2") == std::string(ptr))  
  {
    this->getNextValue( "nOffsetSun" );
    this->getNextValue( "nSnooze"    );
    this->getNextValue( "nTimeout"   );
    this->getNextValue( "nDim"       );
    this->getNextValue( "nVolume"    );
  }
  /* -------------------------------------------------- */
  /* save values from page 4                            */
  else if(std::string("page 4") == std::string(ptr))  
  {
    this->getNextValue( "r0");
    this->getNextValue( "r1");
    this->getNextValue( "r2");
  }
  /* -------------------------------------------------- */
  /* save values from page 5                            */
  else if(std::string("page 5") == std::string(ptr))  
  {
    this->getNextValue( "nDisBright" );
    this->getNextValue( "nBrightness");
    this->getNextValue( "nMaxDimSR"  );
  }
  /* -------------------------------------------------- */
  /* save values from page 6                            */
  else if(std::string("page 6") == std::string(ptr))  
  {
    this->getNextValue( "nWhite");
    this->getNextValue( "nRed"  );
    this->getNextValue( "nGreen");
    this->getNextValue( "nBlue" );
  }
  while( !this->getNextionDisplay()->getNexPageByName("page 0")->show() )
    vTaskDelay(100/portTICK_PERIOD_MS);
  /*-------------------------------------------------------*/
  /* save to config file                                   */
  this->configServer->saveToConfigfile();
}

/**
 * @brief  createConfigJson()
 * @note   creates the json config and saves it to file (config.json)
 * @retval None
 */
void Lichtwecker::createConfigJson()
{
    this->configServer->putElement("nWhite"     , "40"  ); 
    this->configServer->putElement("nRed"       , "255" ); 
    this->configServer->putElement("nGreen"     , "100" );
    this->configServer->putElement("nBlue"      , "10"  );
    this->configServer->putElement("nDisBright" , "10"  );
    this->configServer->putElement("nBrightness", "10"  );
    this->configServer->putElement("nMaxDimSR"  , "5"   );
    this->configServer->putElement("r0"         , "0"   );
    this->configServer->putElement("r1"         , "1"   );
    this->configServer->putElement("r2"         , "0"   );
    this->configServer->putElement("nOffsetSun" , "20"  );
    this->configServer->putElement("nSnooze"    , "5"   );
    this->configServer->putElement("nTimeout"   , "15"  );
    this->configServer->putElement("nDim"       , "20"  );
    this->configServer->putElement("nVolume"    , "15"  );
    this->configServer->putElement("nHour1"     , "6"   );
    this->configServer->putElement("nMin1"      , "30"  );
    this->configServer->putElement("cbOnOff1"   , "0"   );
    this->configServer->putElement("cbMo1"      , "0"   );
    this->configServer->putElement("cbDi1"      , "0"   );
    this->configServer->putElement("cbMi1"      , "0"   );
    this->configServer->putElement("cbDo1"      , "0"   );
    this->configServer->putElement("cbFr1"      , "0"   );
    this->configServer->putElement("cbSa1"      , "0"   );
    this->configServer->putElement("cbSo1"      , "0"   );
    this->configServer->putElement("nHour2"     , "6"   );
    this->configServer->putElement("nMin2"      , "30"  );
    this->configServer->putElement("cbOnOff2"   , "0"   );
    this->configServer->putElement("cbMo2"      , "0"   );
    this->configServer->putElement("cbDi2"      , "0"   );
    this->configServer->putElement("cbMi2"      , "0"   );
    this->configServer->putElement("cbDo2"      , "0"   );
    this->configServer->putElement("cbFr2"      , "0"   );
    this->configServer->putElement("cbSa2"      , "0"   );
    this->configServer->putElement("cbSo2"      , "0"   );
    this->configServer->putElement("nHour3"     , "7"   );
    this->configServer->putElement("nMin3"      , "30"  );
    this->configServer->putElement("cbOnOff3"   , "0"   );
    this->configServer->putElement("cbMo3"      , "0"   );
    this->configServer->putElement("cbDi3"      , "0"   );
    this->configServer->putElement("cbMi3"      , "0"   );
    this->configServer->putElement("cbDo3"      , "0"   );
    this->configServer->putElement("cbFr3"      , "0"   );
    this->configServer->putElement("cbSa3"      , "0"   );
    this->configServer->putElement("cbSo3"      , "0"   );

    this->configServer->saveToConfigfile();
}