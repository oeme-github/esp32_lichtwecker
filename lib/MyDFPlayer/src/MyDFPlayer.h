#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Nextion.h>
#include <genericstate.h>


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

    TaskHandle_t hTaskSoundLoop;
    TaskFunction_t pvTaskCode;

private:
    static void print(const std::string &str, bool bNewLine = true) {
        if( bNewLine )
            dbSerialPrintln(str.c_str());
        else
            dbSerialPrint(str.c_str());
    }
    static void print(int iNum, bool bNewLine = false) { 
        if( bNewLine )
            dbSerialPrintln(iNum);
        else
            dbSerialPrintln(iNum);
    }
    static void unhandledEvent(const std::string &str) { print("unhandled event " + str); }

public:
    // MyDFPlayer();
    MyDFPlayer(int8_t rx_=SWSERIAL_RX, int8_t tx_=SWSERIAL_TX );
    ~MyDFPlayer();

    const char *getDetail(uint8_t type, int value);
    
    SoftwareSerial *getSoftwareSerial(){return &softwareSerial;}

    bool _begin_(){ return begin(softwareSerial,false,true); }

    void startDFPlayer();

    void startSunLoopTask();
    void stopSoundLoopTask();

    void setTaskFunction( TaskFunction_t pvTaskCode_ )
    {
        pvTaskCode = pvTaskCode_;
    }

    void alaramOff()
    { 
        alarmState->alarmOff(); 
    }
    void alaramOn()
    { 
        alarmState->alarmOn(); 
    }

    void snoozeInit()
    { 
        alarmState->initSnooze(); 
    }
    void snoozeOff()
    { 
        alarmState->snoozeOff(); 
    }
    void snoozeOn()
    { 
        alarmState->snoozeOn(); 
    }


/* states */
private:
    /* snooze */
    struct SnoozeState : public GenericState<MyDFPlayer, SnoozeState> {
        using GenericState::GenericState;
        virtual void snoozeOn(int iTime) { (void)iTime; unhandledEvent("SnoozeState"); }
        virtual void snoozeOff() { unhandledEvent("SnoozeState"); }
    };

    struct SnoozeOn : public SnoozeState {
        using SnoozeState::SnoozeState;
        void entry() 
        { 
            print("entry in SnoozeOn");
            vTaskSuspend(stm.hTaskSoundLoop); 
        }
        void snoozeOn(int iTime_) 
        {
            print("snooze already on");
        }
        void snoozeOff()
        {
            print("snoozeOff called");
            change<SnoozeOff>();
        }
        void exit() 
        { 
            print("leaving SnoozeOn");
        }
    };

    struct SnoozeOff : public SnoozeState {
        using SnoozeState::SnoozeState;
        void entry() 
        { 
            print("entry in SnoozeOff");
            vTaskResume(stm.hTaskSoundLoop);
        }
        void snoozeOn(int iTime_) 
        {
            print("switch snooze on");
            change<SnoozeOn>();
        }
        void snoozeOff()
        {
            print("snooze already off");
        }
        void exit() 
        { 
            print("leaving SnoozeOff"); 
        }
    };

private:
    /* alarm */
    struct AlarmState : public GenericState<MyDFPlayer, AlarmState> {
        using GenericState::GenericState;
        virtual void alarmOn() { unhandledEvent("alarm on"); }
        virtual void alarmOff() { unhandledEvent("alarm off"); }
        virtual void snoozeOn() { unhandledEvent("snooze on"); }
        virtual void snoozeOff() { unhandledEvent("snooze on"); }
        virtual void initSnooze() { unhandledEvent("initSnooze"); }
    };
    StateRef<AlarmState> alarmState;

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
            stm.startSunLoopTask(); 
        }
        void alarmOn() 
        { 
            print("Alarm is already on"); 
        }
        void alarmOff() 
        {
            print("switch Alarm off"); 
            change<AlarmOff>();
            /* stop sound task */
            stm.stopSoundLoopTask();
        }
        void snozzeOn() 
        {
            // snooze for 5 minutes
            snoozeState->snoozeOn(5);
        }
        void snozzeOff() 
        {
            // snooze for 5 minutes
            snoozeState->snoozeOff();
        }
        void exit() 
        { 
            print("leaving AlarmOff"); 
        }
    private:
        // snooze is state of AlarmOn
        StateRef<SnoozeState> snoozeState;
    };

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
        void exit() 
        { 
            print("leaving AlarmOff"); 
        }
    };

};

#endif