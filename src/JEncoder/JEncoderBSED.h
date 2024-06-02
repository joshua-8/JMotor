#ifndef J_ENCODER_BSED_H
#define J_ENCODER_BSED_H
#include "JEncoder.h"
#include <Arduino.h>
#include <byte-sized-encoder-decoder.h>
/**
 * @brief  reads a quadrature (incremental) encoder that is connected to a Byte Sized Encoder Decoder board connected over I2C to the main controller
 */

class JEncoderBSED : public JEncoder {

protected:
    ByteSizedEncoderDecoder& bsed;
    byte encoderChannel;
    float distPerCountFactor;
    int8_t reverse; // can be 1 or -1
    int32_t zeroVal;

public:
    /**
     * @brief  constructor, sets pins and settings
     * @note
     * @param  _bsed: instance of BSED class
     * @param  _encoderChannel: channel of encoder on BSED (1-8)
     * @param  _reverse: false(default)
     * @param  _distPerCountFactor: conversion factor for getting distance in an actual unit
     * @param  _slowestIntervalMicros: after this many microseconds without an encoder tick velocity is set to zero.
     * @param  _encoderEnoughCounts: enough counts to calculate velocity from
     */
    JEncoderBSED(ByteSizedEncoderDecoder& _bsed, byte _encoderChannel, bool _reverse = false, float _distPerCountFactor = 1.0, int16_t _slowestIntervalMicros = 0, int16_t _encoderEnoughCounts = 0)
        : bsed(_bsed)
    {
        encoderChannel = constrain(_encoderChannel, 1, 8);
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
        zeroVal = 0;
        distPerCountFactor = _distPerCountFactor;
        bsed.setEncoderSlowestInterval(_encoderChannel, _slowestIntervalMicros);
        bsed.setEncoderEnoughCounts(_encoderChannel, _encoderEnoughCounts);
    }

    long zeroCounter()
    {
        zeroVal = bsed.getEncoderPosition(encoderChannel);
        return zeroVal;
    }
    float getVel()
    {
        return bsed.getEncoderVelocity(encoderChannel) * distPerCountFactor * reverse;
    }
    long getCounter()
    {
        return (bsed.getEncoderPosition(encoderChannel) - zeroVal) * reverse;
    }
    float getPos()
    {
        return (bsed.getEncoderPosition(encoderChannel) - zeroVal) * distPerCountFactor * reverse;
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
    void setReverse(bool _reverse)
    {
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
    }
    bool hasDirection()
    {
        return true;
    }
    bool isVelNew()
    {
        return bsed.isVelNew(encoderChannel);
    }
    void run()
    {
    }
};

#endif // JENCODER_QUADRATURE_20COPY_H
