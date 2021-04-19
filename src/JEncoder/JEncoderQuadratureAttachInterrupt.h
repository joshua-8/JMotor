/**
 * @note  uses attachInterrupt() \n
 * platform: esp32, teensy \n
 *          (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */

#ifndef J_ENCODER_QUADRATURE_ATTACH_INTERRUPT_H
#define J_ENCODER_QUADRATURE_ATTACH_INTERRUPT_H
#include "JEncoderQuadrature.h"
#include <Arduino.h>

class JEncoderQuadratureAttachInterrupt : public JEncoderQuadrature {

public:
    /**
     * @brief  constructor, sets pins and settings
     * @note   
     * @param  _encoderAPin: one channel of quadrature encoder
     * @param  _encoderBPin: other channel of quadrature encoder
     * @param  _countsToDistFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderQuadratureAttachInterrupt(byte _encoderAPin, byte _encoderBPin, float _countsToDistFactor = 1.0, boolean _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderQuadrature(_encoderAPin, _encoderBPin, _countsToDistFactor, _reverse, _slowestIntervalMicros)
    {
    }

    /**
     * @brief  set up pins and interrupts
     * @param  _isrAPointer: global function that calls internal ISRA, to use with enableInterrupt
     * @param  _isrBPointer: global function that calls internal ISRA, to use with enableInterrupt
     */
    void setUpInterrupts(void (*_isrAPointer)(void), void (*_isrBPointer)(void))
    {
        isrAPointer = _isrAPointer;
        isrBPointer = _isrBPointer;

        pinMode(encoderAPin, INPUT);
        pinMode(encoderBPin, INPUT);

        attachInterrupt(encoderAPin, isrAPointer, CHANGE);
        attachInterrupt(encoderBPin, isrBPointer, CHANGE);
    }
    /**
     * @brief  disable interrupts and stop monitoring encoder
     * @note use setUpInterrupts to start encoder again
     */
    void turnOffInterrupts()
    {
        detachInterrupt(encoderAPin);
        detachInterrupt(encoderBPin);
    }
};
#endif
