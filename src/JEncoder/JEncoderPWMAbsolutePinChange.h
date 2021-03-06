#include "EnableInterrupt.h" //https://github.com/GreyGnome/EnableInterrupt
#include "JEncoderPWMAbsolute.h"
#include <Arduino.h>
#ifndef J_ENCODER_PWM_ABSOLUTE_PIN_CHANGE_H
#define J_ENCODER_PWM_ABSOLUTE_PIN_CHANGE_H
/**
 * @brief  uses a pin change interrupt library to support more pins than attachInterrupt().
 *
 * Interrupt library (tested with v1.1.0): https://github.com/GreyGnome/EnableInterrupt \n
 * platform: AVR (standard Arduinos)
 * @note make sure to call setUpInterrupts() when your code starts
 */
class JEncoderPWMAbsolutePinChange : public JEncoderPWMAbsolute {
public:
    /**
     * @brief  sets pins and settings for reading the encoder
     * @param  _encoderPin: pin to read encoder signal with
     * @param  _ps: (struct pwmSettings) parameters of encoder signal
     * @param  _reverse: (bool) reverse positive direction, default=false
     * @param  _distPerCountFactor: (float) for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder
     * @param  _velEnoughTime: (default=0, no limit) shortest interval between velocity calculations, if run() is called faster the calculation will wait to run
     * @param  _velEnoughTicks: (default=0, no limit) if the encoder turns more than this number of steps velocity calculations will be done even if velEnoughTime hasn't been reached
     * @param  _resetAngleOnStart: (default=false) zero encoder angle on startup
     * @param  _zeroPos: (default=0) change what angle of the absolute encoder is considered zero. set _resetAngleOnStart to false to keep setting from being overridden
     */
    JEncoderPWMAbsolutePinChange(byte _encoderPin, pwmSettings _ps, bool _reverse = false, float _distPerCountFactor = 1.0, unsigned long _velEnoughTime = 0, unsigned long _velEnoughTicks = 0, bool _resetAngleOnStart = false, uint16_t _zeroPos = 0)
        : JEncoderPWMAbsolute(_encoderPin, _ps, _reverse, _distPerCountFactor, _velEnoughTime, _velEnoughTicks, _resetAngleOnStart, _zeroPos)
    {
    }

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
