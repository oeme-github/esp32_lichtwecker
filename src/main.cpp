#include <Math.h>
#include <Lichtwecker.h>
#include <CallbackFunctions.h>
#include <WebServer.h> 
#include "MyAudioPlayer.h"
#include "ArduinoJson.h"
#include <SPIFFS.h>

/**
 * @brief create instance of Lichtwecker
 */
Lichtwecker lichtwecker;
MyAudioPlayer myAudioPlayer;

// used only internally
int fileSize  = 0;
bool result   = true;

/** 
 * === TASKS ===
 */
TaskHandle_t hTaskNexLoop;
TaskHandle_t hTaskWebServer;


/**
 * @brief Get the Volume From Nextion object
 * 
 */
void getVolumeFromNextion()
{
  uint32_t iTemp = -1;
  float fVol     = 0.5;
  for(int i=0; i<=5; i++)
  {
      lichtwecker.getNextionDisplay()->getNexVariableByName("vaVolume")->getValue(&iTemp);
      if(iTemp >= 0)
      {
          break;
      }
      if(iTemp<0)
      {
        iTemp = 10;
      }
  }
  fVol = (float)iTemp/(float)30;
  myAudioPlayer.setVolume(fVol);
}
/**
 * @brief Get the Bt Val From Nextion object
 * 
 * @param bt 
 * @return int 
 */
int getBtValFromNextion(const char *btName)
{
   uint32_t iTemp = -1;
  /* ---------------------------------------------- */
  /* get the offset from nextion dixplay            */
  for(int i=0; i<=5; i++)
  {
      lichtwecker.getNextionDisplay()->getNexButtonByName(btName)->getValue(&iTemp);
      if(iTemp >= 0)
      {
          break;
      }
  }
  return iTemp;
}

/**
 * @brief taskNexLoopCode()
 * 
 * @param pvParameters 
 */
void taskNexLoopCode( void * pvParameters ){
  dbSerialPrint( "taskNexLoop running on core ");
  dbSerialPrintln( xPortGetCoreID() );

  while(true)
  {    
    // watch the touch events  
    nexLoop(lichtwecker.getNextionDisplay()->getNexListenList().data(), lichtwecker.getNextionDisplay()->getNexListenList().size());
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
/**
 * @brief taskSunLoopCode()
 * 
 * @param pvParameters 
 */
void taskSunLoopCode( void * pvParameters ){
  
  dbSerialPrint( "taskSunLoop running on core " );
  dbSerialPrintln( xPortGetCoreID() );

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
    vTaskDelay(2/portTICK_PERIOD_MS);
  }
}

/**
 * @brief taskSoundLoopCode()
 * 
 * @param pvParameters 
 */
void taskSoundLoopCode( void * pvParameters ){
  
  dbSerialPrint( "taskSoundLoop running on core " );
  dbSerialPrintln( xPortGetCoreID() );

  while(true)
  {
    if( myAudioPlayer.getActive() == true )
    {
      /* -------------------------------------------------- */    
      /* check the sun and run it                           */
      /* need to delay the task because of warchdog         */
      myAudioPlayer.play();
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
/** 
 * === END TASKS SECTION ===
 */

/**
 * === Web-Server functions ===
 */

/**
 * @brief handle upload rft files to nextion
 * 
 * @return true 
 * @return false 
 */
bool handleFileUpload()
{
    /* -------------------------------------------------- */
    /* prepare upload server                              */
    HTTPUpload& upload = lichtwecker.getWebServer()->upload();
    /* -------------------------------------------------- */
    /* Check if file seems valid nextion tft file         */
    if(!upload.filename.endsWith(F(".tft"))){
        lichtwecker.getWebServer()->send(500, F("text/plain"), F("ONLY TFT FILES ALLOWED\n"));
        return false;
    }
    /* -------------------------------------------------- */
    /* handle send errors                                 */  
    if(!result){
        /* ---------------------------------------------- */
        lichtwecker.getWebServer()->sendHeader(F("Location"),"/failure.html?reason=" + lichtwecker.getEspNexUpload()->statusMessage);
        lichtwecker.getWebServer()->send(303);
        return false;
    }
    /* -------------------------------------------------- */
    /* check upload status                                */
    if(upload.status == UPLOAD_FILE_START){
        /* ---------------------------------------------- */
        /* Prepare the Nextion display by seting up serial*/ 
        /* and telling it the file size to expect         */
        result = lichtwecker.getEspNexUpload()->prepareUpload(fileSize);    
        if(result){
            dbSerialPrint("Start upload. File size is: ");
            dbSerialPrintln( (String(fileSize)+ String(" bytes")).c_str() );
        }else{
            dbSerialPrintln(lichtwecker.getEspNexUpload()->statusMessage.c_str());
            return false;
        }      
    }
    else if(upload.status == UPLOAD_FILE_WRITE)
    {
        /* ---------------------------------------------- */
        /* Write the received bytes to the nextion        */
        result = lichtwecker.getEspNexUpload()->upload(upload.buf, upload.currentSize);    
        if(result){
            dbSerialPrint(".");
        }else{
            dbSerialPrintln(lichtwecker.getEspNexUpload()->statusMessage.c_str());
            return false;
        }
        vTaskDelay(10);
    }
    else if(upload.status == UPLOAD_FILE_END)
    {
        /* ---------------------------------------------- */
        /* End the serial connection to the Nextion and   */
        /* softrest it                                    */
        lichtwecker.getEspNexUpload()->end();
    }
    /* -------------------------------------------------- */
    /* default return                                     */
    return true;
}
/**
 * @brief handle success page
 * 
 * @return true 
 * @return false 
 */
bool handleSuccess()
{
    dbSerialPrintln(F("Succesfully updated Nextion!\n"));
    // Redirect the client to the success page after handeling the file upload
    lichtwecker.getWebServer()->sendHeader(F("Location"),F("/success.html"));
    lichtwecker.getWebServer()->send(303);
    vTaskDelay(100/portTICK_PERIOD_MS);
    ESP.restart();
    return true;
}
/**
 * @brief htaskWebServerCode()
 * 
 * @param pvParameters 
 */
void taskWebServerCode( void * pvParameters ){
    dbSerialPrint("taskWebServerCode running on core ");
    dbSerialPrintln(xPortGetCoreID());
    /* -------------------------------------------------- */
    /* switch webserver on                                */ 
    lichtwecker.getWebServer()->on( "/"
                                  , HTTP_POST
                                  , handleSuccess
                                  , handleFileUpload
    );
    /* -------------------------------------------------- */ 
    /* receive fileSize once a file is selected           */
    /* (Workaround as the file content-length is          */
    /* of by +/- 200 bytes.                               */
    /* Known issue:                                       */ 
    /*    https://github.com/esp8266/Arduino/issues/3787) */
    lichtwecker.getWebServer()->on( "/fs"
                                  , HTTP_POST
                                  , [](){
                                          fileSize = lichtwecker.getWebServer()->arg(F("fileSize")).toInt();
                                          lichtwecker.getWebServer()->send(200, F("text/plain"), "");
                                        }
    );
    /* -------------------------------------------------- */ 
    /* called when the url is not defined here use it to  */
    /* load content from SPIFFS                           */
    lichtwecker.getWebServer()->onNotFound([](){
                                                  if( !lichtwecker.handleFileRead( lichtwecker.getWebServer()->uri() ) )
                                                  {
                                                      lichtwecker.getWebServer()->send(404, F("text/plain"), F("FileNotFound"));
                                                  }
                                                }
    );
    /* -------------------------------------------------- */ 
    /* start HTTP server                                  */
    lichtwecker.getWebServer()->begin();
    dbSerialPrintln(F("\nHTTP server started"));
    /* -------------------------------------------------- */ 
    /* catch upload server events                         */
    while(true)
    {
        lichtwecker.getWebServer()->handleClient();
        vTaskDelay(50/portTICK_PERIOD_MS);
    }
}

/**
 * === END Web-Server functions ===
 */

/**
 * === Sun functions ===
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
 * === END Sun functions ===
 */

/** 
 * === NEXTION CALLBACK SECTION ===
 */
/**
 * @brief button callback 
 * 
 * @param ptr 
 */
void page2_btResetPushCallback(void *ptr)
{
  dbSerialPrint("page2_btResetPushCallback(");
  dbSerialPrint(F((String *)ptr));
  dbSerialPrintln(")");

  vTaskDelay(100/portTICK_PERIOD_MS);
  ESP.restart();
} 
/**
 * @brief timer callback for serial communication
 * 
 * @param ptr 
 */
void page_save_values(void *ptr)
{
  dbSerialPrint("page_save_values(");
  dbSerialPrint(F((String *)ptr));
  dbSerialPrintln(")");

  lichtwecker.saveToConfigfile((const char *)ptr);
} 
/**
 * @brief timer callback for serial communication
 * 
 * @param ptr 
 */
void page0_tmSerialCmdCallback(void *ptr)
{
  dbSerialPrint("tmSerialCmdCallback(");
  dbSerialPrint(F((String *)ptr));
  dbSerialPrintln(")");
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
      uint32_t iOffsetSun = 0;
      /* ---------------------------------------------- */
      /* get the offset from nextion dixplay            */
      for(int i=0; i<=5; i++)
      {
          lichtwecker.getNextionDisplay()->getNexVariableByName("vaOffsetSun")->getValue(&iOffsetSun);
          if(iOffsetSun > 0)
          {
              break;
          }
      }
      if(iOffsetSun==0)
      {
          iOffsetSun=WAKE_DELAY;
      }
      lichtwecker.getSimpleSun()->setWakeDelay(iOffsetSun);  
  }
  /* -------------------------------------------------- */
  /* sunup -> start sunrise                             */
  if( (strncmp("light_on", (char *)ptr, 8 ) == 0))  
  {
      uint32_t iBrightness = 0;
      /* ---------------------------------------------- */
      /* get the offset from nextion dixplay            */
      for(int i=0; i<=5; i++)
      {
          lichtwecker.getNextionDisplay()->getNexVariableByName("vaBright")->getValue(&iBrightness);
          if(iBrightness > 0)
          {
              break;
          }
      }
      if(iBrightness==0)
      {
          iBrightness=10;
      }
      lichtwecker.getSimpleSun()->setBrightness(iBrightness);  
  }
  /* -------------------------------------------------- */
  /* get volume                                         */
  if( (strncmp("ala_up", (char *)ptr, 6 ) == 0))  
  {
    uint32_t iTemp = 0;
    for(int i=0; i<=5; i++)
    {
        lichtwecker.getNextionDisplay()->getNexVariableByName("vaVolume")->getValue(&iTemp);
        if(iTemp > 0)
        {
            myAudioPlayer.setVolume( iTemp/30 );
            break;
        }
    }
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

  int iValButton = getBtValFromNextion("btSound");
  if( iValButton == 0)
  {
    myAudioPlayer.alaramOn();
  }
  else if(iValButton == 1)
  {
    myAudioPlayer.alaramOff();
  }
  else
  {
    dbSerialPrintln("ERROR: something went wrong.");
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
  uint32_t bValue = 0;
  lichtwecker.getNextionDisplay()->getNexButtonByName("btSnooze")->getValue(&bValue);
  if( bValue == 1 )
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
  uint32_t bValue = 0;
  uint32_t iWhite = 0;
  uint32_t iRed   = 0;
  uint32_t iGreen = 0;
  uint32_t iBlue  = 0;

  lichtwecker.getNextionDisplay()->getNexButtonByName("btLightTest")->getValue(&bValue);

  if( bValue == 1 )
  {
    lichtwecker.getNextionDisplay()->getNexNumberByName("nWhite")->getValue(&iWhite);
    lichtwecker.getNextionDisplay()->getNexNumberByName("nRed")->getValue(&iRed);
    lichtwecker.getNextionDisplay()->getNexNumberByName("nGreen")->getValue(&iGreen);
    lichtwecker.getNextionDisplay()->getNexNumberByName("nBlue")->getValue(&iBlue);

    lichtwecker.getSimpleSun()->setWhite(iWhite);
    lichtwecker.getSimpleSun()->setRed(iRed);
    lichtwecker.getSimpleSun()->setGreen(iGreen);
    lichtwecker.getSimpleSun()->setBlue(iBlue);
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
    /* -------------------------------------------------- */
    /* init serial db                                     */
    dbSerial.begin(dbSerialSpeed);
    dbSerialPrintln("\nStart LICHTWECKER 4.0\n");
    /* -------------------------------------------------- */
    /* startup lichtwecker                                */
    lichtwecker.start();
    lichtwecker.getSimpleSun()->setTaskFunction(taskSunLoopCode);                
    lichtwecker.getSimpleSun()->setTimerCB( sunRiseMain );
    vTaskDelay(100/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* start audio                                        */
    myAudioPlayer.begin();
    myAudioPlayer.registerCB(lichtwecker.getMDispatcher());
    myAudioPlayer.setTaskFunction(taskSoundLoopCode);  
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(100/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->startSunLoopTask();
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(100/portTICK_PERIOD_MS);
    myAudioPlayer.startSoundLoopTask();
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(100/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* create task for touch events                       */
    xTaskCreatePinnedToCore(
                    taskNexLoopCode,        /* Task function.                            */
                    "TaskNexLoop",          /* name of task.                             */
                    4096,                   /* Stack size of task                        */
                    NULL,                   /* parameter of the task                     */
                    1,                      /* priority of the task                      */
                    &hTaskNexLoop,          /* Task handle to keep track of created task */
                    0                       /* pin task to core 0                        */
    );      
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(100/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* start web-server                                   */
    xTaskCreatePinnedToCore(
                  taskWebServerCode,        /* Task function. */
                  "TaskWebServer",          /* name of task. */
                  4096,                     /* Stack size of task */
                  NULL,                     /* parameter of the task */
                  1,                        /* priority of the task */
                  &hTaskWebServer,          /* Task handle to keep track of created task */
                  0);                       /* pin task to core 0 */                    
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(100/portTICK_PERIOD_MS);
}
/**
 * @brief loop() function
 * 
 */
void loop(void){

    dbSerialPrint("loop() - Core: ");
    dbSerialPrintln(xPortGetCoreID());
    /* -------------------------------------------------- */
    /* we need the loop just for start up tests           */
    lichtwecker.getSimpleSun()->lightOn();
    vTaskDelay(2000/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->lightOff();
    vTaskDelay(100/portTICK_PERIOD_MS);

    uint32_t iTemp = 0;
    /* ---------------------------------------------- */
    /* get the offset from nextion dixplay            */
    for(int i=0; i<=5; i++)
    {
        lichtwecker.getNextionDisplay()->getNexVariableByName("vaOffsetSun")->getValue(&iTemp);
        if(iTemp > 0)
        {
            lichtwecker.getSimpleSun()->setWakeDelay(iTemp);
            break;
        }
    }
    /* ---------------------------------------------- */
    /* get the offset from nextion dixplay            */
    for(int i=0; i<=5; i++)
    {
        lichtwecker.getNextionDisplay()->getNexVariableByName("vaBright")->getValue(&iTemp);
        if(iTemp > 0)
        {
            lichtwecker.getSimpleSun()->setBrightness(iTemp);
            break;
        }
    }
    /* ALARAM TEST */
    /* ---------------------------------------------- */
    /* get volume from nextion dixplay                */
    getVolumeFromNextion();
    /* ---------------------------------------------- */
    /* test alarm again                               */
    myAudioPlayer.alaramOn();
    vTaskDelay(1000/portTICK_PERIOD_MS);
    myAudioPlayer.alaramOff();
    /* ---------------------------------------------- */
    /* delete the task                                */
    vTaskDelete(NULL);
}
