/*! \mainpage
 * JMotor library \n
 * github repository: https://github.com/joshua-8/JMotor \n
 * reference docs: https://joshua-8.github.io/JMotor/md_readme.html \n
 * supports the following boards:   ESP32, Arduino AVR (tested on nano), Teensy (tested on LC), SAMD M0 boards (tested on feather m0)
 */

#ifndef J_MOTOR_LIB_H
#define J_MOTOR_LIB_H
#include "Derivs_Limiter.h" // https://github.com/joshua-8/Derivs_Limiter/releases
#include "JDrivetrain/JDrivetrain.h"
#include "JDrivetrainController/JDrivetrainController.h"
#include "JEncoder/JEncoder.h"
#include "JMotorCompensator/JDeadzoneRemover.h"
#include "JMotorCompensator/JMotorCompensator.h"
#include "JMotorController/JMotorController.h"
#include "JMotorDriver/JMotorDriver.h"
#include "JServoController/JServoController.h"
#include "JTwoDTransform.h"
#endif
