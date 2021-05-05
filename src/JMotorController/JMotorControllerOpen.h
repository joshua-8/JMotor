// #ifndef J_MOTOR_CONTROLLER_OPEN_H
// #define J_MOTOR_CONTROLLER_OPEN_H
// #include "Derivs_Limiter.h"
// #include "JMotorController.h"
// #include "JMotorControllerBasicOpen.h"
// #include <Arduino.h>
// class JMotorControllerOpen : public JMotorController, private JMotorControllerBasicOpen {
// protected:
//     Derivs_Limiter dL;
//     bool posMode;
//     float position;
//     float positionTarget;
//     float velLimit;

// public:
//     JMotorControllerOpen(JMotorDriver& _driver, JMotorCompensator& _compensator, float _velLimit = 0, float _accelLimit = 0)
//         : JMotorController(_driver, _compensator)
//         , JMotorControllerBasicOpen(_driver, _compensator, _accelLimit)
//         , dL(Derivs_Limiter(_velLimit, _accelLimit))
//     {
//         positionTarget = 0;
//         velLimit = max(_velLimit, (float)0.0);
//     }
//     void setVel(float vel)
//     {
//         posMode = false;
//         JMotorControllerBasicOpen::setVel(vel);
//     }
//     float setVelTarget(float vel)
//     {
//         posMode = false;
//         return JMotorControllerBasicOpen::setVelTarget(vel);
//     }

//     /**
//      * @brief  set target position
//      * @note this function can be called repeatedly, or just once if run() is called frequently
//      * @param  _posTarget: (float) target position
//      * @retval (bool) did the target change
//      */
//     bool setPosTarget(float _posTarget, float _startPos = 0)
//     {
//         resetPos(_startPos);
//         posMode = true;
//         if (positionTarget != _posTarget) {
//             positionTarget = _posTarget;
//             run();
//             return true;
//         }
//         run();
//         return false;
//     }

//     /**
//      * @brief  set maximum motor speed
//      * @note   set to INFINITY to disable acceleration limiting
//      * @param  _accelLimit: (float)
//      */
//     void setVelLimit(float _velLimit)
//     {
//         velLimit = max(_velLimit, (float)0.0);
//         dL.setVelLimit(velLimit);
//     }

//     void setAccelLimit(float _accelLimit)
//     {
//         JMotorControllerBasicOpen::setAccelLimit(_accelLimit);
//         dL.setAccelLimit(accelLimit);
//     }
//     /**
//      * @brief  set velocity and acceleration limits for motor
//      * @param  _vel: 
//      * @param  _accel: 
//      * @retval None
//      */
//     void setVelAccelLimits(float _vel, float _accel)
//     {
//         setVelLimit(_vel);
//         setAccelLimit(_accel);
//     }

// protected:
//     void run()
//     {
//         if (posMode) {
//             dL.setVelocity(velocity);
//             dL.setVelLimit(compensator.getMaxVel());
//             dL.calc(positionTarget);
//             JMotorControllerBasicOpen::setVelTarget(dL.getVelocity());
//         }
//         position += velocity * (micros() - lastRunMicros) / 1000000.0;
//         JMotorControllerBasicOpen::run();
//     }
// };
// #endif
