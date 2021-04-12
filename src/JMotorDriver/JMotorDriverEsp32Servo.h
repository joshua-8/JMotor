#ifndef J_MOTOR_DRIVER_ESP32_SERVO_H
#define J_MOTOR_DRIVER_ESP32_SERVO_H
#include "JMotorDriver.h"
/**
 * @brief  
 * @note   
 */
class JMotorDriverEsp32Servo : private JMotorDriver {
private:
    boolean enabled = false;
    int servoPin;
    int pwmChannel;
    float SERVO_TICKS_PER_MICROSECOND = 52.4288;
    int SERVO_FREQ = 50;
    int SERVO_RES = 20;

public:
    int minServoValue = 544;
    int maxServoValue = 2400;

    JMotorDriverEsp32Servo(int _pwmChannel, int _servoPin)
    {
        enabled = false;
        servoPin = _servoPin;
        pwmChannel = _pwmChannel;
        setFrequencyAndResolution();
    }
    JMotorDriverEsp32Servo(int _pwmChannel, int _servoPin, int _freq, int _resBits)
    {
        enabled = false;
        servoPin = _servoPin;
        pwmChannel = _pwmChannel;
        setFrequencyAndResolution(_freq, _resBits);
    }
    /**
     * @brief  
     * @param  freq: Hz (default 50) must be <= int(80E6 / 2^resBits)
     * @param  resBits: (default 20)
     * @retval (float) returns PWM cycles per microsecond-used in ledcWrite call, returned for debugging purposes
     */
    float setFrequencyAndResolution(int freq = 50, int resBits = 20)
    {
        //TODO: verify values?
        SERVO_FREQ = freq;
        SERVO_RES = resBits;
        SERVO_TICKS_PER_MICROSECOND = (1 << SERVO_RES) * SERVO_FREQ / 1000000; //DEFAULT=52.4288  2^SERVO_RES / 1E6 * SERVO_FREQ
        ledcSetup(pwmChannel, SERVO_FREQ, SERVO_RES);
        return SERVO_TICKS_PER_MICROSECOND;
    }
    JMotorDriverType getType()
    {
        return JMotorDriverType::espServo;
    }
    /**
     * @brief  
     * @note   
     * @param  _val: 
     * @retval true if value at end of range
     */
    boolean set(float _val)
    {
        if (enabled) {
            ledcWrite(pwmChannel, SERVO_TICKS_PER_MICROSECOND * ((maxServoValue + minServoValue) / 2 + (maxServoValue - minServoValue) * constrain(_val, -1.0, 1.0) / 2));
        }
        return abs(_val) >= 1.0;
    }
    /**
     * @brief  
    * @note   
    * @param  enable: 
    * @retval returns true if state changed, false otherwise
    */
    boolean setEnable(boolean enable)
    {
        if (enable) {
            if (!enabled) {
                //actually enable
                enabled = true;
                pinMode(servoPin, OUTPUT);
                ledcSetup(pwmChannel, SERVO_FREQ, SERVO_RES);
                ledcAttachPin(servoPin, pwmChannel);
                return true;
            }
        } else { //disable
            if (enabled) {
                //actually disable
                enabled = false;
                ledcDetachPin(servoPin);
                //TODO: avoid cutting pulses short?
                pinMode(servoPin, OUTPUT);
                digitalWrite(servoPin, LOW);
                return true;
            }
        }
        return false;
    }

    boolean getEnable()
    {
        return enabled;
    }
    float getMaxRange()
    {
        return 1.0;
    }
    float getMinRange()
    {
        return -1.0;
    }
};
#endif