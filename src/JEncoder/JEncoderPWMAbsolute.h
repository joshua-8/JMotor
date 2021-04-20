/**
 * @brief  reads a PWM signal from an absolute encoder using attachInterrupt()
 * tested with https://ams.com/as0548b 
 * @note  platform: esp32, teensy \n
 *          (could be used with standard avr arduinos but only with pins that support attachInterrupt())
 */

#ifndef J_ENCODER_ATTACH_INTERRUPT_PWM_ABSOLUTE_H
#define J_ENCODER_ATTACH_INTERRUPT_PWM_ABSOLUTE_H
#include "JEncoder.h"
#include <Arduino.h>

#define JEncoderPWMAbsolute_AS5048settings \
    {                                      \
        4095, 4119, 16, HIGH               \
    }
#define JEncoderPWMAbsolute_MA310bitsettings \
    {                                        \
        1023, 1026, 1, HIGH                  \
    } //untested
#define JEncoderPWMAbsolute_MA312bitsettings \
    {                                        \
        4095, 4098, 1, HIGH                  \
    } //untested

class JEncoderPWMAbsolute : public JEncoder {
public:
    struct pwmSettings {
        uint16_t RESOLUTION; //resolution of encoder data (pwm steps)
        uint16_t PWM_STEPS; //number of total clock steps in pwm output
        uint16_t MIN_STEPS; //number of steps corresponding to min angle. MAX_STEPS=MIN_STEPS+RESOLUTION
        boolean dataState; //HIGH or LOW, which state when pulse length increasing=increasing angle
    };

protected:
    byte encoderPin;

private:
    long turns;
    uint16_t angle;
    uint16_t lastAngle;
    int8_t reverse;
    float distPerCountFactor;
    boolean newSpeed;
    uint16_t lastVelAngle;
    long lastVelTurns;
    unsigned long lastVelTimeMicros;
    unsigned long velEnoughTime;
    unsigned long velEnoughTicks;
    float velocity;
    boolean newAngle;

    unsigned long dataStartMicros;
    unsigned long dataEndMicros;
    unsigned long earlyDataStartMicros;
    unsigned long oldDataEndMicros;

    pwmSettings ps;

public:
    /* /def
    * Functions called by interrupts can't expect parameters or be functions of classes so a workaround is needed to use interrupts in a class.
    * The workaround that's used here is global functions need to be made when a new instance of this class is made.
    * The global function get used when attaching interrupts, and the global function can call a function inside the class.
    * The following macro makes it easy to make the global function.
    */
#define jENCODER_PWMABS_MAKE_ISR_MACRO(name) \
    void name##_jENCODER_ISR() { name.encoderISR(); }

    /**
     * @brief  sets pins and settings for reading the encoder
     * @param  _encoderPin: pin to read encoder signal with
     * @param  _ps: (struct pwmSettings) parameters of encoder signal
     * @param  _reverse: (boolean) reverse positive direction, default=false 
     * @param  _distPerCountFactor: (float) for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder
     * @param  _velEnoughTime: (default=0, no limit) shortest interval between velocity calculations, if run() is called faster the calculation will wait to run
     * @param  _velEnoughTicks: (default=0, no limit) if the encoder turns more than this number of steps velocity calculations will be done even if velEnoughTime hasn't been reached
     */
    JEncoderPWMAbsolute(byte _encoderPin, pwmSettings _ps, boolean _reverse = false, float _distPerCountFactor = 1.0, unsigned long _velEnoughTime = 0, unsigned long _velEnoughTicks = 0)
    {
        encoderPin = _encoderPin;
        ps = _ps;
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
        setDistPerCountFactor(_distPerCountFactor);

        velEnoughTime = _velEnoughTime;
        velEnoughTicks = _velEnoughTicks;

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
     * @note divide by STEPS_PER_TURN to get fraction of full turn
     * @retval the raw angle reading from the sensor (0 to 16,383) (but negative if reverse is true)
     */

    int rawReading()
    {
        return angle * reverse;
    }

    /**
     * @brief  how many full turns the encoder has made.
     * @note   this and rawReading can be used as an alternative to getDist if the limited precision of a float is a concert
     * @retval (long) how far the encoder has turned
     */
    long intTurns()
    {
        return turns * reverse;
    }

    long zeroCounter()
    {
        long tTurns = turns;
        uint16_t tAngle = angle;
        turns = 0;
        angle = 0;
        return (tTurns * ps.RESOLUTION + tAngle) * reverse;
    }

    float getVel()
    {
        return velocity;
    }

    long getCounter()
    {
        return (turns * ps.RESOLUTION + angle) * reverse;
    }

    float getDist()
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

    boolean hasDirection()
    {
        return true;
    }

    boolean isVelNew()
    {
        return newSpeed;
    }

    /**
     * @note   call this function as frequently as possible or this code can't keep track of how many times the encoder has turned
     */
    void run()
    {
        if (newAngle) {
            long interval = dataEndMicros - dataStartMicros;
            long cycle = dataEndMicros - oldDataEndMicros;
            if (interval > 0 && cycle > 0) { //check that interrupts didn't change one number but not the other
                angle = constrain(interval * ps.PWM_STEPS / cycle - ps.MIN_STEPS, 0, ps.RESOLUTION);
                newAngle = false;
                if (abs(angle - lastAngle) > ps.RESOLUTION / 2) { // angle jump over half of circle is assummed to be the shorter crossing of 0
                    if (angle > lastAngle) {
                        turns--;
                    } else {
                        turns++;
                    }
                }
            }
        }
        long velDist = (angle - lastVelAngle) + (turns - lastVelTurns) * ps.RESOLUTION;
        if (micros() - lastVelTimeMicros > velEnoughTime || abs(velDist) > velEnoughTicks) {
            velocity = (double)1000000.0 * velDist / (micros() - lastVelTimeMicros) * distPerCountFactor * reverse;
            lastVelTimeMicros = micros();
            newSpeed = true;
            lastVelAngle = angle;
            lastVelTurns = turns;
        } else {
            newSpeed = false;
        }

        lastAngle = angle;
    }
    void encoderISR()
    {
        if (digitalRead(encoderPin) == ps.dataState) {
            earlyDataStartMicros = micros();
        } else {
            oldDataEndMicros = dataEndMicros;
            dataEndMicros = micros();
            dataStartMicros = earlyDataStartMicros;
        }
        newAngle = true;
    }
};
#endif
