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
  pinMode(GPIO_MP3, INPUT);
  /* init states */
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
  /* softwareserail */
  getSoftwareSerial()->begin(9600);
  /* DFPlayer begin */  
  if (!_begin_()) 
  {   
    print("Unable to begin:");
    print("1.Please recheck the connection!");
    print("2.Please insert the SD card!");
    while(true);
  }
  print("DFPlayer Mini online.");
  /* test sound */
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
    xTaskCreatePinnedToCore(
                    this->pvTaskCode,       /* Task function. */
                    "TaskSoundLoop",        /* name of task. */
                    10000,                  /* Stack size of task */
                    NULL,                   /* parameter of the task */
                    1,                      /* priority of the task */
                    &this->hTaskSoundLoop,  /* Task handle to keep track of created task */
                    0                       /* pin task to core 1 */
    );                          
}

/**
 * @brief delete sound loop task
 * 
 */
void MyDFPlayer::stopSoundLoopTask()
{
    vTaskDelete(this->hTaskSoundLoop);
}
