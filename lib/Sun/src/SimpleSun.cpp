#include <SimpleTimer.h>
#include <SimpleSun.h>


/**
 * @brief init function, set the start values
 * 
 * @param iWakeDelay 
 */
void SimpleSun::init( int iWakeDelay)
{
    uint16_t iWake  = iWakeDelay*1000/SUN_PHASE;
    uint16_t iMap   = map(iWakeDelay, 60, 1800, 3, 15);
    float_t  fMap   = 1.0/(float_t)iMap;
    uint16_t iDelta = round((float_t)iWakeDelay*fMap);

    this->wakeDelay = iWake - iDelta;
}
/**
 * @brief initialzes the led driver
 * 
 * @return true 
 * @return false 
 */
bool SimpleSun::init_ledDriver()
{
    dbSunSerialPrintln("init_ledDriver()");
    /* -------------------------------------------------- */
    /* initialize led driver and strands                  */
    digitalLeds_initDriver();
    this->STRANDS[0]  = { &strand };
    this->STRANDCNT   = COUNT_OF(STRANDS);
    /* -------------------------------------------------- */
    /* Init unused outputs low to reduce noise            */
    gpioSetup(14, OUTPUT, LOW);
    gpioSetup(15, OUTPUT, LOW);
    gpioSetup(26, OUTPUT, LOW);
    gpioSetup(27, OUTPUT, LOW);
    gpioSetup(this->getStrand().gpioNum, OUTPUT, LOW);
    this->rc = digitalLeds_addStrands(this->STRANDS, this->STRANDCNT);
    dbSunSerialPrint( "LED-RC:"); dbSunSerialPrintln(this->rc);
    if(digitalLeds_initDriver() == 0) {
#ifdef _WITH_TEST_LED_
            weislicht();
            vTaskDelay(2000/portTICK_PERIOD_MS);
            resetPixels();
            vTaskDelay(100/portTICK_PERIOD_MS);
#endif
        /* ---------------------------------------------- */
        /* init states                                    */
        LightState::init<LightOff>(*this, lightState);
        SunState::init<SunDown>(*this, sunState);
        /* ---------------------------------------------- */
        /* return                                         */
        return true;
    }
    /* -------------------------------------------------- */
    /* default return                                     */
    dbSunSerialPrint( "LED-ERROR: something went wrong.");
    return false;
}
/**
 * @brief rest all pixels of the sun
 * 
 */
void SimpleSun::resetPixels()
{
    digitalLeds_resetPixels(this->STRANDS, this->STRANDCNT);
    this->drawPixels();
    this->sunPhase=0;
}
/**
 * @brief draw all pixels of th sun
 * 
 */
void SimpleSun::drawPixels()
{
#ifdef SHOW_PIXELS_VALUES
    int i = 0;
    for( i=0; i < STRANDS[0]->numPixels ; i++ )
    {
      dbSunSerialPrint("pixels[");
      dbSunSerialPrint( i );
      dbSunSerialPrint("] (r/g/b/w):");
      dbSunSerialPrint( STRANDS[0]->pixels[i].r );
      dbSunSerialPrint("/");
      dbSunSerialPrint( STRANDS[0]->pixels[i].g );
      dbSunSerialPrint("/");
      dbSunSerialPrint( STRANDS[0]->pixels[i].b );
      dbSunSerialPrint("/");
      dbSunSerialPrintln( STRANDS[0]->pixels[i].w );
    }
#endif

    digitalLeds_drawPixels(this->STRANDS, this->STRANDCNT);
}
/**
 * @brief function to calculate the sun leds (red, white)
 * 
 */
void SimpleSun::drawSun()
{
    /*
        Start with red  -> iRet bis 255, iGreen bis 70
        add green       -> iGreen bis 240
        and finaly blue -> iBlue bis 210 
        iWhite          -> 0 - 255 */
    print("SimpleSun::drawSun(): sunPhase:", false); print( this->sunPhase );
    strand_t* strip = this->STRANDS[0];

    int iWhite = 0;
    int iRed   = 0;
    int iGreen = 0;
    int iBlue  = 0;
    int iMod1  = this->sunPhase%this->getNumLeds();
    if(iMod1==0) iMod1=this->getNumLeds();

    for(int i = 0; i < iMod1; i++)
    {
        if( iMod1 == 1 )
        {
//            iRed   = RED_LEVEL*this->sunPhase/SUN_PHASE;
//            iGreen = GREEN_LEVEL*this->sunPhase/SUN_PHASE;
//            iBlue  = BLUE_LEVEL*this->sunPhase/SUN_PHASE;
//            iWhite = WHITE_LEVEL*this->sunPhase/SUN_PHASE;
            iRed   = map( this->sunPhase, 0, SUN_PHASE, 0, RED_LEVEL);
            iGreen = map( this->sunPhase, 0, SUN_PHASE, 0, GREEN_LEVEL);
            iBlue  = map( this->sunPhase, 0, SUN_PHASE, 0, BLUE_LEVEL);
            iWhite = map( this->sunPhase, 0, SUN_PHASE, 0, WHITE_LEVEL);
        }
        else{
            /* set all led step by step to the new values */
            iRed   = strip->pixels[0].r;
            iGreen = strip->pixels[0].g;
            iBlue  = strip->pixels[0].b;
            iWhite = strip->pixels[0].w;
        }
        strip->pixels[i] = pixelFromRGBW( iRed, iGreen, iBlue, iWhite);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}
/**
 * @brief increases sunPhase, ambiente and aurora
 * 
 */
void SimpleSun::sunrise()
{
    this->increaseSunPhase();
    this->drawSun();
    this->drawPixels();
}
/**
 * @brief decreases sunPhase, ambiente and aurora
 * 
 */
void SimpleSun::sunset()
{
    this->decreaseSunPhase();
    this->drawSun();
    this->drawPixels();
}
/**
 * @brief increase value of sunPhase and call increase functions
 * 
 */
void SimpleSun::increaseSunPhase()
{
    if (this->sunPhase < SUN_PHASE)
    {
        this->sunPhase++;
    }
}
/**
 * @brief decrease value of sunPhase and call decrease functions
 * 
 */
void SimpleSun::decreaseSunPhase()
{
    if (this->sunPhase > 0)
    {
        this->sunPhase--;
    }
}
/**
 * @brief set the total number of leds 
 * 
 * @param iLeds 
 */
void SimpleSun::setNumLeds(int iLeds)
{
    this->iLeds = iLeds;
};
/**
 * @brief get the number of leds
 * 
 * @return int 
 */
int SimpleSun::getNumLeds()
{
    return this->iLeds;
};
/**
 * @brief set the value of wakeDelay (default 1000)
 * 
 * @param iDelay 
 */
void SimpleSun::setWakeDelay(unsigned int iDelay)
{
    this->wakeDelay = iDelay;
}
/**
 * @brief return the value of wakeDelay
 * 
 * @return unsigned int 
 */
unsigned int SimpleSun::getWakeDelay()
{
    return this->wakeDelay;
}
/**drawSun
 * @brief return the led starnd
 * 
 * @return strand_t 
 */
strand_t SimpleSun::getStrand()
{
    return this->strand;
}
/**
 * @brief returns the return code (led lib)
 * 
 * @return int 
 */
int SimpleSun::getRc()
{
    return this->rc;
}
/**
 * @brief returns the sun phase
 * 
 * @return int 
 */
boolean SimpleSun::getSunPhase()
{
    if( this->sunPhase < SUN_PHASE )
        return true; 
    else
        return false;
}
/**
 * @brief set all leds to blue
 * 
 */
void SimpleSun::blaulicht()
{
    rgbwLicht(0, 0, 255, 0);
}
/**
 * @brief set all leds to blue
 * 
 */
void SimpleSun::rgbwlicht()
{
    rgbwLicht(this->iRed, this->iGreen, this->iBlue, this->iWhite);
    vTaskDelay(5000/portTICK_PERIOD_MS);
    this->lightOff();
}
/**
 * @brief set all leds to blue
 * 
 */
void SimpleSun::weislicht()
{
    rgbwLicht( map(this->iBrightness, 0, 10, 0, BRIGHTNESS)
             , map(this->iBrightness, 0, 10, 0, BRIGHTNESS)
             , map(this->iBrightness, 0, 10, 0, BRIGHTNESS)
             , map(this->iBrightness, 0, 10, 0, BRIGHTNESS));
}
/**
 * @brief set all leds to blue
 * 
 */
void SimpleSun::rgbwLicht(uint16_t iRed, uint16_t iGreen, uint16_t iBlue, uint16_t iWhite)
{
    strand_t* strip = this->STRANDS[0];
    for(int i = 0; i < this->getNumLeds(); i++)
    {
        /* -------------------------------------------------- */
        /* map(long x, long in_min, long in_max, long out_min, long out_max) */
        /*                                  r,   g,   b, w    */
        strip->pixels[i] = pixelFromRGBW( iRed
                                        , iGreen
                                        , iBlue
                                        , iWhite ); 
    }
    this->drawPixels();
}
/**
 * @brief deletes a timer
 * 
 */
void SimpleSun::delTimer()
{
    this->deleteTimer(this->numTimer);
}
/**
 * @brief stores the id of a timer in numTimer
 * 
 * @param iTimer 
 */
void SimpleSun::setNumTimer( int iTimer )
{
    this->numTimer = iTimer;
}
/**
 * @brief returns the timer id
 * 
 * @return int 
 */
int SimpleSun::getNumTimer()
{
    return this->numTimer;
}

/**
 * @brief function to start sunrise
 * 
 * @param intPayload 
 */
void SimpleSun::letSunRise( int intWakeDelay_, bool bInit_ )
{
    if( bInit_ )
    {
        /* ----------------------------------------------- */
        /* set start parameters                            */
        this->init(intWakeDelay_);
    }
    /* -------------------------------------------------- */
    /* rise the sun                                       */
    this->sunrise();
    /* -------------------------------------------------- */
    /* start timer                                        */
    if( this->getSunPhase() )
    {
        /* ----------------------------------------------- */
        /* continue sunrise                                */
        this->setNumTimer(this->setTimeout( this->getWakeDelay(), this->ptrTimerCB ));
    }
    else
    {
        /* ----------------------------------------------- */
        /* sun is risen                                    */
        print("-> Sun is up!");
        this->sunUp();
    }    
}

/**
 * @brief create sun loop task
 * 
 */
void SimpleSun::startSunLoopTask()
{
    print("SimpleSun::startSunLoopTask()");
    xTaskCreatePinnedToCore(
                    this->pvTaskCode,       /* Task function. */
                    "TaskSunLoop",          /* name of task. */
                    10000,                  /* Stack size of task */
                    NULL,                   /* parameter of the task */
                    1,                      /* priority of the task */
                    &this->hTaskSunLoop,    /* Task handle to keep track of created task */
                    1                       /* pin task to core 1 */
    );    
    print("suspend the task");
    vTaskSuspend(this->hTaskSunLoop);
    print("startSunLoopTask done.");
}

/**
 * @brief delete sun loop task
 * 
 */
void SimpleSun::stopSunLoopTask()
{
    print("SimpleSun::stopSunLoopTask()");
    this->deleteTimer(this->numTimer);
    vTaskSuspend(this->hTaskSunLoop);
}

/**
 * @brief listener function for receiving message from dispatcher
 * 
 * @param string_ 
 * @param event_ 
 */
void SimpleSun::listener(String string_, EventEnum event_) {
    print( "listener() for ", false);
    print( "simpleSun"      , false);
    print( ", got: "        , false );
    print( string_.c_str()  , false );
    print( ", "             , false );
    print( event_ ); 
    print( "SunState: ", false );
    print( this->getSunState() );
    print( "LightState: ", false );
    print( this->getLightState() );

    /* -------------------------------------------------- */
    /* switch light on                                    */
    if( (strcmp("light_on", string_.c_str() ) == 0))  
    {
        print("light_on");
        this->lightOn();
    }
    /* -------------------------------------------------- */
    /* switch light off                                   */
    if( (strcmp("light_off", string_.c_str() ) == 0))  
    {
        print("light_off");
        this->lightOff();
        this->sunDown();
    }
    /* -------------------------------------------------- */
    /* sunup -> start sunrise                             */
    if( (strcmp("sunup", string_.c_str() ) == 0))  
    {
        /* --------------------------------------------- */
        /* let the sun rise                              */
        this->sunRise();
    }
    /* -------------------------------------------------- */
    /* start light test                                   */
    if( (strcmp("LightTestOn", string_.c_str() ) == 0))  
    {
        /* --------------------------------------------- */
        /* licht test                                    */
        this->lightRGB();
    }
    /* -------------------------------------------------- */
    /* stop ligth test                                    */
    if( (strcmp("LightTestOff", string_.c_str() ) == 0))  
    {
        /* --------------------------------------------- */
        /* switch off the ligh                           */
        this->lightOff();
    }
}    
