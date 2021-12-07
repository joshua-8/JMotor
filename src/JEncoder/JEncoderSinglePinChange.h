#ifndef J_ENCODER_SINGLE_PIN_CHANGE_H
#define J_ENCODER_SINGLE_PIN_CHANGE_H
#include "EnableInterrupt.h" //https://github.com/GreyGnome/EnableInterrupt
#include "JEncoderSingle.h"
#include <Arduino.h>
/**
 * @brief  uses a pin change interrupt library to support more pins than attachInterrupt().
 * 
 * interrupt library (tested with v1.1.0): https://github.com/GreyGnome/EnableInterrupt \n
 * platform: AVR (standard Arduinos)
 */
class JEncoderSinglePinChange : public JEncoderSingle {
public:
    /**
     * @brief  constructor, sets pins and settings
     * @param  _encoderPin: encoder input pin
     * @param  _distPerCountFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderSinglePinChange(byte _encoderPin, float _distPerCountFactor = 1.0, bool _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderSingle(_encoderPin, _distPerCountFactor, _reverse, _slowestIntervalMicros)
    {
    }

    /**
     * @note interrupt created by jENCODER_SINGLE_MAKE_ISR_MACRO(name of encoder) will be of the form <name of encoder instance>_jENCODER_ISR
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {
        pinMode(encoderPin, INPUT);

        enableInterrupt(encoderPin, _isrPointer, CHANGE);
    }
    void turnOffInterrupts()
    {
        disableInterrupt(encoderPin);
    }
};
#endif
