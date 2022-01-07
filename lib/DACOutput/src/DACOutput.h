#ifndef __sampler_base_h__
#define __sampler_base_h__

#include <Arduino.h>
#include <driver/i2s.h>

class SampleSource;

/**
 * Base Class for both the ADC and I2S sampler
 **/
class DACOutput
{
private:
    // I2S write task
    TaskHandle_t m_i2sWriterTaskHandle;
    // i2s writer queue
    QueueHandle_t m_i2sQueue;
    // src of samples for us to play
    SampleSource *m_sample_generator;
    // timer
    int m_iTimer;
    //
    TaskHandle_t writerTaskHandle;
    // 
public:
    void start(SampleSource *sample_generator, int iTimer);
    void stop();
    void suspend();
    void resume();

    friend void i2sWriterTask(void *param);

    TaskHandle_t getTaskHandle();
};

#endif