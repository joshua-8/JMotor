/**
 * @note uses attachInterrupt() \n
 * platform: esp32, teensy \n
 *      (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */
#include "JEncoderPWMAbsolute.h"
#include <Arduino.h>
#ifndef J_ENCODER_PWM_ABSOLUTE_ATTACH_INTERRUPT_H
#define J_ENCODER_PWM_ABSOLUTE_ATTACH_INTERRUPT_H
class JEncoderPWMAbsoluteAttachInterrupt : public JEncoderPWMAbsolute {
public:
    /**
     * @brief  sets pins and settings for reading the encoder
     * @param  _encoderPin: pin to read encoder signal with
     * @param  _ps: (struct pwmSettings) parameters of encoder signal
     * @param  _reverse: (boolean) reverse positive direction, default=false 
     * @param  _distPerCountFactor: (float) for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder
     * @param  _velEnoughTime: (default=0, no limit) shortest interval between velocity calculations, if run() is called faster the calculation will wait to run
     * @param  _velEnoughTicks: (default=0, no limit) if the encoder turns more than this number of steps velocity calculations will be done even if velEnoughTime hasn't been reached
     */
    JEncoderPWMAbsoluteAttachInterrupt(byte _encoderPin, pwmSettings _ps, boolean _reverse = false, float _distPerCountFactor = 1.0, unsigned long _velEnoughTime = 0, unsigned long _velEnoughTicks = 0)
        : JEncoderPWMAbsolute(_encoderPin, _ps, _reverse, _distPerCountFactor, _velEnoughTime, _velEnoughTicks)
    {
    }

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
