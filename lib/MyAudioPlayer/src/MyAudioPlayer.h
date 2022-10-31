#pragma once

#include "SPIFFS.h"
#include "genericstate.h"
#include "MDispatcher.h"
#include <Nextion.h>

#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

/**
 * @brief audio 
 * 
 */
class MyAudioPlayer
{
private:
    I2SStream i2s; 
    MP3DecoderHelix helix;
    EncodedAudioStream out;
    StreamCopy copier;
    VolumeStream volume;
    LogarithmicVolumeControl lvc;

    I2SConfig cfg   = i2s.defaultConfig();
    int sample_rate = 24000;
    int channels    = 1;
    float fVolume   = 0.1;
    char filename[20] = "/double_beep.mp3";
    File audioFile;

    boolean bActive = false;

    TaskHandle_t hTaskSoundLoop;
    TaskFunction_t pvTaskCode;

public:
    enum Snooze_t
    {
        SNOOZE_ON,
        SNOOZE_OFF
    };

/**
 * @brief Constructor, Destructur
 * 
 */
public:
    /**
     * @brief Construct a new Audio Player object
     * 
     */
    MyAudioPlayer(){
        /* -------------------------------------------------- */
        /* init states                                        */
        AlarmState::init<AlarmOff>(*this, alarmState);
        this->snoozeInit();
    };
    /**
     * @brief Destroy the Audio Player object
     * 
     */
    ~MyAudioPlayer(){};

/**
 * @brief public methodes
 * 
 */
public:
    /**
     * @brief setup the audio
     * 
     */
    void begin()
    {
        // begin processing
        cfg.sample_rate = this->sample_rate;
        cfg.channels    = this->channels;
        i2s.begin(cfg);
        // set init volume
        volume.setTarget(i2s);
        volume.setVolumeControl(lvc);
        volume.setVolume(this->fVolume);
        // begin
        out.begin(&volume, &helix);
        copier.begin(out, audioFile);
    }
    /**
     * @brief register receiving function to MDispatcher
     * 
     * @param dispatcher 
     */
    void registerCB(MDispatcher<String, EventEnum> &dispatcher) 
    {
        using namespace std::placeholders;
        dispatcher.addCB(std::bind(&MyAudioPlayer::listener, this, _1, _2));
    }    
    /**
     * @brief play the sound file
     * 
     */
    void play() 
    {
        print("MyAudioPlayer::play(): ", false); print(this->filename); 
        if(SPIFFS.exists(this->filename))
        {
            this->audioFile = SPIFFS.open(this->filename, FILE_READ);
            helix.begin();
            volume.setVolume(this->fVolume);
            copier.copyAll();
            helix.end();
            audioFile.close();
        }
        else
        {
            print("file ", false); print(this->filename, false); print(" did not exists");
        }
    }
    /**
     * @brief start task for audio processing
     * 
     */ 
    void startSoundLoopTask()
    {
        print("MyAudioPlayer::startSoundLoopTask()");
        /* -------------------------------------------------- */
        /* start the sound task                               */
        xTaskCreatePinnedToCore(
                        this->pvTaskCode,       /* Task function. */
                        "TaskSoundLoop",        /* name of task. */
                        2048,                   /* Stack size of task */
                        NULL,                   /* parameter of the task */
                        2 | portPRIVILEGE_BIT,  /* priority of the task */
                        &this->hTaskSoundLoop,  /* Task handle to keep track of created task */
                        1                       /* pin task to core */
        );                          
        /* --------------------------------------------------- */
        /* suspend the sound task                              */
        suspendSoundLoopTask();
    }
    /**
     * @brief suspend sound loop task
     * 
     */
    void suspendSoundLoopTask()
    {
        print("MyAudioPlayer::suspendSoundLoopTask()");
        this->bActive=false;
        vTaskDelay(1500/portTICK_PERIOD_MS);
        vTaskSuspend(this->hTaskSoundLoop);
    }
    /**
     * @brief resume the soundloop task
     * 
     */
    void resumeSoundLoopTask()
    {
        print("MyAudioPlayer::resumeSoundLoopTask()");
        this->bActive=true;
        vTaskDelay(10/portTICK_PERIOD_MS);
        vTaskResume(this->hTaskSoundLoop);
    }
    /**
     * @brief Get the Active object
     * 
     * @return boolean 
     */
    boolean getActive(){return this->bActive;}
    /**
     * @brief Set the Task Function object
     * 
     * @param pvTaskCode_ 
     */
    void setTaskFunction( TaskFunction_t pvTaskCode_ ){ pvTaskCode = pvTaskCode_; }
    /**
     * @brief Get the Task Function object
     * 
     * @return TaskFunction_t 
     */
    TaskFunction_t getTaskFunction( ){ return pvTaskCode; }
    /**
     * @brief Set the Task Sound Loop object
     * 
     * @param hTaskSoundLoop_ 
     */
    void setTaskSoundLoop( TaskHandle_t hTaskSoundLoop_ ){  hTaskSoundLoop = hTaskSoundLoop_; }
    /**
     * @brief Get the Task Sound Loop object
     * 
     * @return TaskHandle_t 
     */
    TaskHandle_t getTaskSoundLoop( ){ return hTaskSoundLoop; }
    /**
     * @brief Set the Sample Rate object
     * 
     * @param sample_rate_ 
     */
    void setSampleRate(int sample_rate_){ this->sample_rate=sample_rate_; }
    /**
     * @brief Get the Sample Rate object
     * 
     * @return int 
     */
    int getSampleRate(){ return this->sample_rate; }
    /**
     * @brief Set the Channels object
     * 
     * @param sample_rate_ 
     */
    void setChannels(int channels_){ this->channels=channels_; }
    /**
     * @brief Get the Channels object
     * 
     * @return int 
     */
    int getChannels(){ return this->channels; }
    /**
     * @brief Set the Audio File object
     * 
     * @param filename_ 
     */
    void setFilename(char filename_[20]){ sprintf(this->filename, filename_); }
    /**
     * @brief Get the Audio File object
     * 
     * @return File
     */
    char *getFilename(){ return this->filename; }
    /**
     * @brief Set the Volume object
     * 
     * @param fVolume_ 
     */
    void setVolume( float_t fVolume_ ){ this->fVolume = fVolume_;}
    /**
     * @brief Get the Volume object
     * 
     * @return uint32_t 
     */
    uint32_t getVolume(){ return this->fVolume; }
    /**
     * @brief switch off alarm
     * 
     */
    void alaramOff(){ alarmState->alarmOff(); }
    /**
     * @brief switch on alarm
     * 
     */
    void alaramOn(){ alarmState->alarmOn(); }
    /**
     * @brief initialize snoozeState 
     * 
     */
    void snoozeInit(){ alarmState->initSnooze(); }
    /**
     * @brief switch snooze
     * 
     */
    void snooze_on_off(Snooze_t snooze_) { alarmState->snooze(snooze_); }
    /**
     * @brief Get the Alarm State object
     * 
     * @return const char* 
     */
    const char *getAlarmState(){ return alarmState->getAlarmState(); }
    /**
     * @brief listerner function for MDispatcher
     * 
     * @param string_ 
     * @param event_ 
     */
    void listener(String string_, EventEnum event_) {
        print( "listener() for ", false);
        print( "MyAudioPlayer"  , false);
        print( ", got: "        , false );
        print( string_.c_str()  , false );
        print( ", "             , false );
        print( event_ ); 
        print( "AlarmState: "   , false );
        print( this->getAlarmState() );
        /* -------------------------------------------------- */
        /* alaup -> switch alarm on                           */
        if( (strcmp("alaup", string_.c_str() ) == 0))  
        {
            print("alaup");
            alaramOn();
        }
        /* -------------------------------------------------- */
        /* a_off -> switch alarm off                          */
        if( (strcmp("a_off", string_.c_str() ) == 0))  
        {
            print("a_off");
            alaramOff();
        }
        /* -------------------------------------------------- */
        /* SnoozeOn -> switch snooze on                       */
        if( (strcmp("SnoozeOn", string_.c_str() ) == 0) || (strcmp("snoozeon", string_.c_str() ) == 0))  
        {
            print("SnoozeOn");
            vTaskSuspend(this->hTaskSoundLoop);
        }
        /* -------------------------------------------------- */
        /* SnoozeOff -> switch snooze off                       */
        if( (strcmp("snoozeoff", string_.c_str() ) == 0) || (strcmp("SnoozeOff", string_.c_str() ) == 0))  
        {
            print("SnoozeOff");
            vTaskResume(this->hTaskSoundLoop);
        }
    }    
/**
 * @brief private methodes
 * 
 */
private:
    /**
     * @brief print function for string 
     * 
     * @param str 
     * @param bNewLine 
     */
    static void print(const std::string &str, bool bNewLine = true) {
        if( bNewLine )
            dbSerialPrintln(str.c_str());
        else
            dbSerialPrint(str.c_str());
    }
    /**
     * @brief print function for int 
     * 
     * @param iNum 
     * @param bNewLine 
     */
    static void print(int iNum, bool bNewLine = false) { 
        if( bNewLine )
            dbSerialPrintln(iNum);
        else
            dbSerialPrintln(iNum);
    }
    /**
     * @brief print function for int
     * 
     * @param fNum 
     * @param bNewLine 
     */
    static void print(float_t fNum, bool bNewLine = true) {
        char msg[10];
        sprintf(msg,"%f",fNum); 
        if( bNewLine )
            dbSunSerialPrintln(msg);
        else
            dbSunSerialPrint(msg);
    }
    /**
     * @brief default unhandelt event function
     * 
     * @param str 
     */
    static void unhandledEvent(const std::string &str) { print("unhandled event " + str); }
    /**
     * @brief print snooze state after chage
     * 
     * @param snooze 
     */
    void changedSnooze(const std::string &snooze) { print( "changed snooze to " + snooze ); }
    /**
     * @brief generic snooze state
     * 
     */
    struct SnoozeState : public GenericState<MyAudioPlayer, SnoozeState> {
        using GenericState::GenericState;
        virtual void snooze(Snooze_t snooze_){ (void)snooze_; unhandledEvent("SnoozeState"); };
    };
    /**
     * @brief snooze on state
     * 
     */
    struct SnoozeOn : public SnoozeState {
        using SnoozeState::SnoozeState;
        void entry() 
        { 
            print("entry in SnoozeOn");
            stm.changedSnooze("ON"); 
        }
        void snooze( Snooze_t snooze ); // implemented in MyDFPlayer.cpp
        void exit() 
        { 
            print("leaving SnoozeOn");
        }
    };
    /**
     * @brief snooze of state
     * 
     */
    struct SnoozeOff : public SnoozeState {
        using SnoozeState::SnoozeState;
        void entry() 
        { 
            print("entry in SnoozeOff");
            stm.changedSnooze("OFF"); 
        }
        void snooze( Snooze_t snooze ); // implemented in MyDFPlayer.cpp
        void exit() 
        { 
            print("leaving SnoozeOff"); 
        }
    };

/**
 * @brief Statemachine for alarm
 * 
 */
private:
    /**
     * @brief generic alarm state
     * 
     */
    struct AlarmState : public GenericState<MyAudioPlayer, AlarmState> {
        using GenericState::GenericState;
        virtual void alarmOn() { unhandledEvent("alarm on"); }
        virtual void alarmOff() { unhandledEvent("alarm off"); }
        virtual void initSnooze() { unhandledEvent("initSnooze"); }
        virtual const char *getAlarmState(){ unhandledEvent( "AlaramState"); return "unhandledEvent"; }
        virtual void snooze(Snooze_t snooze) { print("AlaramState.snooze"); (void)snooze; unhandledEvent("snooze in AlarmSatet"); }
    };
    StateRef<AlarmState> alarmState;
    /**
     * @brief alarm on state
     * 
     */
    struct AlarmOn : public AlarmState {
        using AlarmState::AlarmState;
        void initSnooze()
        {
            print("initSnooze");
            SnoozeState::init<SnoozeOff>(stm, snoozeState);
        }
        void entry() 
        { 
            print("entering AlarmOn");
            /* start sound task */
            stm.resumeSoundLoopTask(); 
        }
        void alarmOn() 
        { 
            print("Alarm is already on"); 
        }
        void alarmOff() 
        {
            print("switch Alarm off"); 
            change<AlarmOff>();
        }
        void snooze() 
        {
            print("snoozeOn in AlarmOn");
        }
        const char *getAlarmState()
        {
            return "AlarmOn";
        }
        void exit() 
        { 
            print("leaving AlarmOn"); 
            stm.suspendSoundLoopTask();
        }
    private:
        // snooze is state of AlarmOn
        StateRef<SnoozeState> snoozeState;
    };
    /**
     * @brief alarm off state
     * 
     */
    struct AlarmOff : public AlarmState {
        using AlarmState::AlarmState;
        void entry() 
        { 
            print("entry AlarmOff");
            /* stop sound task */
        }
        void alarmOff() 
        { 
            print("Alarm is already off"); 
        }
        void alarmOn() 
        {
            print("switch Alarm on"); 
            change<AlarmOn>(); 
        }
        const char *getAlarmState()
        {
            return "AlarmOff";
        }
        void exit() 
        { 
            print("leaving AlarmOff"); 
        }
    };
};

/**
 * @brief switch snooze on
 * 
 * @param snooze_ 
 */
void MyAudioPlayer::SnoozeOn::snooze(MyAudioPlayer::Snooze_t snooze_)
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
void MyAudioPlayer::SnoozeOff::snooze(MyAudioPlayer::Snooze_t snooze_)
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

