#include <SPIFFS.h>
#include <libConfig.h>
#include <libDebug.h>
#include <libFile.h>
#include <Lichtwecker.h>
#include <CallbackFunctions.h>
#include <MyAudioPlayer.h>
#include <MyWifiServer.h>
#include <MyWebServer.h>

uint32_t iTemp = 0;

/*---------------------------------------------------------*/
/* global varaibles                                        */
MyWifiServer wifiServer;
MyWebServer webServer;
Lichtwecker lichtwecker;
MyAudioPlayer audioPlayer;

/** 
 * === TASKS ===
 */
TaskHandle_t hTaskNexLoop;
TaskHandle_t hTaskWebServer;

/**
 * @brief  taskNexLoopCode()
 * @note   ist started during setup()
 * @param  pvParameters
 * @retval None 
 */
void taskNexLoopCode( void * pvParameters )
{
  while(true)
  {    
    // watch the touch events  
    nexLoop(lichtwecker.getNextionDisplay()->getNexListenList().data(), lichtwecker.getNextionDisplay()->getNexListenList().size());
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
/**
 * @brief  taskSunLoopCode()
 * @note   ist started during setup()
 * @param  pvParameters 
 * @retval None
 */
void taskSunLoopCode( void * pvParameters )
{
  while(true)
  {
    /* -------------------------------------------------- */    
    /* check the sun and run it                           */
    if( lichtwecker.getSimpleSun()->getRc() == 0 )
    {
      /* ------------------------------------------------ */
      /* run the timer                                    */
      if( strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunRise") == 0 
          || strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunSet") == 0 )
      {
          lichtwecker.getSimpleSun()->run();
      }
      else
      {
        vTaskSuspend(NULL);        
      }
    }
    /* -------------------------------------------------- */
    /* need to delay the task because of wachdog          */
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}

/**
 * @brief  taskSoundLoopCode()
 * @note   ist started during setup()
 * @param  pvParameters 
 * @retval None
 */
void taskSoundLoopCode( void * pvParameters )
{
  dbSerialPrintln("*** void taskSoundLoopCode( void * pvParameters ) ***");
  while(true)
  {
    if( audioPlayer.getActive() == true )
    {
      /* -------------------------------------------------- */    
      /* check the sun and run it                           */
      /* need to delay the task because of warchdog         */
      audioPlayer.play();
    }
    else
    {
      vTaskSuspend(NULL);        
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

/**
 * @brief  taskWebServerCode() 
 * @note   ist started during setup()
 * @param  pvParameters: 
 * @retval None
 */
void taskWebServerCode( void * pvParameters )
{
  /* -------------------------------------------------- */ 
  /* catch upload server events                         */
  while(true)
  {
    webServer.handleClient();
    vTaskDelay(20/portTICK_PERIOD_MS);
  }
}
/** 
 * === END TASKS SECTION ===
 */


/**
 * === SUN FUNCTIONS ===
 */

/**
 * @brief sun timer function
 * 
 */
void sunRiseMain()
{
  if(lichtwecker.getSimpleSun()->getSunPhase())
  {
    /* let sun rise (without parameter) */
    lichtwecker.getSimpleSun()->letSunRise();
  }
  else
  {
    /* sun is risen */
    lichtwecker.getSimpleSun()->sunUp();
  }
}
/**
 * === END SUN FUNCTIONS ===
 */

/** 
 * === NEXTION CALLBACK SECTION ===
 */
/**
 * @brief Get the Volume From Nextion object
 * 
 */
void getVolumeFromNextion()
{
  iTemp      = -1;
  float fVol = 0.5;
  if(lichtwecker.configContainsKey("nVolume"))
    iTemp = std::stoi(lichtwecker.configGetElement("nVolume"));
  if(iTemp<0 || iTemp>30)
  {
    iTemp = 10;
  }
  fVol = (float)iTemp/(float)30;
  dbSerialPrintf("Volume: %6.1lf", fVol);
  audioPlayer.setVolume(fVol);
}
/**
 * @brief Get the Bt Val From Nextion object
 * 
 * @param bt 
 * @return int 
 */
int getBtValFromNextion(const char *btName)
{
  iTemp = -1;
  /* ---------------------------------------------- */
  /* get the offset from nextion dixplay            */
  lichtwecker.getNextionDisplay()->getNexButtonByName(btName)->getValue(&iTemp);
  if(iTemp < 0)
  {
      iTemp = 20;
  }
  return iTemp;
}

/**
 * @brief button callback 
 * 
 * @param ptr 
 */
void page2_btResetPushCallback(void *ptr)
{
  ESP.restart();
} 
/**
 * @brief timer callback for serial communication
 * 
 * @param ptr 
 */
void page_save_values(void *ptr)
{
  std::string param = (const char *)ptr;
  param.replace(param.find("save_"), sizeof("save_") - 1, "page ");
  lichtwecker.saveToConfigfile( param.c_str() );
} 
/**
 * @brief timer callback for serial communication
 * 
 * @param ptr 
 */
void page0_tmSerialCmdCallback(void *ptr)
{
  iTemp = 0;
  /* ================================================== */
  /* disable timer, otherwise it would interfere with   */
  /* the communication                                  */
  for(auto & elem : lichtwecker.getNextionDisplay()->getNexTimer() )
  {
    elem->disable();
  }
  /* -------------------------------------------------- */
  /* sunup -> start sunrise                             */
  if( (strncmp("sunup", (char *)ptr, 5 ) == 0))  
  {
    /* ---------------------------------------------- */
    /* get the offset from nextion dixplay            */
    lichtwecker.getSimpleSun()->setWakeDelay(std::stoi(lichtwecker.configGetElement("nOffsetSun").c_str()));
  }
  /* -------------------------------------------------- */
  /* sunup -> start sunrise                             */
  if( (strncmp("light_on", (char *)ptr, 8 ) == 0))
  {
    /* ---------------------------------------------- */
    /* get the offset from nextion dixplay            */
    lichtwecker.getSimpleSun()->setBrightness(std::stoi(lichtwecker.configGetElement("nBrightness").c_str()));  
  }
  /* -------------------------------------------------- */
  /* get volume                                         */
  if( (strncmp("ala_up", (char *)ptr, 6 ) == 0))  
  {
    audioPlayer.setVolume( std::stoi(lichtwecker.configGetElement("nVolume").c_str())/30 );
  }
  /* -------------------------------------------------- */
  /* broadcast to all                                   */
  lichtwecker.broadcastMessage((const char *)ptr);
  /* ================================================== */
  /* enable the timer                                    */
  for(auto & elem : lichtwecker.getNextionDisplay()->getNexTimer() )
  {
    elem->enable();
  }
  /* =====END========================================== */
}

/**
 * @brief push callback btSound
 * 
 * @param ptr 
 */
void page0_btSoundPushCallback(void *ptr)
{
  getVolumeFromNextion();

  iTemp = 0;
  iTemp = getBtValFromNextion("btSound");
  if( iTemp == 0)
  {
    audioPlayer.alaramOn();
  }
  else
  {
    audioPlayer.alaramOff();
  }
}

/**
 * @brief puch calback btTimeSync
 * 
 */
void page2_btTimeSyncPushCallback(void *ptr)
{
  lichtwecker.getNexRtc()->updateDateTime();
}

/**
 * @brief push callback btSnooze
 * 
 * @param ptr 
 */
void page3_btSnoozePushCallback(void *ptr)
{
  /* -------------------------------------------------- */
  iTemp = 0;
  lichtwecker.getNextionDisplay()->getNexButtonByName("btSnooze")->getValue(&iTemp);
  if( iTemp == 1 )
  {
    // snooze_on
    /* ----------------------------------------------- */
    /* broadcast to all                                */
    lichtwecker.broadcastMessage("SnoozeOn");
  }
  else
  {
    // snooze_off
    /* ----------------------------------------------- */
    /* broadcast to all                                */
    lichtwecker.broadcastMessage("SnoozeOff");
  }
}
/**
 * @brief push callback btSnooze
 * 
 * @param ptr 
 */
void page6_btLightTestPushCallback(void *ptr)
{
  /* -------------------------------------------------- */
  iTemp = 0;

  lichtwecker.getNextionDisplay()->getNexButtonByName("btLightTest")->getValue(&iTemp);

  if( iTemp == 1 )
  {
    lichtwecker.getSimpleSun()->setWhite(std::stoi(lichtwecker.configGetElement("nWhite").c_str()));
    lichtwecker.getSimpleSun()->setRed(  std::stoi(lichtwecker.configGetElement("nRet"  ).c_str()));
    lichtwecker.getSimpleSun()->setGreen(std::stoi(lichtwecker.configGetElement("nGreen").c_str()));
    lichtwecker.getSimpleSun()->setBlue( std::stoi(lichtwecker.configGetElement("nBlue" ).c_str())); 
    /* ----------------------------------------------- */
    /* broadcast to all                                */
    lichtwecker.broadcastMessage("LightTestOn");
  }
  else
  {
    /* ----------------------------------------------- */
    /* broadcast to all                                */
    lichtwecker.broadcastMessage("LightTestOff");
  }
}
/** 
 * === END CALLBACK SECTION ===
 */

/* ================================================== */
/* setup and main loop                                */
/* ================================================== */
/**
 * @brief setup() function
 * 
 */
void setup(void)
{
  /*-------------------------------------------------------*/
  /* init serial db                                        */
  dbSerial.begin(dbSerialSpeed);
  dbSerialPrintf("Start LICHTWECKER 4.0");
  /*-------------------------------------------------------*/
  /* start filesystem                                      */
  if(!SPIFFS.begin())
  {
      dbSerialPrintln("ERROR: An Error has occurred while mounting SPIFFS.");
      vTaskDelay(5000/portTICK_PERIOD_MS);
      ESP.restart();
  }
  /*-------------------------------------------------------*/
  /* connect to Wifi                                       */
  if(!wifiServer.connectWifi())
  {
      dbSerialPrintln("ERROR: Can not connect to WIFI.");
      vTaskDelay(5000/portTICK_PERIOD_MS);
      ESP.restart();
  } 
  /*-------------------------------------------------------*/
  /* just wait a while                                     */
  vTaskDelay(100/portTICK_PERIOD_MS);
  /*-------------------------------------------------------*/
  /* start HTTP server                                     */
  webServer.loadWebServer(&SPIFFS);
  /*-------------------------------------------------------*/
  /* startup lichtwecker                                   */
  lichtwecker.start();
  lichtwecker.getSimpleSun()->setTaskFunction(taskSunLoopCode);                
  lichtwecker.getSimpleSun()->setTimerCB( sunRiseMain );
  vTaskDelay(100/portTICK_PERIOD_MS);
  /*-------------------------------------------------------*/
  /* start audio                                           */
  audioPlayer.begin();
  audioPlayer.registerCB(lichtwecker.getMDispatcher());
  audioPlayer.setTaskFunction(taskSoundLoopCode);  
  vTaskDelay(100/portTICK_PERIOD_MS);
  /*-------------------------------------------------------*/
  /* start up tasks Sun & Sound                            */
  lichtwecker.getSimpleSun()->startSunLoopTask();
  vTaskDelay(100/portTICK_PERIOD_MS);
  audioPlayer.startSoundLoopTask();
  vTaskDelay(100/portTICK_PERIOD_MS);
  /*-------------------------------------------------------*/
  /* create task for touch events                          */
  xTaskCreatePinnedToCore(
                  taskNexLoopCode,        /* Task function.                            */
                  "TaskNexLoop",          /* name of task.                             */
                  4096,                   /* Stack size of task                        */
                  NULL,                   /* parameter of the task                     */
                  1,                      /* priority of the task                      */
                  &hTaskNexLoop,          /* Task handle to keep track of created task */
                  0                       /* pin task to core 0                        */
  );      
  vTaskDelay(100/portTICK_PERIOD_MS);
  /*-------------------------------------------------------*/
  /* create task for web-server                            */
  xTaskCreatePinnedToCore(
                taskWebServerCode,        /* Task function. */
                "TaskWebServer",          /* name of task. */
                3072,                     /* Stack size of task */
                NULL,                     /* parameter of the task */
                1,                        /* priority of the task */
                &hTaskWebServer,          /* Task handle to keep track of created task */
                0);                       /* pin task to core 0 */                    
  vTaskDelay(100/portTICK_PERIOD_MS);
}
/**
 * @brief loop() function
 * 
 */
void loop(void){
  /* -------------------------------------------------- */
  /* we need the loop just for start up tests           */
  lichtwecker.getSimpleSun()->lightOn();
  vTaskDelay(2000/portTICK_PERIOD_MS);
  lichtwecker.getSimpleSun()->lightOff();
  vTaskDelay(100/portTICK_PERIOD_MS);
  /* ---------------------------------------------- */
  /* get volume from nextion dixplay                */
  getVolumeFromNextion();
  /* ---------------------------------------------- */
  /* test alarm again                               */
  audioPlayer.alaramOn();
  vTaskDelay(1000/portTICK_PERIOD_MS);
  audioPlayer.alaramOff();
  /* ---------------------------------------------- */
  /* delete the task                                */
  vTaskDelete(NULL);
}
