#ifndef J_MOTOR_DRIVER_SERVO_ADVANCED_H
#define J_MOTOR_DRIVER_SERVO_ADVANCED_H
#include "JMotorDriverServo.h"
#include <Arduino.h>
/**
 * @brief  defines interface for servo driver with variable frequency, subclass of JMotorDriverServo and JMotorDriver
 */
class JMotorDriverServoAdvanced : public JMotorDriverServo {
public:
    /**
     * @brief  adjust frequency as a fraction of standard 50hz frequency
     * lower frequencies (try .75) may let you reduce how strongly the servo holds its position
     * @note   different servos may respond differently, possibly including being damaged, so use at your own risk!
     * @param  freq: (float) freq*50=setFrequency
     */
    virtual void adjustFrequency(float freq) = 0;
};
#endif
