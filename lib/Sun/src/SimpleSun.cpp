#include <SimpleTimer.h>
#include <SimpleSun.h>


/**
 * @brief init function, set the start values
 * 
 * @param iWhite 
 * @param iSunPhase 
 * @param iFadeStep 
 * @param iSunFadeStep 
 * @param iWakeDelay 
 */
void SimpleSun::init(int iWhite, int iSunPhase, int iFadeStep, int iSunFadeStep, int iWakeDelay)
{
    this->whiteLevel  = iWhite;
    this->sunPhase    = iSunPhase;
    this->fadeStep    = iFadeStep;
    this->sunFadeStep = iSunFadeStep;
    this->wakeDelay   = iWakeDelay*10;
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
            vTaskDelay(5000/portTICK_PERIOD_MS);
            resetPixels();
            vTaskDelay(5000/portTICK_PERIOD_MS);
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
 * @brief calculates the aurora leds (red, green, white)
 * 
 */
void SimpleSun::drawAurora()
{
    strand_t* strip = this->STRANDS[0];
    this->currentAurora = map(this->sunPhase, 0, 100, 0, this->aurora);
    if(this->currentAurora % 2 != 0)
    {
        this->currentAurora--;
    }
    if (this->currentAurora != this->oldAurora)
    {
        this->fadeStep = 0;
    }
    int sunStart       = (this->getNumLeds()/2)-(this->currentAurora/2);
    int newAuroraLeft  = sunStart-1;
    int newAuroraRight = sunStart+this->currentAurora;
    if(newAuroraLeft >= 0 && newAuroraRight <= this->getNumLeds())
    {
        int redValue   =  map(this->fadeStep, 0, 100, this->whiteLevel, 95);
        int greenValue =  map(this->fadeStep, 0, 100,                0, 15);

        strip->pixels[newAuroraRight] = pixelFromRGBW( redValue, greenValue, 0, 0);
        strip->pixels[newAuroraLeft]  = pixelFromRGBW( redValue, greenValue, 0, 0);
    }
    for(int i = sunStart; i < sunStart+this->currentAurora; i++)
    {
        strip->pixels[i] = pixelFromRGBW( min(strip->pixels[i].r+4, 127)
                                        , min(strip->pixels[i].g+1, 25), 0, 0); 
    }
    this->oldAurora   = this->currentAurora;
}
/**
 * @brief calculates the white level and returns its value
 * 
 * @return int 
 */
int SimpleSun::calWhiteValue()
{
    int whiteValue = 0;
    if( this->whiteLevel <= 60 )
    {
        whiteValue = map(this->whiteLevel, 0, 60, 0, 30);
    }
    else if( this->whiteLevel > 60 && this->whiteLevel <= 90 )
    {
        whiteValue = map(this->whiteLevel, 0, 90, 0, 80);
    }
    else
    {
        whiteValue = min((int)map(this->whiteLevel, 0, 100, 0, 255), 255);
    }
#ifdef _DEBUG_SUN_DETAILS_
    print("calWhiteValue() : ", false);
    print(whiteValue);
#endif
    return whiteValue;
}
/**
 * @brief calculates the ambiente leds (white)
 * 
 */
void SimpleSun::drawAmbient()
{
    /* -------------------------------------------------- */
    /* ab 25 wird led nicht mehr wirklich heller          */
    /* insgesamt wird es zu schnell hell                  */
    /* -> erste einzelne led zuschalten                   */ 
    /* -> bis Anzahl led erreicht                         */
    /* dann durch die Reihe heller werden                 */
    /* this->whiteLevel läuft von 1 - 100                 */
    /* -> 0 - 28 je eine led dazu                         */
    /* -> ab 28 je 2 erhöhen                              */
    strand_t* strip = this->STRANDS[0]; 
#ifdef _DEBUG_SUN_DETAILS_
    print("drawAmbient() - whiteLevel: ", false);
    print( this->whiteLevel );
#endif
    if( this->whiteLevel < 28 )
    {
        for(int i = 0; i <= this->whiteLevel; i++)
        {
            strip->pixels[i] = pixelFromRGBW(  0,   0,  1,  1); 
        }
    }
    else
    {
        for(int i = 0; i < this->whiteLevel%29; i++)
        {
            strip->pixels[i] = pixelFromRGBW( 0, 0, 2, min(strip->pixels[i].w+2, 255) ); 
        }
    }
}
/**
 * @brief function to calculate the sun leds (red, white)
 * 
 */
void SimpleSun::drawSun()
{
    strand_t* strip = this->STRANDS[0];
    this->currentSun = map(this->sunPhase, 0, 100, 0, this->sun);
    if(this->currentSun % 2 != 0)
    {
       this->currentSun--;
    }
    if (this->currentSun != this->oldSun)
    {
        this->sunFadeStep = 0;
    }
    int sunStart    = (this->getNumLeds()/2)-(this->currentSun/2);
    int newSunLeft  = sunStart-1;
    int newSunRight = sunStart+this->currentSun;
    int whiteVale   = this->calWhiteValue();
    if(newSunLeft >= 0 && newSunRight <= this->getNumLeds() && this->sunPhase > 0)
    {
        int redValue   = map(this->sunFadeStep, 0, 100, 0, max(this->whiteLevel,50));
        int greenValue = map(this->sunFadeStep, 0, 100, 0, min(this->whiteLevel,25));
        int blueValue  = map(this->sunFadeStep, 0, 100, 0, min(this->whiteLevel,20));
        
        strip->pixels[newSunLeft]  = pixelFromRGBW( redValue, greenValue, blueValue, whiteVale);
        strip->pixels[newSunRight] = pixelFromRGBW( redValue, greenValue, blueValue, whiteVale);
    }
    for(int i = sunStart; i < sunStart+this->currentSun; i++)
    {
        strip->pixels[i] = pixelFromRGBW( min(strip->pixels[i].r+5, 255)
                                        , min(strip->pixels[i].g+2, 64)
                                        , 0
                                        , whiteVale);
    }
    this->oldSun = this->currentSun;
}
/**
 * @brief calls function to calculate ambiente, aurora and sun
 * 
 */
void SimpleSun::calSun()
{
#ifdef _DEBUG_SUN_DETAILS_
    dbSunSerialPrintln("SimpleSun::calSun()");
#endif
    this->drawAmbient();
    this->drawAurora();
    this->drawSun();
}
/**
 * @brief increases sunPhase, ambiente and aurora
 * 
 */
void SimpleSun::sunrise()
{
#ifdef _DEBUG_SUN_DETAILS_
    dbSunSerialPrintln("SimpleSun::sunrise()");
#endif
    this->increaseSunPhase();
    this->calSun();
    this->drawPixels();
}
/**
 * @brief decreases sunPhase, ambiente and aurora
 * 
 */
void SimpleSun::sunset()
{
    this->decreaseSunPhase();
    this->calSun();
    this->drawPixels();
}
/**
 * @brief increase value of sunPhase and call increase functions
 * 
 */
void SimpleSun::increaseSunPhase()
{
#ifdef _DEBUG_SUN_DETAILS_
    dbSunSerialPrintln("SimpleSun::increaseSunPhase()");
#endif
    if (this->sunPhase < 100)
    {
        this->sunPhase++;
        this->increaseWhiteLevel();
        this->increaseFadeStep();
        this->increaseSunFadeStep();      
    }
    else
    {
        dbSunSerialPrintln("...and it is light.");
        this->sunUp();
    }
}
/**
 * @brief decrease value of sunPhase and call decrease functions
 * 
 */
void SimpleSun::decreaseSunPhase()
{
#ifdef _DEBUG_SUN_DETAILS_
    dbSunSerialPrintln("SimpleSun::decreaseSunPhase()");
#endif
    if (this->sunPhase > 0)
    {
        this->sunPhase--;
        this->decreaseWhiteLevel();
        this->decreaseFadeStep();
        this->decreaseSunFadeStep();      
    }
    else
    {
        dbSunSerialPrintln("...and it is darkness.");
        this->sunDown();
    }
}
/**
 * @brief increase the value of whiteLeven (min. 0)
 * 
 */
void SimpleSun::increaseWhiteLevel()
{
    if(this->whiteLevel < 100)
    {
        this->whiteLevel++;
    }
}
/**
 * @brief decrease the value of whiteLeven (min. 0)
 * 
 */
void SimpleSun::decreaseWhiteLevel()
{
    if(this->whiteLevel > 0)
    {
        this->whiteLevel--;
    }
}
/**
 * @brief increases the fadeStep values on step (max 100)
 * 
 */
void SimpleSun::increaseFadeStep()
{
    if (this->fadeStep < 100)
    {
        this->fadeStep++;
    }
}
/**
 * @brief decreases the fadeStep values on step (min 0)
 * 
 */
void SimpleSun::decreaseFadeStep()
{
    if (this->fadeStep > 0)
    {
        this->fadeStep--;
    }
}
/**
 * @brief increas the value of sunFadeStep one step (max 100)
 * 
 */
void SimpleSun::increaseSunFadeStep()
{
    if (this->sunFadeStep < 100)
    {
        this->sunFadeStep++;
    }
}
/**
 * @brief decreas the value of sunFade one step (min 0)
 * 
 */
void SimpleSun::decreaseSunFadeStep()
{
    if (this->sunFadeStep > 0)
    {
        this->sunFadeStep--;
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
/**
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
int SimpleSun::getSunPhase()
{
    return this->sunPhase;
}
/**
 * @brief set all leds to blue
 * 
 */
void SimpleSun::blaulicht()
{
    strand_t* strip = this->STRANDS[0];
    for(int i = 0; i < this->getNumLeds(); i++)
    {
        strip->pixels[i] = pixelFromRGBW( 0, 0, 255, 0 ); 
    }
    this->drawPixels();
}
/**
 * @brief set all leds to blue
 * 
 */
void SimpleSun::weislicht()
{
    strand_t* strip = this->STRANDS[0];
    for(int i = 0; i < this->getNumLeds(); i++)
    {
        strip->pixels[i] = pixelFromRGBW( 255, 255, 255, 255 ); 
    }
    this->drawPixels();
}
/**
 * @brief deletes a timer
 * 
 */
void SimpleSun::delTimer()
{
#ifdef _DEBUG_SUN_DETAILS_
    dbSunSerialPrint("resetTimer():");
    dbSunSerialPrintln(this->numTimer);
#endif
    this->deleteTimer(this->numTimer);
}
/**
 * @brief stores the id of a timer in numTimer
 * 
 * @param iTimer 
 */
void SimpleSun::setNumTimer( int iTimer )
{
#ifdef _DEBUG_SUN_DETAILS_     
    dbSunSerialPrint("setNumTimer():");
    dbSunSerialPrintln(this->numTimer);
#endif
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
void SimpleSun::letSunRise( int intPayload_, bool bInit_ )
{
#ifdef _DEBUG_SUN_DETAILS_
    dbSunSerialPrint("SimpleSun::letSunRise( ");
    dbSunSerialPrint( intPayload_ );
    dbSunSerialPrint( ", " );
    dbSunSerialPrint( bInit_ );
    dbSunSerialPrintln( " )");
#endif
    if( bInit_ )
    {
#ifdef _DEBUG_SUN_DETAILS_
        dbSunSerialPrintln("init sun parameters....");
#endif
        /* set start parameters */
        this->init(0,0,0,0,intPayload_);
    }
    /* rise the sun */
    this->sunrise();
    /* start timer */
    if( strcmp( this->getSunState(), "SunRise" ) == 0 )
    {
#ifdef _DEBUG_SUN_DETAILS_
        dbSunSerialPrintln( "-> set NumTimer");
#endif
        /* continue sunrise */
        this->setNumTimer(this->setTimeout( this->getWakeDelay(), this->ptrTimerCB ));
    }
}

/**
 * @brief create sun loop task
 * 
 */
void SimpleSun::startSunLoopTask()
{
    xTaskCreatePinnedToCore(
                    this->pvTaskCode,       /* Task function. */
                    "TaskSunLoop",          /* name of task. */
                    10000,                  /* Stack size of task */
                    NULL,                   /* parameter of the task */
                    1,                      /* priority of the task */
                    &this->hTaskSunLoop,    /* Task handle to keep track of created task */
                    1                       /* pin task to core 1 */
    );                          
}

/**
 * @brief delete sun loop task
 * 
 */
void SimpleSun::stopSunLoopTask()
{
    vTaskDelete(this->hTaskSunLoop);
}
