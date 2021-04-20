/**
 * @brief  reads a quadrature (incremental) encoder
 * 
 * speed calulation is done by measuring time between 4 encoder ticks (not between every tick since encoders may not have evenly spaced ticks)
 * velocity is set to zero if the encoder has not turned in slowestIntervalMicros
 * @note don't use this class directly, use a subclass
 */

#ifndef J_ENCODER_QUADRATURE_H
#define J_ENCODER_QUADRATURE_H
#include "JEncoder.h"
#include <Arduino.h>
/* \def
 * Functions called by interrupts can't expect parameters or be functions of classes so a workaround is needed to use interrupts in a class.
 * The workaround that's used here is global functions need to be made when a new instance of this class is made.
 * The global functions get used when attaching interrupts, and the global functions can call functions inside the class.
 * The following macro makes it easy to make the global functions.
 */
#define ENCODER_MAKE_ISRS_MACRO(name)             \
    void name##_jENCODER_ISR_A() { name.ISRA(); } \
    void name##_jENCODER_ISR_B() { name.ISRB(); }

class JEncoderQuadrature : public JEncoder {

protected:
    byte encoderAPin;
    byte encoderBPin;

private:
    int8_t reverse; //can be 1 or -1
    float distPerCountFactor;

    volatile boolean velNew;
    volatile long tickCounter;
    volatile boolean encForwards;
    volatile unsigned long lastEncoderTickMicros;
    volatile unsigned long encoderIntervalMicros;
    boolean newSpeed;
    unsigned long slowestIntervalMicros;
    boolean wasTimedOut;

protected:
    /**
     * @brief  constructor, sets pins and settings
     * @note   
     * @param  _encoderAPin: one channel of quadrature encoder
     * @param  _encoderBPin: other channel of quadrature encoder
     * @param  _countsToDistFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderQuadrature(byte _encoderAPin, byte _encoderBPin, float _countsToDistFactor = 1.0, boolean _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
    {
        encoderAPin = _encoderAPin;
        encoderBPin = _encoderBPin;
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
        distPerCountFactor = _countsToDistFactor;
        slowestIntervalMicros = _slowestIntervalMicros;

        velNew = false;
        tickCounter = 0;
        encForwards = true;
        encoderIntervalMicros = 0;
        lastEncoderTickMicros = 0;
        newSpeed = false;
        wasTimedOut = false;
    }
    /**
     * @brief  set up pins and interrupts
     * @param  _isrAPointer: global function that calls internal ISRA, to use with enableInterrupt
     * @param  _isrBPointer: global function that calls internal ISRB, to use with enableInterrupt
     */
    virtual void setUpInterrupts(void (*_isrAPointer)(void), void (*_isrBPointer)(void));
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

        if (encForwards) {
            return reverse * 1000000.0 / (tempInterval)*distPerCountFactor * 4;
        } else {
            return reverse * -1000000.0 / (tempInterval)*distPerCountFactor * 4;
        }
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
        return true;
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

    void ISRA(void)
    {
        if (digitalRead(encoderAPin) == HIGH) { //once a cycle save values used for speed calculations
            unsigned long tempMicros = micros();
            encoderIntervalMicros = tempMicros - lastEncoderTickMicros;
            lastEncoderTickMicros = tempMicros;
            newSpeed = true;
        }
        encForwards = (digitalRead(encoderAPin) == digitalRead(encoderBPin));
        if (encForwards) {
            tickCounter++;
        } else {
            tickCounter--;
        }
    }
    void ISRB(void)
    {
        encForwards = (digitalRead(encoderAPin) != digitalRead(encoderBPin));
        if (encForwards) {
            tickCounter++;
        } else {
            tickCounter--;
        }
    }
};
#endif
