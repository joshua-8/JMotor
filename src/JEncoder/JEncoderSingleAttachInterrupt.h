/**
 * @note  uses attachInterrupt() \n
 * platform: esp32, teensy \n
 *          (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */
#ifndef J_ENCODER_SINGLE_ATTACH_INTERRUPT_H
#define J_ENCODER_SINGLE_ATTACH_INTERRUPT_H
#include "JEncoderSingle.h"
#include <Arduino.h>
class JEncoderSingleAttachInterrupt : public JEncoderSingle {
public:
    /**
     * @brief  constructor, sets pins and settings
     * @param  _encoderAPin: encoder input pin
     * @param  _countsToDistFactor: conversion factor for getting distance in an actual unit
     * @param  _reverse: false(default)
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     */
    JEncoderSingleAttachInterrupt(byte _encoderPin, float _countsToDistFactor = 1.0, boolean _reverse = false, unsigned long _slowestIntervalMicros = 100000UL)
        : JEncoderSingle(_encoderPin, _countsToDistFactor, _reverse, _slowestIntervalMicros)
    {
    }
    /**
     * @brief  set up pins and interrupts
     * @note interrupt created by jENCODER_SINGLE_MAKE_ISR_MACRO(name of encoder) will be of the form <name of encoder instance>_jENCODER_ISR
     * @param  _isrPointer: global function that calls internal ISR, to use with enableInterrupt
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
};
#endif
