// fully implement JMotorController
// provide access to encoder

// public:
//     /**
//      * @brief  set target position
//      * @note this function can be called repeatedly, or just once if run() is called frequently
//      * @param  pt: (float) target position
//      * @retval (bool) did the  target change
//      */
//     virtual bool setPosTarget(float pt);
//     /**
//      * @brief  get position set as target
//      * @retval (float)
//      */
//     virtual float getPosTarget();
//     /**
//      * @brief  get what position the motor is currently at
//      * @retval (float)
//      */
//     virtual float getPos();
//     /**
//      * @brief  reset what position the controller thinks it's in
//      * @note  make sure to use this frequently enough that the limited precision of floats doesn't become a problem. don't drive too far without resetting
//      * @param  pos: (float) value to reset position to, default: 0
//      * @retval (float) returns old position
//      */
//     virtual float resetPos(float pos = 0);
//     /**
//      * @brief  true if driving to target position, false if under velocity control
//      * @retval (bool)
//      */
