#include <Lichtwecker.h>
#include <CallbackFunctions.h>

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


/** 
 * === TASKS ===
 */
TaskHandle_t hTaskNexLoop;

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
      /* run the timer                                    */
#ifdef _WITH_LOOP_DEBUG_
      dbSerialPrint( "SunState:" );
      dbSerialPrintln( lichtwecker.getSimpleSun()->getSunState() );
#endif
      if( strcmp(lichtwecker.getSimpleSun()->getSunState(), "SunRise") == 0 )
      {
        lichtwecker.getSimpleSun()->run();
      }
    }
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
 * @brief function to increase the sun phase
 * 
 */
void sunRiseMain()
{
#ifdef SHOW_DEBUG_DETAILS
  dbSerialPrint("sunRise()...");
  dbSerialPrintln( simpleSun.getSunPhase());
#endif
  /* let sun rise (without parameter) */
  lichtwecker.getSimpleSun()->letSunRise();
}

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
  /* ================================================== */
  /* Check what tot do?                                 */
  /* -------------------------------------------------- */
  /* switch light on                                    */
  if( (strncmp("light_on", (char *)ptr, 8 ) == 0))  
  {
    dbSerialPrintln("light_on");
    lichtwecker.getSimpleSun()->lightOn();
  }
  /* -------------------------------------------------- */
  /* switch light off                                   */
  if( (strncmp("light_off", (char *)ptr, 9 ) == 0))  
  {
    dbSerialPrintln("light_off");
    lichtwecker.getSimpleSun()->lightOff();
  }
  /* -------------------------------------------------- */
  /* sunup -> start sunrise                             */
  if( (strncmp("sunup", (char *)ptr, 5 ) == 0))  
  {
    dbSerialPrintln("sunup -> SunRise()");
    uint32_t iOffsetSun = 0;
    /* start loop task                                  */
    dbSerialPrintln("create sun loop task...");
    /* get the offset from nextion dixplay              */
    for(int i=0; i<=5; i++)
    {
      lichtwecker.getNextionDisplay()->getNexVariableByName("vaOffsetSun")->getValue(&iOffsetSun);
      if(iOffsetSun > 0)
      {
        lichtwecker.setOffsetSun(iOffsetSun);
        break;
      }
    }
    if(iOffsetSun==0)
    {
      iOffsetSun=WAKE_DELAY;
    }
    lichtwecker.getSimpleSun()->setWakeDelay(iOffsetSun);  
    lichtwecker.getSimpleSun()->setTimerCB( sunRiseMain );
    lichtwecker.getSimpleSun()->sunRise();
  }
  /* -------------------------------------------------- */
  /* alaup -> switch alarm on                           */
  if( (strncmp("alaup", (char *)ptr, 5 ) == 0))  
  {
    dbSerialPrintln("alaup");
    lichtwecker.getDFPlayer()->alaramOn();
  }
  /* -------------------------------------------------- */
  /* a_off -> switch alarm off                          */
  if( (strncmp("a_off", (char *)ptr, 5 ) == 0))  
  {
    dbSerialPrintln("a_off");
    lichtwecker.getDFPlayer()->alaramOff();
  }
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
  // var
//   uint32_t bValue;
//   if( btSnooze.getValue(&bValue) )
//   {
//     // we have the value
//     if( bValue == 1 )
//     {
//       // snooze_on
//       dbSerialPrintln("snooze_on");
//       g_bSnoozeOnOff = true;
//     }
//     else
//     {
//       // snooze_off
//       dbSerialPrintln("snooze_off");
//       g_bSnoozeOnOff = false;
//     }
//   }
}

/** 
 * === END CALLBACK SECTION ===
 */


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
    /* -------------------------------------------------- */
    /* set function for sound loop task                   */
    lichtwecker.getDFPlayer()->setTaskFunction(taskSoundLoopCode);                
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
                    1                       /* pin task to core 1                        */
    );      
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
#endif
    for(int i=1;i<8;i++)
    {
        lichtwecker.getSimpleSun()->lightBlue();
        vTaskDelay(BL_DELAY/portTICK_PERIOD_MS);
        lichtwecker.getSimpleSun()->lightOff();
        vTaskDelay(BL_DELAY/portTICK_PERIOD_MS);
    }
    lichtwecker.getSimpleSun()->lightOn();
    vTaskDelay(5000/portTICK_PERIOD_MS);
    lichtwecker.getSimpleSun()->lightOff();
    lichtwecker.getDFPlayer()->play(kodack);
    /* -------------------------------------------------- */
    /* delete the task                                    */
    vTaskDelete(NULL);
}
