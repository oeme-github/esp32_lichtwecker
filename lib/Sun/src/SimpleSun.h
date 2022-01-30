#include <esp32_digital_led_lib.h>
#include <esp32_digital_led_funcs.h>
#include <SimpleTimer.h> 
#include <genericstate.h>
#include <MDispatcher.h>

#ifndef SIMPLE_SUN_H
#define SIMPLE_SUN_H

#define NUM_LEDS 28                      //number of LEDs in the strip
#define BRIGHTNESS 255                   //strip brightness 255 max
#define SUNSIZE 50                       //percentage of the strip that is the "sun"

#ifndef WAKE_DELAY
#define WAKE_DELAY 1800                  //sunrise span in sec
#endif

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define DEBUG_SERIAL_SUN_ENABLE
#define _WITH_TEST_LED_
#define _DEBUG_SUN_DETAILS_

#ifdef DEBUG_SERIAL_SUN_ENABLE
    #define dbSunSerialPrint(a)    Serial.print(a)
	#define dbSunSerialPrintHex(a) Serial.print(a, HEX)
    #define dbSunSerialPrintln(a)  Serial.println(a)
    #define dbSunSerialBegin(a)    Serial.begin(a)
#else
    #define dbSunSerialPrint(a)    do{}while(0)
    #define dbSunSerialPrintHex(a) do{}while(0)		
    #define dbSunSerialPrintln(a)  do{}while(0)
    #define dbSunSerialBegin(a)    do{}while(0)
#endif


class SimpleSun : public SimpleTimer {
/* data */
private:
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"  // It's noisy here with `-Wall`
    strand_t strand = {.rmtChannel = 0, .gpioNum = 27, .ledType = LED_SK6812W_V1, .brightLimit = 100, .numPixels = NUM_LEDS};
    strand_t * STRANDS [1];
    int STRANDCNT = sizeof(STRANDS); 
    #pragma GCC diagnostic pop

    int rc                 = 0;

    int iLeds              = NUM_LEDS;
    uint32_t wakeDelay     = WAKE_DELAY;

    int sunFadeStep        = 100;
    int fadeStep           = 100;
    int whiteLevel         = 100;
    int sunPhase           = 100;

    int currentSun         = 100;
    int oldSun             = 0;
    
    int currentAurora      = 100;
    int oldAurora          = 0;

    int sun                = (SUNSIZE * NUM_LEDS)/100;
    int aurora             = NUM_LEDS-sun;

    int numTimer           = 0; 

    timer_callback ptrTimerCB;

    TaskHandle_t hTaskSunLoop;
    TaskFunction_t pvTaskCode;

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
     * @param iWhite 
     * @param iSunPhase 
     * @param iFadeStep 
     * @param iSunFadeStep 
     * @param iWakeDelay 
     */
    void init(int iWhite, int iSunPhase, int iFadeStep, int iSunFadeStep, int iWakeDelay);
    /**
     * @brief initialize led driver
     * 
     * @return true 
     * @return false 
     */
    bool init_ledDriver();
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
    int getSunPhase();
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
     * @param intPayload_ 
     * @param bInit_ 
     */
    void letSunRise( int intPayload_ = 0, bool bInit_ = false );
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
        print("sunDown() - sunState->sunDown()");
        sunState->sunDown();  
    }
    /**
     * @brief companion function sun rise state
     * 
     */
    void sunRise()
    {
        print("sunRise() - sunState->sunRise()");
        sunState->sunRise(); 
    }
    /**
     * @brief companion function sun up state
     * 
     */
    void sunUp()
    { 
        print("sunUp() - sunState->sunUp()");
        sunState->sunUp();  
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
        print("lightOn() -> lightState->lightOn()");  
        lightState->lightOn();  
    }
    /**
     * @brief light off companion
     * 
     */
    void lightOff()
    { 
        print("lightOff() -> lightState->lightOff()");
        lightState->lightOff();
    }
    /**
     * @brief light blue companion
     * 
     */
    void lightBlue()
    { 
        print("lightBlue()");
        lightState->lightBlue();
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
     * @brief switch on the lamp with blue light
     * 
     */
    void blaulicht();
    /**
     * @brief calculate the sun
     * 
     */
    void calSun();
    /**
     * @brief draw the ambiente during sunrise/sunset
     * 
     */
    void drawAmbient();
    /**
     * @brief draw aurora during sunrise/sunset
     * 
     */
    void drawAurora();
    /**
     * @brief draw sun
     * 
     */
    void drawSun();
    /**
     * @brief calculate white level
     * 
     * @return int 
     */
    int calWhiteValue();
    /**
     * @brief increase sun fade during sinrise
     * 
     */
    void increaseSunFadeStep();
    /**
     * @brief decrease sun fade during sunset
     * 
     */
    void decreaseSunFadeStep();
    /**
     * @brief increase fade during sunrise
     * 
     */
    void increaseFadeStep();
    /**
     * @brief decrease fade during sunset
     * 
     */
    void decreaseFadeStep();
    /**
     * @brief increase white level during sunrise
     * 
     */
    void increaseWhiteLevel();
    /**
     * @brief decrease white level during sunset
     * 
     */
    void decreaseWhiteLevel();
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
     * @brief print function for string
     * 
     * @param str 
     * @param bNewLine 
     */
    static void print(const std::string &str, bool bNewLine = true) {
        if( bNewLine )
            dbSunSerialPrintln(str.c_str());
        else
            dbSunSerialPrint(str.c_str());
    }
    /**
     * @brief print function for int
     * 
     * @param iNum 
     * @param bNewLine 
     */
    static void print(int iNum, bool bNewLine = true) { 
        if( bNewLine )
            dbSunSerialPrintln(iNum);
        else
            dbSunSerialPrint(iNum);
    }
    /**
     * @brief default unhandled event function
     * 
     * @param str 
     */
    static void unhandledEvent(const std::string &str) { print("unhandled event " + str); }


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
            print("entry SunDown (switch light off)");
            stm.lightOff();
        }
        void sunDown() 
        { 
            print("sun is already down");
        }
        void sunRise() 
        { 
            print("sun is down and will rise");
            change<SunRise>();
        }
        void sunUp()
        {
            print("sun is down and will be up");
            change<SunUp>();
        }
        const char *getSunState() 
        { 
            return "SunDown";
        }
        void exit() 
        { 
            print("exit SunDown"); 
        }
    };
    /**
     * @brief sun rise state
     * 
     */
    struct SunRise : public SunState {
        using SunState::SunState;
        void entry() 
        { 
            /* start sunrise */
            print( "entry SunRise" );
            print( "-> resume sun loop task..." );
            vTaskResume(stm.hTaskSunLoop);
            vTaskDelay(50/portTICK_PERIOD_MS);
            print( "-> letSunRise..." );
            stm.letSunRise( stm.getWakeDelay()*60, true );
            print("entry SunRise done.");
        }
        void sunDown() 
        { 
            print("sun is rising, can not switch to SunDown");
        }
        void sunRise() 
        { 
            print("sun is already rising");
        }
        void sunUp()
        {
            print("sun is rising, switch to SunUp");
            change<SunUp>();
        }
        const char *getSunState() 
        { 
            return "SunRise";
        }
        void exit() 
        { 
            print("exit SunRise (stop sun loop task)."); 
            // print("-> stop sun loop task");
            // stm.stopSunLoopTask();
            print( "exit SunRise done.");
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
            print("entry SunUp");
            print("-> switch light on");
            stm.lightOn();
            print("entry SunUp done.");
        }
        void sunDown() 
        { 
            print("sun is up, switch to SunDown");
            change<SunDown>();
        }
        void sunRise() 
        { 
            print("sun is up, can not switch to SunRise");
        }
        void sunUp()
        {
            print("sun is already up");
        }
        const char *getSunState() 
        { 
            return "SunUp";
        }
        void exit() 
        { 
            print("exit SunUp"); 
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
        virtual void lightBlue() { unhandledEvent("light blue"); }
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
            print("entry LightOn");
            stm.weislicht();
            print("entry LightOn done.");
        }
        void lightOn() 
        { 
            print("light is already on"); 
        }
        void lightOff() 
        {
            print("switch light off"); 
            change<LightOff>(); 
        }
        void lightBlue() 
        { 
            print("switch light to blue");
            change<LightBlue>();
        }
        const char *getLightState() 
        { 
            return "LightOn"; 
        }
        void exit() 
        { 
            print("leaving LightOn"); 
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
            print("entering LightOff");
            print("-> reset pixels...");
            stm.resetPixels();
            print("entry LightOff done.");
        }
        void lightOff() 
        { 
            print("light is already off"); 
        }
        void lightOn() 
        {
            print("switch light on"); 
            change<LightOn>(); 
        }
        void lightBlue() 
        { 
            print("switch light to blue");
            change<LightBlue>();
        }
        const char *getLightState() 
        { 
            return "LightOff"; 
        }
        void exit() 
        { 
            print("exit LightOff"); 
        }
    };
    /**
     * @brief light blue state
     * 
     */
    struct LightBlue : public LightState {
        using LightState::LightState;
        void entry() 
        { 
            print("entering LightBlue"); 
            stm.blaulicht();
        }
        void lightOff() 
        { 
            print("switch light off");
            change<LightOff>(); 
        }
        void lightOn() 
        {
            print("switch light on"); 
            change<LightOn>(); 
        }
        void lightBlue() 
        { 
            print("light is already blue"); 
        }
        const char *getLightState() 
        { 
            return "LightBlue"; 
        }
        void exit() 
        { 
            print("leaving LightOff"); 
        }
    };
};


#endif
