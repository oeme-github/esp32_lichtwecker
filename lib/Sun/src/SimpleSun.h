

#include <esp32_digital_led_lib.h>
#include <esp32_digital_led_funcs.h>
#include <SimpleTimer.h> 
#include <genericstate.h>

#ifndef SUN_H
#define SUN_H

#define NUM_LEDS 28                      //number of LEDs in the strip
#define BRIGHTNESS 255                   //strip brightness 255 max
#define SUNSIZE 50                       //percentage of the strip that is the "sun"

#ifndef WAKE_DELAY
#define WAKE_DELAY 1800                  //sunrise span in sec
#endif

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define DEBUG_SERIAL_SUN_ENABLE
#define _WITH_TEST_LED_
// _DEBUG_SUN_DETAILS_

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

typedef enum {
    SUNRISE   = 1,
    LIGHT     = 11,
    SUNSET    = 2,
    DARK      = 22,
    SUNBLAULI = 3,
    SUNOFF    = 0
} sunmode_t;

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

    //int intSunriseDelay    = WAKE_DELAY;
    int sun                = (SUNSIZE * NUM_LEDS)/100;
    int aurora             = NUM_LEDS;

    sunmode_t mode         = SUNOFF; 
    
    int numTimer           = 0; 
    int blueLightCicles    = 0;

    uint32_t iOffsetSun    = 30;

    timer_callback ptrTimerCB;

    TaskHandle_t hTaskSunLoop;
    TaskFunction_t pvTaskCode;

    int calWhiteValue();

/* methodes */
public:
    SimpleSun(){};
    ~SimpleSun(){};
    
    void init(int iWhite, int iSunPhase, int iFadeStep, int iSunFadeStep, int iWakeDelay);
    bool init_ledDriver();
    
    void setNumLeds(int iLeds);
    int  getNumLeds();

    int getRc();
    sunmode_t getMode();
    bool setMode(sunmode_t newmode);

    int getSunPhase();

    void sunrise();
    void sunset();

    bool isLight();
    bool isDark();

    void setWakeDelay(unsigned int iDelay);
    unsigned int getWakeDelay();

    void delTimer();

    void setNumTimer( int iNumTimer );
    int  getNumTimer();

    void letSunRise( int intPayload_ = 0, bool bInit_ = false );

    void setTimerCB(timer_callback ptrTimerCB_ )
    {
        ptrTimerCB = ptrTimerCB_;
    }

    void setTaskFunction( TaskFunction_t pvTaskCode_ )
    {
        pvTaskCode = pvTaskCode_;
    }

    void startSunLoopTask();

    /* sun */
    void sunDown() 
    {  
        print("sunDown() - sunState->sunDown()");
        sunState->sunDown();  
    }
    void sunRise()
    {
        print("sunRise() - sunState->sunRise()");
        sunState->sunRise();  
    }
    void sunUp()
    { 
        print("sunUp() - sunState->sunUp()");
        sunState->sunUp();  
    }
    const char *getSunState()
    {
        return sunState->getSunState();
    }

    /* light */
    void lightOn()
    {
        print("lightOn()");  
        lightState->lightOn();  
    }
    void lightOff()
    { 
        print("lightOn()");
        lightState->lightOff();
    }
    void lightBlue()
    { 
        print("lightBlue()");
        lightState->lightBlue();
    }
    const char *getLightState()
    {
        return lightState->getLightState();
    }

private:
    void weislicht();
    void blaulicht();

    void calSun();
    void drawAmbient();
    void drawAurora();
    void drawSun();

    void increaseSunFadeStep();
    void decreaseSunFadeStep();

    void increaseFadeStep();
    void decreaseFadeStep();

    void increaseWhiteLevel();
    void decreaseWhiteLevel();

    void increaseSunPhase();
    void decreaseSunPhase();

    void resetPixels();
    void drawPixels();

    strand_t getStrand();

    void taskSunLoopCode( void * pvParameters );
    void stopSunLoopTask();

private:

    static void print(const std::string &str, bool bNewLine = true) {
        if( bNewLine )
            dbSunSerialPrintln(str.c_str());
        else
            dbSunSerialPrint(str.c_str());
    }
    static void print(int iNum, bool bNewLine = true) { 
        if( bNewLine )
            dbSunSerialPrintln(iNum);
        else
            dbSunSerialPrint(iNum);
    }
    static void unhandledEvent(const std::string &str) { print("unhandled event " + str); }

private:
    /* sun */
    struct SunState : public GenericState<SimpleSun, SunState> {
        using GenericState::GenericState;
        virtual void sunDown() { unhandledEvent("sunDown"); }
        virtual void sunRise() { unhandledEvent("sunRise"); }
        virtual void sunUp()   { unhandledEvent("sunUp"); }
        virtual const char *getSunState() { unhandledEvent("getLightState"); return "unhandledEvent";}
    };
    StateRef<SunState> sunState;

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

    struct SunRise : public SunState {
        using SunState::SunState;
        void entry() 
        { 
            print("entry SunRise");
            /* start sunrise */
            stm.startSunLoopTask();
            stm.letSunRise( stm.getWakeDelay()*60, true );
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
            print("exit SunRise");
            stm.stopSunLoopTask();
        }
    };

    struct SunUp : public SunState {
        using SunState::SunState;
        void entry() 
        { 
            print("entry SunUp");
            stm.lightOn();
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
    /* light */
    struct LightState : public GenericState<SimpleSun, LightState> {
        using GenericState::GenericState;
        virtual void lightOn() { unhandledEvent("light on"); }
        virtual void lightOff() { unhandledEvent("light off"); }
        virtual void lightBlue() { unhandledEvent("light blue"); }
        virtual const char *getLightState() { unhandledEvent("getLightState"); return "unhandledEvent";}        
    };
    StateRef<LightState> lightState;

    struct LightOn : public LightState {
        using LightState::LightState;
        void entry() 
        { 
            print("entry LightOn");
            stm.weislicht();
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

    struct LightOff : public LightState {
        using LightState::LightState;
        void entry() 
        { 
            print("entering LightOff"); 
            stm.resetPixels();
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
