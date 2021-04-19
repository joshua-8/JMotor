/**
 * @brief  reads a PWM signal from an absolute encoder using attachInterrupt()
 * tested with https://ams.com/as0548b 
 * @note  platform: esp32, teensy \n
 *          (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */

#ifndef J_ENCODER_ATTACH_INTERRUPT_PWM_ABSOLUTE
#define J_ENCODER_ATTACH_INTERRUPT_PWM_ABSOLUTE
#include "JEncoder.h"
#include <Arduino.h>

class JEncoderAttachInterruptPWMAbsolute : private JEncoder {
private:
    byte encoderPin;
    void (*isrPointer)(void);
    long turns;
    uint16_t angle;
    uint16_t lastAngle;
    int8_t reverse;
    float distPerCountFactor;
    boolean newSpeed;

    unsigned long dataStartMicros;
    unsigned long dataEndMicros;
    unsigned long earlyDataStartMicros;

    struct pwmSettings {
        uint16_t RESOLUTION; //resolution of encoder data (pwm steps)
        uint16_t PWM_STEPS; //number of total clock steps in pwm output
        uint16_t MIN_STEPS; //number of steps corresponding to min angle. MAX_STEPS=MIN_STEPS+RESOLUTION
        boolean dataState; //HIGH or LOW, which state when pulse length increasing=increasing angle
    };

public:
    pwmSettings AS5048settings = { 4095, 4119, 16, HIGH };
    pwmSettings MA310bitsettings = { 1023, 1026, 1, HIGH }; //untested
    pwmSettings MA310bitsettings = { 4095, 4098, 1, HIGH }; //untested

    /**
     * @brief  set up pins and interrupts
     * @param  _isrPointer: global function that calls internal ISRA, to use with enableInterrupt
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {
        isrPointer = _isrPointer;

        pinMode(encoderPin, INPUT);

        attachInterrupt(encoderPin, isrPointer, CHANGE);
    }
    /**
     * @brief  disable interrupts and stop monitoring encoder
     * @note use setUpInterrupts to start encoder again
     */
    void turnOffInterrupts()
    {
        detachInterrupt(encoderPin);
    }

    long zeroCounter()
    {
        long tempTurns = turns * reverse;
        turns = 0;
        angle = 0;
        return tempTurns;
    }

    float getVel()
    {
    }

    long getCounter()
    {
        return (turns * RESOLUTION + angle) * reverse;
    }

    float getDist()
    {
        return (turns * RESOLUTION + angle) * reverse * distPerCountFactor;
    }

    float getDistPerCountFactor()
    {
        return distPerCountFactor;
    }

    /**
     * @note for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder.
     *      it is converted to actual counts within this function
     */
    void setDistPerCountFactor(float _factor)
    {
        distPerCountFactor = _factor / RESOLUTION;
    }

    boolean hasDirection()
    {
        return true;
    }

    boolean isVelNew()
    {
        return newSpeed;
    }

    /**
     * @note   call this function as frequently as possible or this code can't keep track of how many times the encoder has turned
     */
    void run()
    {
        if (abs(angle - lastAngle) > RESOLUTION / 2) { // angle jump over half of circle is assummed to be the shorter crossing of 0
            if (angle > lastAngle) {
                turns--;
            } else {
                turns++;
            }
        }
    }
};

#endif
