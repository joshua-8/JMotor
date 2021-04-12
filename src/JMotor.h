/*! \mainpage
 * JMotor is a library for controlling motors
 * 
 */

#ifndef J_MOTOR_LIB_H
#define J_MOTOR_LIB_H
#include "JMotorDriver/JMotorDriver.h"

#ifdef ESP32
#include "JMotorDriver/JMotorDriverEsp32L293d.h"
#include "JMotorDriver/JMotorDriverEsp32PWM.h"
#include "JMotorDriver/JMotorDriverEsp32PWMDir.h"
#include "JMotorDriver/JMotorDriverEsp32Servo.h"
#else
#include "JMotorDriver/JMotorDriverAvrL293D.h"
#include "JMotorDriver/JMotorDriverAvrPWM.h"
#include "JMotorDriver/JMotorDriverAvrPWMDir.h"
#include "JMotorDriver/JMotorDriverAvrServo.h"
#endif

#endif