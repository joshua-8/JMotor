#ifndef J_ENCODER_SINGLE_ATTACH_INTERRUPT_H
#define J_ENCODER_SINGLE_ATTACH_INTERRUPT_H
#include "JEncoderSingle.h"
#include <Arduino.h>
/**
 * @brief  uses attachInterrupt()
 * platform: esp32, teensy 
 * @note  (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */
class JEncoderSingleAttachInterrupt : public JEncoderSingle {
public:
    /**
     * @brief  constructor, sets pins and settings
     * @param  _encoderPin: encoder input pin
     * @param  _distPerCountFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderSingleAttachInterrupt(byte _encoderPin, float _distPerCountFactor = 1.0, bool _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderSingle(_encoderPin, _distPerCountFactor, _reverse, _slowestIntervalMicros)
    {
    }
    /**
     * @note interrupt created by jENCODER_SINGLE_MAKE_ISR_MACRO(name of encoder) will be of the form <name of encoder instance>_jENCODER_ISR
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {

        pinMode(encoderPin, INPUT);

        attachInterrupt(encoderPin, _isrPointer, CHANGE);
    }
    void turnOffInterrupts()
    {
        detachInterrupt(encoderPin);
    }
};
#endif
