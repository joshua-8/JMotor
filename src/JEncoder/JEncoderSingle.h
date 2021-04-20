/**
 * @brief  reads a single channel (incremental) encoder
 * 
 * direction can't be calculated with a single channel encoder, only speed can be. \n
 * speed calulation is done by measuring time between 2 encoder ticks (not between every tick since encoders may not have evenly spaced ticks) \n
 * velocity is set to zero if the encoder has not turned in slowestIntervalMicros
 * 
 * @note  don't use this class directly, use a subclass
 */
#ifndef J_ENCODER_ATTACH_INTERRUPT_SINGLE_H
#define J_ENCODER_ATTACH_INTERRUPT_SINGLE_H
#include "JEncoder.h"
#include <Arduino.h>
/* \def
 * Functions called by interrupts can't expect parameters or be functions of classes so a workaround is needed to use interrupts in a class.
 * The workaround that's used here is global functions need to be made when a new instance of this class is made.
 * The global function get used when attaching interrupts, and the global function can call a function inside the class.
 * The following macro makes it easy to make the global function.
 */
#define jENCODER_SINGLE_MAKE_ISR_MACRO(name) \
    void name##_jENCODER_ISR() { name.encoderISR(); }

class JEncoderSingle : public JEncoder {

protected:
    byte encoderPin;

private:
    int8_t reverse;
    float distPerCountFactor;

    volatile boolean velNew;
    volatile long tickCounter;
    volatile unsigned long lastEncoderTickMicros;
    volatile unsigned long encoderIntervalMicros;
    boolean newSpeed;
    unsigned long slowestIntervalMicros;
    boolean wasTimedOut;

protected:
    /**
     * @brief  constructor, sets pins and settings
     * @param  _encoderAPin: encoder input pin
     * @param  _countsToDistFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderSingle(byte _encoderPin, float _countsToDistFactor = 1.0, boolean _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
    {
        encoderPin = _encoderPin;
        distPerCountFactor = _countsToDistFactor;
        slowestIntervalMicros = _slowestIntervalMicros;
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }

        velNew = false;
        tickCounter = 0;
        encoderIntervalMicros = 0;
        lastEncoderTickMicros = 0;
        newSpeed = false;
        wasTimedOut = false;
    }
    /**
     * @brief  set up pins and interrupts
     * @param  _isrPointer: global function that calls internal ISRA to use with enableInterrupt
     */
    virtual void setUpInterrupts(void (*_isrPointer)(void));
    /**
     * @brief  disable interrupts and stop monitoring encoder
     * @note use setUpInterrupts to start encoder again
     */
    virtual void turnOffInterrupts();

public:
    long zeroCounter()
    {
        long tempCounter = tickCounter * reverse;
        tickCounter = 0;
        return tempCounter;
    }
    float getVel()
    {
        //store temporary copies of variables set by ISR since they may be changed at any time
        unsigned long tempEncoderTickMicros = lastEncoderTickMicros;
        unsigned long tempInterval = encoderIntervalMicros;

        //occasionally micros seems to be smaller than tempEncoderTickMicros and there's an overflow
        //  adding tempInterval fixes the problem (looking back one more tick)
        if ((micros() - tempEncoderTickMicros + tempInterval) > slowestIntervalMicros) {
            return 0.0; //set to zero if it's been a while since a tick
        }
        if (tempInterval == 0) { //avoid divide by zero
            return 0.0;
        }
        return reverse * 1000000.0 / tempInterval * distPerCountFactor * 2;
    }
    long getCounter()
    {
        return tickCounter * reverse;
    }
    float getDist()
    {
        return tickCounter * distPerCountFactor * reverse;
    }
    float getDistPerCountFactor()
    {
        return distPerCountFactor;
    }
    void setDistPerCountFactor(float _factor)
    {
        distPerCountFactor = _factor;
    }
    /**
     * @brief  reverse readings of encoder
     * @param  _reverse: whether to invert sign of readings
     * @retval None
     */
    void setReverse(boolean _reverse)
    {
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
    }
    boolean hasDirection()
    {
        return false;
    }
    boolean isVelNew()
    {
        //occasionally micros seems to be smaller than tempEncoderTickMicros and there's an overflow
        //  adding tempInterval fixes the problem (looking back one more tick)

        unsigned long tempEncoderTickMicros = lastEncoderTickMicros;
        unsigned long tempInterval = encoderIntervalMicros;
        if ((micros() - tempEncoderTickMicros + tempInterval) > slowestIntervalMicros) {
            if (!wasTimedOut) {
                wasTimedOut = true;
                return true;
            }
        } else {
            wasTimedOut = false;
        }

        if (newSpeed) {
            newSpeed = false;
            return true;
        }
        return false;
    }
    void run() { }

    void encoderISR(void)
    {
        if (digitalRead(encoderPin) == HIGH) { //once a cycle save values used for speed calculations
            unsigned long tempMicros = micros();
            encoderIntervalMicros = tempMicros - lastEncoderTickMicros;
            lastEncoderTickMicros = tempMicros;
            newSpeed = true;
        }
        tickCounter++;
    }
};
#endif
