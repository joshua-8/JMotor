#ifndef J_MOTOR_CONTROLLER_H
#define J_MOTOR_CONTROLLER_H
#include "JMotorControllerBase.h"
// #include <Arduino.h>
// /**
//  * @brief  This class defines a common interface for classes which control velocity and position of a motor controlled by a JMotorDriver
//  */
// class JMotorController : public JMotorControllerBase {
// protected:
//     /**
//      * @brief  constructor
//      * @param  _driver: reference to JMotorDriver
//      */
//     JMotorController(JMotorDriver& _driver, JMotorCompensator& _compensator)
//         : JMotorControllerBase(_driver, _compensator)
//     {
//         posMode = false;
//         position = 0;
//         positionTarget = 0;
//         velLimit = 0;
//     }

// public:
//     /**
//      * @brief  set target position
//      * @note this function can be called repeatedly, or just once if run() is called frequently
//      * @param  _posTarget: (float) target position
//      * @param _startPos: (float) reset position to this value (default:0)
//      * @retval (bool) did the  target change
//      */
//     virtual bool setPosTarget(float _posTarget, float _startPos = 0);
//     /**
//      * @brief  get position set as target
//      * @retval (float)
//      */
//     float getPosTarget()
//     {
//         return positionTarget;
//     }
//     /**
//      * @brief  get what position the motor is currently at
//      * @retval (float)
//      */
//     float getPos()
//     {
//         return position;
//     }
//     /**
//      * @brief  reset what position the controller thinks it's in
//      * @note  make sure to use this frequently enough that the limited precision of floats doesn't become a problem. don't drive too far without resetting
//      * @param  pos: (float) value to reset position to, default: 0
//      * @retval (float) returns old position
//      */
//     float resetPos(float pos = 0)
//     {
//         float tempPos = position;
//         position = pos;
//         return tempPos;
//     }
//     /**
//      * @brief  true if driving to target position, false if under velocity control
//      * @retval (bool)
//      */
//     bool positioningMode()
//     {
//         return posMode;
//     }

//     /**
//      * @brief  How fast of a motor speed setting would get adjusted to full motor power
//      * @note   may change as battery voltage changes
//      * @retval  (float)
//      */
//     float getMaxVel()
//     {
//         return compensator.getMaxVel();
//     }
// };
// #include "JMotorControllerClosed.h"
// #include "JMotorControllerOpen.h"
#endif
