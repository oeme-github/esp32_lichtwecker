#include <Lichtwecker.h>
#include <CallbackFunctions.h>

#include <WebServer.h>

#define _DEBUG_SUNRISE_

/**
 * @brief define section
 */
#define SOUND_TABLE \
X(no_sound)  \
X(piep)    \
X(amsel)   \
X(pmh)     \
X(hahn)    \
X(sirene)  \
X(kodack)  \
X(qubodup) \
X(samster)

#define X(a) a,
typedef enum SOUND_T {
  SOUND_TABLE
} sound_t;
#undef X

#define X(a) +1
int sound_count = 0 SOUND_TABLE;
#undef X

#define WAKE_DELAY 1800

/**
 * @brief create instance of Lichtwecker
 */
Lichtwecker lichtwecker;

// used only internally
int fileSize  = 0;
bool result   = true;

/** 
 * === TASKS ===
 */
TaskHandle_t hTaskNexLoop;
TaskHandle_t hTaskWebServer;

/**
 * @brief taskNexLoopCode()
 * 
 * @param pvParameters 
 */
void taskNexLoopCode( void * pvParameters ){
  dbSerialPrintln( "taskNexLoop running on core " + xPortGetCoreID());

  while(true)
  {    
    // watch the touch events  
    nexLoop(lichtwecker.getNextionDisplay()->getNexListenList().data());
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
#ifdef _WITH_LOOP_DEBUG_
      dbSerialPrint( "SunState:" );
      dbSerialPrintln( lichtwecker.getSimpleSun()->getSunState() );
      dbSerialPrint( "SunRise? " );
      dbSerialPrintln( strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunRise") );
#endif
      if( strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunRise") == 0 
          || strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunSet") == 0 )
      {
#ifdef _WITH_LOOP_DEBUG_
          dbSunSerialPrintln("run the timer...");
#endif
          lichtwecker.getSimpleSun()->run();
      }
      else
      {
        dbSerialPrint( "TASK - SunState:" );
        dbSerialPrintln( lichtwecker.getSimpleSun()->getSunState() );
        vTaskSuspend(NULL);        
      }
    }
    /* -------------------------------------------------- */
    /* need to delay the task because of warchdog         */
    vTaskDelay(100/portTICK_PERIOD_MS);
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

  lichtwecker.getDFPlayer()->volume(10);
  while(true)
  {
    /* -------------------------------------------------- */    
    /* check the sun and run it                           */
    /* need to delay the task because of warchdog         */
    lichtwecker.getDFPlayer()->play( kodack);
    while(!digitalRead(GPIO_MP3))
    {
        dbSerialPrintln("player still busy...");
        vTaskDelay(100/portTICK_PERIOD_MS);
    }             
    vTaskDelay(2000/portTICK_PERIOD_MS);
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
    dbSerialPrint("hTaskWebServer running on core ");
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
        vTaskDelay(10/portTICK_PERIOD_MS);
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
#ifdef _DEBUG_SUNRISE_
  dbSerialPrint("sunRiseMain() - SunPhase: ");
  dbSerialPrintln( lichtwecker.getSimpleSun()->getSunPhase());
#endif
  if( lichtwecker.getSimpleSun()->getSunPhase() < 100 )
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
  dbSerialPrintln("disable timer...");
  for(auto & elem : lichtwecker.getNextionDisplay()->getNexTimer() )
  {
    dbSerialPrintln(elem->getName());
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
              dbSerialPrintln("got the offset...");
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
  /* alaup                                              */
  if((strncmp("alaup", (char *)ptr, 5 ) == 0))
  {
      uint32_t iVolume = 0; 
      lichtwecker.getNextionDisplay()->getNexVariableByName("vaVolume")->getValue(&iVolume);
      lichtwecker.getDFPlayer()->setVolume(iVolume);
  }
  /* -------------------------------------------------- */
  /* broadcast to all                                   */
  lichtwecker.broadcastMessage((const char *)ptr);
  /* ================================================== */
  /* enable the time                                    */
  dbSerialPrintln("enable timer...");
  for(auto & elem : lichtwecker.getNextionDisplay()->getNexTimer() )
  {
        dbSerialPrintln(elem->getName());
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
  dbSerialPrintln("btSoundPushCallback()");
  // var
//   uint32_t bValue;
//   if( btSound.getValue(&bValue) )
//   {
//     // we have the value
//     if( bValue == 1 )
//     {
//       // sound_on
//       dbSerialPrintln("sound_on");
//       g_bSoundOnOff = true;
//     }
//     else
//     {
//       // sound_off
//       dbSerialPrintln("sound_off");
//       g_bSoundOnOff = false;
//     }
//   }
}

/**
 * @brief puch calback btTimeSync
 * 
 */
void page2_btTimeSyncPushCallback(void *ptr)
{
  dbSerialPrintln("btTimeSyncPushCallback()");
  if( lichtwecker.getNexRtc()->updateDateTime() )
  {
    dbSerialPrintln("update complet.");
  }
  else
  {
    dbSerialPrintln("somthing went wrong!");
  }
}

/**
 * @brief push callback btSnooze
 * 
 * @param ptr 
 */
void page3_btSnoozePushCallback(void *ptr)
{
  dbSerialPrintln("btSnoozePushCallback()");
  /* -------------------------------------------------- */
  uint32_t bValue = 0;
  lichtwecker.getNextionDisplay()->getNexButtonByName("btSnooze")->getValue(&bValue);
  if( bValue == 1 )
  {
    // snooze_on
    dbSerialPrintln("snooze_on");
    /* ----------------------------------------------- */
    /* broadcast to all                                */
    lichtwecker.broadcastMessage("SnoozeOn");
  }
  else
  {
    // snooze_off
    dbSerialPrintln("snooze_off");
    /* ----------------------------------------------- */
    /* broadcast to all                                */
    lichtwecker.broadcastMessage("SnoozeOff");
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
    /* -------------------------------------------------- */
    /* set function for sun loop task                     */
    lichtwecker.getSimpleSun()->setTaskFunction(taskSunLoopCode);                
    lichtwecker.getSimpleSun()->setTimerCB( sunRiseMain );
    /* -------------------------------------------------- */
    /* set function for sound loop task                   */
    lichtwecker.getDFPlayer()->setTaskFunction(taskSoundLoopCode);  
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(500/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->startSunLoopTask();
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(500/portTICK_PERIOD_MS);
    lichtwecker.getDFPlayer()->startSoundLoopTask();
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(500/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* create task for touch events                       */
    xTaskCreatePinnedToCore(
                    taskNexLoopCode,        /* Task function.                            */
                    "TaskNexLoop",          /* name of task.                             */
                    10000,                  /* Stack size of task                        */
                    NULL,                   /* parameter of the task                     */
                    1,                      /* priority of the task                      */
                    &hTaskNexLoop,          /* Task handle to keep track of created task */
                    0                       /* pin task to core 1                        */
    );      
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(500/portTICK_PERIOD_MS);
    /* -------------------------------------------------- */
    /* start web-server                                   */
    xTaskCreatePinnedToCore(
                  taskWebServerCode,        /* Task function. */
                  "TaskWebServer",          /* name of task. */
                  10000,                    /* Stack size of task */
                  NULL,                     /* parameter of the task */
                  1,                        /* priority of the task */
                  &hTaskWebServer,          /* Task handle to keep track of created task */
                  1);                       /* pin task to core 0 */                    
    /* -------------------------------------------------- */
    /* just wait a while                                  */
    vTaskDelay(500/portTICK_PERIOD_MS);
}
/**
 * @brief loop() function
 * 
 */
void loop(void){
    /* -------------------------------------------------- */
    /* we need the loop just for start up tests           */
    dbSerialPrintln("loop: start test licht...");
#ifdef _WITH_SOUND_TEST_
    lichtwecker.getDFPlayer()->play(sirene);
    for(int i=1;i<8;i++)
    {
        lichtwecker.getSimpleSun()->lightBlue();
        vTaskDelay(BL_DELAY/portTICK_PERIOD_MS);
        lichtwecker.getSimpleSun()->lightOff();
        vTaskDelay(BL_DELAY/portTICK_PERIOD_MS);
    }
#endif
    lichtwecker.getSimpleSun()->lightOn();
    vTaskDelay(5000/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->lightOff();
    lichtwecker.getDFPlayer()->play(kodack);
#ifdef _WITH_SOUND_TEST_
    vTaskDelay(5000/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->sunUp();
    vTaskDelay(5000/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->sunDown();
    vTaskDelay(5000/portTICK_PERIOD_MS);

    lichtwecker.getSimpleSun()->setWakeDelay(1);
    lichtwecker.getSimpleSun()->sunRise();
    while ( strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunRise") == 0 )
    {
      dbSerialPrintln("===>>> sun still rising... <<<===");
      vTaskDelay(10000/portTICK_PERIOD_MS);
    }
    lichtwecker.getSimpleSun()->sunDown();
#endif
    /* -------------------------------------------------- */
    /* delete the task                                    */
    vTaskDelete(NULL);
}
