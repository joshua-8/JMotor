#ifndef J_MOTOR_LIB_H
#define J_MOTOR_LIB_H
#include "JMotorDriver/JMotorDriver.h"

#ifdef ESP32
#include "JMotor/JMotorDriver/JMotorDriverEsp32L293d.h"
#include "JMotor/JMotorDriver/JMotorDriverEsp32PWM.h"
#include "JMotor/JMotorDriver/JMotorDriverEsp32Servo.h"
#else
#include "JMotor/JMotorDriver/JMotorDriverAvrL293D.h"
#include "JMotor/JMotorDriver/JMotorDriverAvrPWM.h"
#include "JMotor/JMotorDriver/JMotorDriverAvrServo.h"
#endif

#endif