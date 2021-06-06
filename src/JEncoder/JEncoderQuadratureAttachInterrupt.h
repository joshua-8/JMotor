#ifndef J_ENCODER_QUADRATURE_ATTACH_INTERRUPT_H
#define J_ENCODER_QUADRATURE_ATTACH_INTERRUPT_H
#include "JEncoderQuadrature.h"
#include <Arduino.h>
/**
 * @brief  uses attachInterrupt() 
 * platform: esp32, teensy
 * @note (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */

class JEncoderQuadratureAttachInterrupt : public JEncoderQuadrature {

public:
    /**
     * @brief  constructor, sets pins and settings
     * @note   
     * @param  _encoderAPin: one channel of quadrature encoder
     * @param  _encoderBPin: other channel of quadrature encoder
     * @param  _distPerCountFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderQuadratureAttachInterrupt(byte _encoderAPin, byte _encoderBPin, float _distPerCountFactor = 1.0, bool _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderQuadrature(_encoderAPin, _encoderBPin, _distPerCountFactor, _reverse, _slowestIntervalMicros)
    {
    }

    void setUpInterrupts(void (*_isrAPointer)(void), void (*_isrBPointer)(void))
    {

        pinMode(encoderAPin, INPUT);
        pinMode(encoderBPin, INPUT);

        attachInterrupt(encoderAPin, _isrAPointer, CHANGE);
        attachInterrupt(encoderBPin, _isrBPointer, CHANGE);
    }
    void turnOffInterrupts()
    {
        detachInterrupt(encoderAPin);
        detachInterrupt(encoderBPin);
    }
};
#endif
