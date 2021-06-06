#ifndef J_ENCODER_QUADRATURE_PIN_CHANGE_H
#define J_ENCODER_QUADRATURE_PIN_CHANGE_H
#include "EnableInterrupt.h" //https://github.com/GreyGnome/EnableInterrupt
#include "JEncoderQuadrature.h"
#include <Arduino.h>
/**
 * @brief  uses a pin change interrupt library to support more pins than attachInterrupt().
 * 
 * Interrupt library (tested with v1.1.0): https://github.com/GreyGnome/EnableInterrupt
 * @note platform: AVR (standard Arduinos)
 */
class JEncoderQuadraturePinChange : public JEncoderQuadrature {
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
    JEncoderQuadraturePinChange(byte _encoderAPin, byte _encoderBPin, float _distPerCountFactor = 1.0, bool _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderQuadrature(_encoderAPin, _encoderBPin, _distPerCountFactor, _reverse, _slowestIntervalMicros)
    {
    }

    void setUpInterrupts(void (*_isrAPointer)(void), void (*_isrBPointer)(void))
    {

        pinMode(encoderAPin, INPUT);
        pinMode(encoderBPin, INPUT);

        enableInterrupt(encoderAPin, _isrAPointer, CHANGE);
        enableInterrupt(encoderBPin, _isrBPointer, CHANGE);
    }
    void turnOffInterrupts()
    {
        disableInterrupt(encoderAPin);
        disableInterrupt(encoderBPin);
    }
};
#endif
