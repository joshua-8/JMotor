#ifndef J_ENCODER_SINGLE_ATTACH_INTERRUPT_H
#define J_ENCODER_SINGLE_ATTACH_INTERRUPT_H
#include "JEncoderSingle.h"
#include <Arduino.h>
/**
 * @brief  uses attachInterrupt()
 * platform: esp32, teensy (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 * @note make sure to call setUpInterrupts() when your code starts
 */
class JEncoderSingleAttachInterrupt : public JEncoderSingle {
public:
    /**
     * @brief  constructor, sets pins and settings
     * @param  _encoderPin: encoder input pin
     * @param  _distPerCountFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     * @param  _switchBounceIntervalMicros: ignore additional pulses for this many microseconds after each pulse
     * @param  _interruptType: counts on RISING, FALLING, or CHANGE interrupt
     */
    JEncoderSingleAttachInterrupt(byte _encoderPin, float _distPerCountFactor = 1.0, bool _reverse = false, unsigned long _slowestIntervalMicros = 100000UL, unsigned long _switchBounceIntervalMicros = 0, byte _interruptType = CHANGE)
        : JEncoderSingle(_encoderPin, _distPerCountFactor, _reverse, _slowestIntervalMicros, _switchBounceIntervalMicros, _interruptType)
    {
    }
    /**
     * @note interrupt created by jENCODER_SINGLE_MAKE_ISR_MACRO(name of encoder) will be of the form <name of encoder instance>_jENCODER_ISR
     */
    void setUpInterrupts(void (*_isrPointer)(void))
    {

        pinMode(encoderPin, INPUT);

        attachInterrupt(encoderPin, _isrPointer, interruptType);
    }
    void turnOffInterrupts()
    {
        detachInterrupt(encoderPin);
    }
};
#endif
