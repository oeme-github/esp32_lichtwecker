#pragma once

#include <esp32_digital_led_lib.h>
#include <esp32_digital_led_funcs.h>
#include <SimpleTimer.h> 
#include <genericstate.h>
#include <MDispatcher.h>
#include <libDebug.h>

#include <SimpleSunConfig.h>

#include <MyQueueHandler.h>

#ifndef SIMPLE_SUN_H
#define SIMPLE_SUN_H

class SimpleSun : public SimpleTimer {
/* data */
private:
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"  // It's noisy here with `-Wall`
    strand_t strand = {.rmtChannel = 0, .gpioNum = 27, .ledType = LED_SK6812W_V1, .brightLimit = 100, .numPixels = NUM_LEDS};
    strand_t * STRANDS [1];
    int STRANDCNT = sizeof(STRANDS); 
    #pragma GCC diagnostic pop

    int rc                   = 0;
    int iLeds                = NUM_LEDS;
    int iWakeDelay           = WAKE_DELAY;
    unsigned long timerDelay = iWakeDelay * 60 * 1000/SUN_PHASE;
    int sunPhase             = 0;
    int numTimer             = 0;
    int iBrightness          = 8;

    timer_callback ptrTimerCB;
    TaskHandle_t hTaskSunLoop;
    TaskFunction_t pvTaskCode;

    u_int16_t iRed   = 0;
    u_int16_t iGreen = 0;
    u_int16_t iBlue  = 0;
    u_int16_t iWhite = 0;

    MyQueueHandler myQueueHandler;

/* methodes */
public:
    /**
     * @brief Construct a new Simple Sun object
     * 
     */
    SimpleSun(){};
    /**
     * @brief Destroy the Simple Sun object
     * 
     */
    ~SimpleSun(){};
    /**
     * @brief initialize parameter of sun
     * 
     */
    void init();
    /**
     * @brief initialize led driver
     * 
     * @return true 
     * @return false 
     */
    bool init_ledDriver();

    void setQueue(xQueueHandle hQueue_){this->myQueueHandler.setQueue(hQueue_);}
    void sendToQueue(std::string msg_){this->myQueueHandler.sendToQueue(msg_);}
    int getMapValue( int _level);

    /**
     * @brief Set the Num Leds object
     * 
     * @param iLeds 
     */
    void setNumLeds(int iLeds);
    /**
     * @brief Get the Num Leds object
     * 
     * @return int 
     */
    int  getNumLeds();
    /**
     * @brief Set the Red object
     * 
     * @param iRet_ 
     */
    void setRed(u_int16_t iRet_){ this->iRed=iRet_;}
    /**
     * @brief Set the Green object
     * 
     * @param iGreen_ 
     */
    void setGreen(u_int16_t iGreen_){ this->iGreen=iGreen_;}
    /**
     * @brief Set the Blue object
     * 
     * @param iBlue_ 
     */
    void setBlue(u_int16_t iBlue_){ this->iBlue=iBlue_;}
    /**
     * @brief Set the White object
     * 
     * @param iWhite_ 
     */
    void setWhite(u_int16_t iWhite_){ this->iWhite=iWhite_;}
    /**
     * @brief Get the Rc object
     * 
     * @return int 
     */
    int getRc();
    /**
     * @brief Get the Sun Phase object
     * 
     * @return int 
     */
    boolean getSunPhase();
    /**
     * @brief Set the Wake Delay object
     * 
     * @param iDelay 
     */
    void setWakeDelay(unsigned int iDelay);
    /**
     * @brief Get the Wake Delay object
     * 
     * @return unsigned int 
     */
    unsigned int getWakeDelay();
    /**
     * @brief delete timer
     * 
     */
    void delTimer();
    /**
     * @brief Set the Num Timer object
     * 
     * @param iNumTimer 
     */
    void setNumTimer( int iNumTimer );
    /**
     * @brief Get the Num Timer object
     * 
     * @return int 
     */
    int  getNumTimer();
    /**
     * @brief Set the Timer C B object
     * 
     * @param ptrTimerCB_ 
     */
    void setTimerCB(timer_callback ptrTimerCB_ )
    {
        ptrTimerCB = ptrTimerCB_;
    }
    /**
     * @brief Set the Brightness object
     * 
     * @param iBrightness_ 
     */
    void setBrightness(int iBrightness_){ this->iBrightness=iBrightness_; }
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
     * @brief let the sun rise
     * 
     * @param bInit_ 
     */
    void letSunRise( bool bInit_ = false );
    /**
     * @brief start the sun loop task
     * 
     */
    void startSunLoopTask();
    /**
     * @brief register to dispatcher
     * 
     * @param dispatcher 
     */
    void registerCB(MDispatcher<String, EventEnum> &dispatcher) 
    {
        using namespace std::placeholders;
        dispatcher.addCB(std::bind(&SimpleSun::listener, this, _1, _2));
    }    
    /* -------------------------------------------------- */
    /* sun state functions                                */
    /**
     * @brief companion function sun down state
     * 
     */
    void sunDown() 
    {  
        this->sendToQueue( "sunDown Phase[" + std::to_string(this->sunPhase) +"] (will be set to SUN_PHASE)");
        sunPhase = 0;
        sunState->sunDown();
        this->deleteTimer(this->numTimer);
        this->numTimer = -1;
    }
    /**
     * @brief companion function sun rise state
     * 
     */
    void sunRise()
    {
        sunState->sunRise(); 
    }
    /**
     * @brief companion function sun up state
     * 
     */
    void sunUp()
    { 
        this->sendToQueue( "sunUp Phase[" + std::to_string(this->sunPhase) +"] (will be set to SUN_PHASE)");
        sunPhase = SUN_PHASE;
        sunState->sunUp();  
        this->deleteTimer(this->numTimer);
        this->numTimer = -1;
    }
    /**
     * @brief Get the Sun State object
     * 
     * @return const char* 
     */
    const char *getSunState()
    {
        return sunState->getSunState();
    }
    /* -------------------------------------------------- */
    /* light state function                               */
    /**
     * @brief light on companion 
     * 
     */
    void lightOn()
    {
        lightState->lightOn();  
    }
    /**
     * @brief light off companion
     * 
     */
    void lightOff()
    { 
        lightState->lightOff();
    }
    /**
     * @brief light blue companion
     * 
     */
    void lightRGB()
    { 
        lightState->lightRGB();
    }
    /**
     * @brief Get the Light State object
     * 
     * @return const char* 
     */
    const char *getLightState()
    {
        return lightState->getLightState();
    }

private:
    /**
     * @brief sunrise function
     * 
     */
    void sunrise();
    /**
     * @brief sunset function
     * 
     */
    void sunset();
    /**
     * @brief switch on the lamp with wight light 
     * 
     */
    void weislicht();
    /**
     * @brief switch on the lamp with rgbw 
     * 
     */
    void rgbwlicht();
    /**
     * @brief display RGBW light
     * 
     * @param iRed 
     * @param iGreen 
     * @param iBlue 
     * @param iWhite 
     */
    void rgbwLicht(uint16_t iRed, uint16_t iGreen, uint16_t iBlue, uint16_t iWhite);
    /**
     * @brief draw sun
     * 
     */
    void drawSun();
    /**
     * @brief increase sunphase during sunrise
     * 
     */
    void increaseSunPhase();
    /**
     * @brief decrease sunphase during sunset
     * 
     */
    void decreaseSunPhase();
    /**
     * @brief Set the Sun Phase object
     * 
     * @param iSunPahse_ 
     */
    void setSunPhase(int8_t iSunPahse_ ){ this->sunPhase = iSunPahse_; }
    /**
     * @brief reset pixels of sun (leds)
     * 
     */
    void resetPixels();
    /**
     * @brief draw pixels
     * 
     */
    void drawPixels();
    /**
     * @brief Get the Strand object
     * 
     * @return strand_t 
     */
    strand_t getStrand();
    /**
     * @brief 
     * 
     * @param pvParameters 
     */
    void taskSunLoopCode( void * pvParameters );
    /**
     * @brief stop the sunloop task
     * 
     */
    void stopSunLoopTask();
    /**
     * @brief listener function for receiving message from dispatcher
     * 
     * @param string_ 
     * @param event_ 
     */
    void listener(String string_, EventEnum event_);

private:
    /**
     * @brief default unhandled event function
     * 
     * @param str 
     */
    static void unhandledEvent(const std::string &str) { dbSerialPrintf("unhandled event %s", str); }


private:
    /* -------------------------------------------------- */
    /* sun states                                         */
    /**
     * @brief generic sun state
     * 
     */
    struct SunState : public GenericState<SimpleSun, SunState> {
        using GenericState::GenericState;
        virtual void sunDown() { unhandledEvent("sunDown"); }
        virtual void sunRise() { unhandledEvent("sunRise"); }
        virtual void sunUp()   { unhandledEvent("sunUp"); }
        virtual const char *getSunState() { unhandledEvent("getLightState"); return "unhandledEvent";}
    };
    StateRef<SunState> sunState;
    /**
     * @brief sun down state
     * 
     */
    struct SunDown : public SunState {
        using SunState::SunState;
        void entry() 
        { 
            stm.lightOff();
        }
        void sunDown() 
        { 
            return;
        }
        void sunRise() 
        { 
            change<SunRise>();
        }
        void sunUp()
        {
            change<SunUp>();
        }
        const char *getSunState() 
        { 
            return "SunDown";
        }
        void exit() 
        { 
            return; 
        }
    };
    /**
     * @brief sun rise state
     * 
     */
    struct SunRise : public SunState 
    {
        using SunState::SunState;
        void entry() 
        { 
            /* start sunrise */
            vTaskResume(stm.hTaskSunLoop);
            vTaskDelay(10/portTICK_PERIOD_MS);
            /* get wake delay in minutes and start sunrise with seconds */
            stm.sendToQueue("SunState:SunRise.entry -> call letSunRise(true)");
            stm.letSunRise( true );
        }
        void sunDown() 
        { 
            return;
        }
        void sunRise() 
        { 
            return;
        }
        void sunUp()
        {
            change<SunUp>();
        }
        const char *getSunState() 
        { 
            return "SunRise";
        }
        void exit() 
        { 
            return;
        }
    };
    /**
     * @brief sun up state
     * 
     */
    struct SunUp : public SunState {
        using SunState::SunState;
        void entry() 
        {
            stm.lightOn();
        }
        void sunDown() 
        { 
            change<SunDown>();
        }
        void sunRise() 
        { 
            return;
        }
        void sunUp()
        {
            return;
        }
        const char *getSunState() 
        { 
            return "SunUp";
        }
        void exit() 
        { 
            return;
        }
    };

private:
    /* -------------------------------------------------- */
    /* light states                                       */
    /**
     * @brief generic light state
     * 
     */
    struct LightState : public GenericState<SimpleSun, LightState> {
        using GenericState::GenericState;
        virtual void lightOn() { unhandledEvent("light on"); }
        virtual void lightOff() { unhandledEvent("light off"); }
        virtual void lightRGB() { unhandledEvent("light RGB"); }
        virtual const char *getLightState() { unhandledEvent("getLightState"); return "unhandledEvent";}        
    };
    StateRef<LightState> lightState;
    /**
     * @brief light on state
     * 
     */
    struct LightOn : public LightState {
        using LightState::LightState;
        void entry() 
        { 
            stm.weislicht();
        }
        void lightOn() 
        { 
            return;
        }
        void lightOff() 
        {
            change<LightOff>(); 
        }
        void lightRGB() 
        {
            change<LightRGB>(); 
        }
        const char *getLightState() 
        { 
            return "LightOn"; 
        }
        void exit() 
        { 
            return;
        }
    };
    /**
     * @brief light off state
     * 
     */
    struct LightOff : public LightState {
        using LightState::LightState;
        void entry() 
        { 
            stm.resetPixels();
        }
        void lightOff() 
        { 
            return;
        }
        void lightOn() 
        {
            change<LightOn>(); 
        }
        void lightRGB() 
        {
            change<LightRGB>(); 
        }
        const char *getLightState() 
        { 
            return "LightOff"; 
        }
        void exit() 
        { 
            return;
        }
    };
    /**
     * @brief light RGB state
     * 
     */
    struct LightRGB : public LightState {
        using LightState::LightState;
        void entry() 
        { 
            stm.rgbwlicht();
        }
        void lightOff() 
        { 
            change<LightOff>(); 
        }
        void lightOn() 
        {
            change<LightOn>(); 
        }
        void lightRGB() 
        {
            return; 
        }
        const char *getLightState() 
        { 
            return "LightRGB"; 
        }
        void exit() 
        { 
        }
    };
};


#endif
