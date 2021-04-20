/**
 * \file JEncoderQuadraturePinChange.h
 * @note  uses a pin change interrupt library to support more pins than attachInterrupt() \n
 *      interrupt library (tested with v1.1.0): https://github.com/GreyGnome/EnableInterrupt \n
 * platform: AVR (standard Arduinos)
 */
#ifndef J_ENCODER_QUADRATURE_PIN_CHANGE_H
#define J_ENCODER_QUADRATURE_PIN_CHANGE_H
#include "EnableInterrupt.h" //https://github.com/GreyGnome/EnableInterrupt
#include "JEncoderQuadrature.h"
#include <Arduino.h>

class JEncoderQuadraturePinChange : public JEncoderQuadrature {
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
    JEncoderQuadraturePinChange(byte _encoderAPin, byte _encoderBPin, float _countsToDistFactor = 1.0, boolean _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderQuadrature(_encoderAPin, _encoderBPin, _countsToDistFactor, _reverse, _slowestIntervalMicros)
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
