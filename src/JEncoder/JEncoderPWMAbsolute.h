#ifndef J_ENCODER_ATTACH_INTERRUPT_PWM_ABSOLUTE_H
#define J_ENCODER_ATTACH_INTERRUPT_PWM_ABSOLUTE_H
#include "JEncoder.h"
#include <Arduino.h>

/**
 \def
 * parameters for reading PWM output of AS5048 absolute encoders
 */
#define JEncoderPWMAbsolute_AS5048settings \
    {                                      \
        4095, 4119, 16, HIGH, 800, 1100    \
    }

/**
 * @brief  reads a PWM signal from an absolute encoder
 *
 * tested with https://ams.com/as0548b 
 * 
 * @note  platform: esp32, teensy \n
 *          (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */
class JEncoderPWMAbsolute : public JEncoder {
public:
    /**
     * @brief  struct for holding parameters for reading PWM Absolute Encoders
     */
    struct pwmSettings {
        /**
         * @brief resolution of encoder data (pwm steps)
         */
        uint16_t RESOLUTION;
        /**
         * @brief number of total clock steps in pwm output
         */
        uint16_t PWM_STEPS;
        /**
         * @brief  number of steps corresponding to min angle. MAX_STEPS=MIN_STEPS+RESOLUTION
         */
        uint16_t MIN_STEPS;
        /**
         * @brief  HIGH or LOW, which state when pulse length increasing=increasing angle
         */
        bool dataState;
        /**
         * @brief shortest time (microseconds) between falling edges that will be considered a valid encoder signal (low end of frequency range of pwm output)
         */
        uint16_t MIN_CYCLE;
        /**
         * @brief longest time (microseconds) between falling edges that will be considered a valid encoder signal (high end of frequency range of pwm output)
         */
        uint16_t MAX_CYCLE;
    };

protected:
    byte encoderPin;

private:
    long turns;
    uint16_t angle;
    uint16_t lastAngle;
    uint16_t zeroPos;
    bool resetAngleOnStart;
    bool firstAngle;
    int8_t reverse;
    float distPerCountFactor;
    bool newSpeed;
    uint16_t lastVelAngle;
    long lastVelTurns;
    unsigned long lastVelTimeMicros;
    unsigned long velEnoughTime;
    unsigned long velEnoughTicks;
    float velocity;
    bool justStartedVel;
    volatile bool newAngle;
    volatile unsigned long dataStartMicros;
    volatile unsigned long dataEndMicros;
    volatile unsigned long earlyDataStartMicros;
    volatile unsigned long oldDataEndMicros;
    bool setPosZero;

    pwmSettings ps;

public:
    /**
     * @brief  sets pins and settings for reading the encoder
     * @param  _encoderPin: pin to read encoder signal with
     * @param  _ps: (struct pwmSettings) parameters of encoder signal
     * @param  _reverse: (bool) reverse positive direction, default=false 
     * @param  _distPerCountFactor: (float) for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder
     * @param  _velEnoughTime: (default=0, no limit) shortest interval (in MICROseconds) between velocity calculations, if run() is called faster the calculation will wait to run
     * @param  _velEnoughTicks: (default=0, no limit) if the encoder turns more than this number of steps velocity calculations will be done even if velEnoughTime hasn't been reached
     * @param  _resetAngleOnStart: (default=false) zero encoder angle on startup (in encoder units)
     * @param  _zeroPos: (default=0) change what angle of the absolute encoder is considered zero. set _resetAngleOnStart to false to keep setting from being overridden
     */
    JEncoderPWMAbsolute(byte _encoderPin, pwmSettings _ps, bool _reverse = false, float _distPerCountFactor = 1.0, unsigned long _velEnoughTime = 0, unsigned long _velEnoughTicks = 0, bool _resetAngleOnStart = false, uint16_t _zeroPos = 0)
        : ps(_ps)
    {
        firstAngle = true;
        resetAngleOnStart = _resetAngleOnStart;
        encoderPin = _encoderPin;
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
        setDistPerCountFactor(_distPerCountFactor);

        velEnoughTime = _velEnoughTime;
        velEnoughTicks = _velEnoughTicks;
        zeroPos = _zeroPos % ps.RESOLUTION;

        turns = 0;
        angle = 0;
        lastAngle = 0;
        newSpeed = false;
        lastVelAngle = 0;
        lastVelTurns = 0;
        lastVelTimeMicros = 0;
        velocity = 0.0;
        newAngle = false;
        dataStartMicros = 0;
        dataEndMicros = 0;
        earlyDataStartMicros = 0;
        oldDataEndMicros = 0;
        justStartedVel = true;
        setPosZero = false;
    }

    /**
     * @brief  set up pins and interrupts
     * @param  _isrPointer: global function that calls internal ISR
     */
    virtual void setUpInterrupts(void (*_isrPointer)(void));
    /**
     * @brief  disable interrupts and stop monitoring encoder
     * @note use setUpInterrupts to start encoder again
     */
    virtual void turnOffInterrupts();

    /** 
     * @note divide by RESOLUTION to get fraction of full turn
     * @retval the angle reading from the sensor [0, RESOLUTION) (but negative if reverse is true)
     */

    int rawReading()
    {
        return angle * reverse;
    }

    /**
     * @brief  how many full turns the encoder has made.
     * @note   this and rawReading can be used as an alternative to getPos if the limited precision of a float is a concern
     * @retval (long) how far the encoder has turned
     */
    long intTurns()
    {
        return turns * reverse;
    }

    long zeroCounter()
    {
        return zeroCounter(true);
    }

    /**
     * @brief  reset the zero point of the encoder
     * @param  resetAngle: (bool) true=set the next read angle as zero, false=reset turns, but leave absolute angle measurement as is
     * @retval  (long) old position (number of turns, not including angle!)
     */
    long zeroCounter(bool resetAngle)
    {
        long tTurns = turns;
        turns = 0;
        if (resetAngle) {
            setPosZero = true;
        }
        justStartedVel = true;
        return (tTurns * ps.RESOLUTION) * reverse;
    }

    float getVel()
    {
        return velocity;
    }

    long getCounter()
    {
        return (turns * ps.RESOLUTION + angle) * reverse;
    }

    float getPos()
    {
        return (turns * ps.RESOLUTION + angle) * reverse * distPerCountFactor;
    }

    float getDistPerCountFactor()
    {
        return distPerCountFactor;
    }

    /**
     * @note for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder.
     *      it is converted to actual counts within this function
     */
    void setDistPerCountFactor(float _factor)
    {
        distPerCountFactor = _factor / ps.RESOLUTION;
    }

    bool hasDirection()
    {
        return true;
    }

    bool isVelNew()
    {
        return newSpeed;
    }

    /**
     * @brief  change what angle of the absolute encoder is zero
     * @note  this will cause distance to jump some
     * @param  zeroAngle: [0,ps.RESOLUTION-1)
     * @retval  (bool) did the zeroAngle change
     */
    bool setZeroPos(uint16_t zeroAngle)
    {
        if (zeroAngle % ps.RESOLUTION != zeroPos) {
            zeroPos = zeroAngle % ps.RESOLUTION;
            return true;
        }
        return false;
    }

    /**
     * @note   call this function as frequently as possible or this code can't keep track of how many times the encoder has turned
     */
    void run()
    {
        if (newAngle) {
            noInterrupts();
            long interval = dataEndMicros - dataStartMicros;
            long cycle = dataEndMicros - oldDataEndMicros;
            newAngle = false;
            interrupts();
            if (interval > 0 && cycle > ps.MIN_CYCLE && cycle < ps.MAX_CYCLE) { //check that interrupts didn't change one number but not the other, and that frequency is within range
                if ((resetAngleOnStart && firstAngle) || setPosZero) {
                    zeroPos = 0;
                }
                angle = (constrain(interval * ps.PWM_STEPS / cycle - ps.MIN_STEPS, 0, ps.RESOLUTION - 1) + ps.RESOLUTION - zeroPos) % ps.RESOLUTION;
                if ((resetAngleOnStart && firstAngle) || setPosZero) {
                    zeroPos = (angle) % ps.RESOLUTION;
                    firstAngle = false;
                    lastAngle = 0;
                    setPosZero = false;
                    angle = 0;
                }
                if (abs((int16_t)angle - (int16_t)lastAngle) >= ps.RESOLUTION / 2) { // angle jump over half of circle is assummed to be the shorter crossing of 0
                    if (angle > lastAngle) {
                        turns--;
                    } else {
                        turns++;
                    }
                }
                lastAngle = angle;
            }
        }
        int64_t velDist = ((int16_t)angle - (int16_t)lastVelAngle) + (turns - lastVelTurns) * ps.RESOLUTION;
        if ((micros() - lastVelTimeMicros > velEnoughTime || abs(velDist) > velEnoughTicks)) {
            if (justStartedVel) {
                justStartedVel = false;
                lastVelAngle = angle;
                velDist = 0;
            }
            velocity = (double)1000000.0 * velDist / (micros() - lastVelTimeMicros) * distPerCountFactor * reverse;
            newSpeed = true;
            lastVelTimeMicros = micros();
            lastVelAngle = angle;
            lastVelTurns = turns;
        } else {
            newSpeed = false;
        }
    }
    void encoderISR()
    {
        if (digitalRead(encoderPin) == ps.dataState) {
            earlyDataStartMicros = micros();
        } else {
            oldDataEndMicros = dataEndMicros;
            dataEndMicros = micros();
            dataStartMicros = earlyDataStartMicros;
            newAngle = true;
        }
    }
};
#endif
