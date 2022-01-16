#include <MyDFPlayer.h>
#include <sound_table.h>

/**
 * @brief Construct a new MyDFPlayer::MyDFPlayer object
 * 
 * @param rx_ 
 * @param tx_ 
 */
MyDFPlayer::MyDFPlayer(int8_t rx_, int8_t tx_):softwareSerial(rx_,tx_)
{
  /* -------------------------------------------------- */
  /* set pin mode for busy chanal                       */
  pinMode(GPIO_MP3, INPUT);
  /* -------------------------------------------------- */
  /* init states                                        */
  AlarmState::init<AlarmOff>(*this, alarmState);
  this->snoozeInit();
}

/**
 * @brief Destroy the My D F Player:: My D F Player object
 * 
 */
MyDFPlayer::~MyDFPlayer()
{
}

/**
 * @brief getDetail() returns information of the dfplayers state 
 * 
 * @param type 
 * @param value 
 * @return const char* 
 */
const char *MyDFPlayer::getDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      return "Time Out!";
      break;
    case WrongStack:
      return "Stack Wrong!";
      break;
    case DFPlayerCardInserted:
      return "Card Inserted!";
      break;
    case DFPlayerCardRemoved:
      return "Card Removed!";
      break;
    case DFPlayerCardOnline:
      return "Card Online!";
      break;
    case DFPlayerPlayFinished:
        return (String("Number: ") + value + String(" Play Finished!")).c_str();
      break;
    case DFPlayerError:
      switch (value) {
        case Busy:
          return "DFPlayerError: Card not found";
          break;
        case Sleeping:
          return "DFPlayerError: Sleeping";
          break;
        case SerialWrongStack:
          return "DFPlayerError: Get Wrong Stack";
          break;
        case CheckSumNotMatch:
          return "DFPlayerError: Check Sum Not Match";
          break;
        case FileIndexOut:
          return "DFPlayerError: File Index Out of Bound";
          break;
        case FileMismatch:
          return "DFPlayerError: Cannot Find File";
          break;
        case Advertise:
          return "DFPlayerError: In Advertise";
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return "okay";
}

/**
 * @brief startDFPlayer() starts up the dfplayer mini
 * 
 */
void MyDFPlayer::startDFPlayer()
{
  print("DFRobot DFPlayer Mini Demo");
  print("Initializing DFPlayer ... (May take 3~5 seconds)");
  /* -------------------------------------------------- */
  /* softwareserail                                     */
  getSoftwareSerial()->begin(9600);
  /* -------------------------------------------------- */
  /* DFPlayer begin                                     */  
  if (!_begin_()) 
  {   
    print("Unable to begin:");
    print("1.Please recheck the connection!");
    print("2.Please insert the SD card!");
    while(true);
  }
  print("DFPlayer Mini online.");
  /* -------------------------------------------------- */
  /* test sound                                         */
  int8_t file_count = readFileCounts();
  print("Files:", false);
  print(file_count);

#ifdef _WITH_SOUND_TEST_
  print("start up - test all sounds...");
  for(int i=1; i<=file_count; i++)
  {
    print("sound: ", false); print(i);
    play(i);
    vTaskDelay(1200/portTICK_PERIOD_MS);
    while(!digitalRead(GPIO_MP3))
    {
      vTaskDelay(1000/portTICK_PERIOD_MS);
      print("player still busy...");
    }        
  }   
#endif
}

/**
 * @brief create sun loop task
 * 
 */
void MyDFPlayer::startSoundLoopTask()
{
    /* -------------------------------------------------- */
    /* start the sound task                               */
    xTaskCreatePinnedToCore(
                    this->pvTaskCode,       /* Task function. */
                    "TaskSoundLoop",        /* name of task. */
                    10000,                  /* Stack size of task */
                    NULL,                   /* parameter of the task */
                    2 | portPRIVILEGE_BIT,  /* priority of the task */
                    &this->hTaskSoundLoop,  /* Task handle to keep track of created task */
                    0                       /* pin task to core 1 */
    );                          
    /* --------------------------------------------------- */
    /* suspend the sound task                              */
    this->suspendSoundLoopTask();
}
/**
 * @brief suspend sound loop task
 * 
 */
void MyDFPlayer::suspendSoundLoopTask()
{
    /* --------------------------------------------------- */
    /* stop the player                                     */
    this->stop();
    this->sleep();
    vTaskSuspend(this->hTaskSoundLoop);
    //this->setPlay(false);
}
/**
 * @brief resume the soundloop task
 * 
 */
void MyDFPlayer::resumeSoundLoopTask()
{
    /* --------------------------------------------------- */
    /* set the volume ans start the player                 */  
    this->volume(this->iVolume);
    this->start();
    /* --------------------------------------------------- */
    /* resume the task                                     */
    vTaskResume(this->hTaskSoundLoop);
    //this->setPlay(true);
}
/**
 * @brief listerner function for MDispatcher
 * 
 * @param string_ 
 * @param event_ 
 */
void MyDFPlayer::listener(String string_, EventEnum event_) {
    print( "listener() for ", false);
    print( "MyDFPlayer"     , false);
    print( ", got: "        , false );
    print( string_.c_str()  , false );
    print( ", "             , false );
    print( event_ ); 
    print( "AlarmState: ", false );
    print( this->getAlarmState() );
    /* -------------------------------------------------- */
    /* alaup -> switch alarm on                           */
    if( (strcmp("alaup", string_.c_str() ) == 0))  
    {
        print("alaup");
        this->alaramOn();
    }
    /* -------------------------------------------------- */
    /* a_off -> switch alarm off                          */
    if( (strcmp("a_off", string_.c_str() ) == 0))  
    {
        print("a_off");
        this->alaramOff();
    }
    /* -------------------------------------------------- */
    /* SnoozeOn -> switch snooze on                       */
    if( (strcmp("SnoozeOn", string_.c_str() ) == 0) || (strcmp("snoozeon", string_.c_str() ) == 0))  
    {
        print("SnoozeOn");
        //vTaskSuspend(this->hTaskSoundLoop);
        this->setPlay(false);
    }
    /* -------------------------------------------------- */
    /* SnoozeOff -> switch snooze off                       */
    if( (strcmp("snoozeoff", string_.c_str() ) == 0) || (strcmp("SnoozeOff", string_.c_str() ) == 0))  
    {
        print("SnoozeOff");
        //vTaskResume(this->hTaskSoundLoop);
        this->setPlay(true);
    }
}    
/**
 * @brief switch snooze on
 * 
 * @param snooze_ 
 */
void MyDFPlayer::SnoozeOn::snooze(MyDFPlayer::Snooze_t snooze_)
{
     switch (snooze_)
     {
     case SNOOZE_OFF:
          print("SnoozeOn machine SNOOZE_OFF");
          change<SnoozeOff>();
          break;
     
     default:
          print("snooze already on");
          break;
     }
}
/**
 * @brief switch snooze off
 * 
 * @param snooze_ 
 */
void MyDFPlayer::SnoozeOff::snooze(MyDFPlayer::Snooze_t snooze_)
{
     switch (snooze_)
     {
     case SNOOZE_ON:
          print("SnoozeOff machine SNOOZE_ON");
          change<SnoozeOn>();
          break;
     
     default:
          print("snooze already off");
          break;
     }
}
