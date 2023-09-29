#include <SimpleTimer.h>
#include <SimpleSun.h>

/**
 * @brief init function, set the start values
 * 
 * @param iWakeDelay 
 */
void SimpleSun::init()
{
    this->sendToQueue( "SimpleSun::init()" );
    /* -------------------------------------------------- */
    /*  1 Sekunde entspricht 1.000 Millisekunden          */
    this->timerDelay = this->iWakeDelay * 60 * 1000/SUN_PHASE;
    this->sunPhase  = 0;

    this->sendToQueue( "  timerDelay[" + std::to_string(this->timerDelay) + "]" );
    this->sendToQueue( "  sunPhase  [" + std::to_string(this->sunPhase)   + "]" );
    /* -------------------------------------------------- */
    /* create the timer                                   */
    this->setNumTimer(this->setInterval( this->timerDelay, this->ptrTimerCB ));
    if( this->getNumTimer() < 0 )
    {
        this->sendToQueue("  ERROR: NumTimer[" + std::to_string( this->getNumTimer() ) + "]");
    }
    else
    {
        this->sendToQueue("  NumTimer[" + std::to_string( this->getNumTimer() ) + "] created");
    }
}
/**
 * @brief initialzes the led driver
 * 
 * @return true 
 * @return false 
 */
bool SimpleSun::init_ledDriver()
{
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
    if(digitalLeds_initDriver() == 0) 
    {
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
    digitalLeds_drawPixels(this->STRANDS, this->STRANDCNT);
}

int SimpleSun::getMapValue( int _level)
{
    return map(this->sunPhase, 0, SUN_PHASE, 0, _level);
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
    strand_t* strip = this->STRANDS[0];

    int iMod1  = this->sunPhase%this->getNumLeds();
    if(iMod1==0) 
        iMod1=this->getNumLeds();

    for(int i = 0; i < iMod1; i++)
    {
        if( iMod1 == 1 )
        {
            this->iRed   = getMapValue(RED_LEVEL);
            this->iGreen = getMapValue(GREEN_LEVEL);
            this->iBlue  = getMapValue(BLUE_LEVEL);
            this->iWhite = getMapValue(WHITE_LEVEL);
        }
        else{
            /* set all led step by step to the new values */
            this->iRed   = strip->pixels[0].r;
            this->iGreen = strip->pixels[0].g;
            this->iBlue  = strip->pixels[0].b;
            this->iWhite = strip->pixels[0].w;
        }
        strip->pixels[i] = pixelFromRGBW( this->iRed, this->iGreen, this->iBlue, this->iWhite);
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
    this->iWakeDelay = iDelay;
    this->timerDelay = this->iWakeDelay * 60 * 1000/SUN_PHASE;

    this->sendToQueue("SimpleSun::setWakeDelay(" + std::to_string(iDelay) + "): this->timerDelay[" + std::to_string(this->timerDelay) + "]");
}
/**
 * @brief return the value of wakeDelay
 * 
 * @return unsigned int 
 */
unsigned int SimpleSun::getWakeDelay()
{
    this->sendToQueue("SimpleSun::getWakeDelay(): " + this->iWakeDelay );
    return this->iWakeDelay;
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
 * @brief set all leds
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
void SimpleSun::letSunRise( boolean bInit_ )
{
    /* -------------------------------------------------- */
    /* init sunrise                                       */
    if( bInit_ )
    {
        this->sendToQueue("SimpleSun::letSunRise(" + std::to_string(bInit_) + ")" );
        /* ----------------------------------------------- */
        /* set start parameters and create timer           */
        this->init();
    }
    /* -------------------------------------------------- */
    /* debug info                                         */
    if( this->sunPhase%100 == 0 )
    {
        this->sendToQueue("SimpleSun::letSunRise(" + std::to_string(bInit_) + ")" );
        this->sendToQueue("SimpleSun::letSunRise() - this->sunPhase [" + std::to_string(this->sunPhase) + "]");
    }
    /* -------------------------------------------------- */
    /* rise the sun                                       */
    this->sunrise();
    /* -------------------------------------------------- */
    /* check if sunPhase has reached SUN_PHASE            */
    if( !this->getSunPhase() )
    {
        /* ----------------------------------------------- */
        /* sun is risen                                    */
        this->deleteTimer(this->numTimer);
        this->sunUp();
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
                    4096,                   /* Stack size of task */
                    NULL,                   /* parameter of the task */
                    1,                      /* priority of the task */
                    &this->hTaskSunLoop,    /* Task handle to keep track of created task */
                    1                       /* pin task to core 1 */
    );    
    vTaskSuspend(this->hTaskSunLoop);
}

/**
 * @brief delete sun loop task
 * 
 */
void SimpleSun::stopSunLoopTask()
{
    this->deleteTimer(this->numTimer);
    vTaskSuspend(this->hTaskSunLoop);
}

/**
 * @brief listener function for receiving message from dispatcher
 * 
 * @param string_ 
 * @param event_ 
 */
void SimpleSun::listener(String string_, EventEnum event_) 
{
    std::string msg = string_.c_str();
    this->sendToQueue( "SimpleSun::listener[" + msg + "]" );
    /* -------------------------------------------------- */
    /* switch light on                                    */
    if( (strcmp("light_on", string_.c_str() ) == 0))  
    {
        this->lightOn();
    }
    /* -------------------------------------------------- */
    /* switch light off                                   */
    if( (strcmp("light_off", string_.c_str() ) == 0))  
    {
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
    if( (strcmp("alaup", string_.c_str() ) == 0))  
    {
        this->sunUp();
        this->lightOn();
    }
    /* -------------------------------------------------- */
    /* a_off -> switch alarm off                          */
    if( (strcmp("a_off", string_.c_str() ) == 0))  
    {
        
    }


}    
