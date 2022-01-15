#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Nextion.h>
#include <genericstate.h>

#include <MDispatcher.h>

#ifndef _MYDFPLAYER_H_
#define _MYDFPLAYER_H_


#ifndef SWSERIAL_RX
#define SWSERIAL_RX 18
#endif

#ifndef SWSERIAL_TX
#define SWSERIAL_TX 19
#endif

#ifndef GPIO_MP3
#define GPIO_MP3 34
#endif


// uncomment if you like to have sound test on startup
//#define _WITH_SOUND_TEST_

class MyDFPlayer : public DFRobotDFPlayerMini
{
private:
    /* data */
    SoftwareSerial softwareSerial;

    TaskHandle_t   hTaskSoundLoop;
    TaskFunction_t pvTaskCode;

    uint32_t iVolume = 20;

public:
    enum Snooze_t
    {
        SNOOZE_ON,
        SNOOZE_OFF
    };

public:
    /**
     * @brief Construct a new My D F Player object
     * 
     * @param rx_ 
     * @param tx_ 
     */
    MyDFPlayer(int8_t rx_=SWSERIAL_RX, int8_t tx_=SWSERIAL_TX );
    /**
     * @brief Destroy the My D F Player object
     * 
     */
    ~MyDFPlayer();
    /**
     * @brief Get the Detail object
     * 
     * @param type 
     * @param value 
     * @return const char* 
     */
    const char *getDetail(uint8_t type, int value);
    /**
     * @brief Get the Software Serial object
     * 
     * @return SoftwareSerial* 
     */
    SoftwareSerial *getSoftwareSerial(){return &softwareSerial;}
    /**
     * @brief wrapper for SoftwareSerial.beginn()
     * 
     * @return true 
     * @return false 
     */
    bool _begin_(){ return begin(softwareSerial,false,true); }
    /**
     * @brief startup sequence
     * 
     */
    void startDFPlayer();
    /**
     * @brief start task for audio processing
     * 
     */
    void startSoundLoopTask();
    /**
     * @brief stop task for audio processing
     * 
     */
    void stopSoundLoopTask();
    /**
     * @brief resume task for audio processing
     * 
     */
    void resumeSoundLoopTask();
    /**
     * @brief register receiving function to MDispatcher
     * 
     * @param dispatcher 
     */
    void registerCB(MDispatcher<String, EventEnum> &dispatcher) {
        using namespace std::placeholders;
        dispatcher.addCB(std::bind(&MyDFPlayer::listener, this, _1, _2));
    }    
    /**
     * @brief Set the Task Function object
     * 
     * @param pvTaskCode_ 
     */
    void setTaskFunction( TaskFunction_t pvTaskCode_ )
    {
        pvTaskCode = pvTaskCode_;
    }
    /**
     * @brief Set the Volume object
     * 
     * @param iVolume_ 
     */
    void setVolume( uint32_t iVolume_ ){ this->iVolume = iVolume_;}
    /**
     * @brief Get the Volume object
     * 
     * @return uint32_t 
     */
    uint32_t getVolume(){ return this->iVolume; }
    /**
     * @brief switch off alarm
     * 
     */
    void alaramOff()
    { 
        alarmState->alarmOff(); 
    }
    /**
     * @brief switch on alarm
     * 
     */
    void alaramOn()
    { 
        alarmState->alarmOn(); 
    }
    /**
     * @brief initialize snoozeState 
     * 
     */
    void snoozeInit()
    { 
        alarmState->initSnooze(); 
    }
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
    const char *getAlarmState()
    {
        return alarmState->getAlarmState();
    }

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
     * @brief listerner function for MDispatcher
     * 
     * @param string_ 
     * @param event_ 
     */
    void listener(String string_, EventEnum event_);

/* states */
private:
    /**
     * @brief generic snooze state
     * 
     */
    struct SnoozeState : public GenericState<MyDFPlayer, SnoozeState> {
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

private:
    /**
     * @brief generic alarm state
     * 
     */
    struct AlarmState : public GenericState<MyDFPlayer, AlarmState> {
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
            /* stop sound task */
            stm.stopSoundLoopTask();
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

#endif