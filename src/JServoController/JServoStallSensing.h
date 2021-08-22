#ifndef J_SERVO_STALL_SENSING_H
#define J_SERVO_STALL_SENSING_H
#include <Arduino.h>
/**
 * @brief  defines interface for classes that can measure how hard a servo is working
 */
class JServoStallSensing {
public:
    virtual float getMeasurement(bool _run = true) = 0;
    virtual void run() = 0;
};
#endif
