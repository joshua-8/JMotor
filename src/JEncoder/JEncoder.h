/**
 * @brief  defines common interface for JEncoder
 * @note   JEcoder's functions are virtual, it can not be used directly
 */
#ifndef J_ENCODER_H
#define J_ENCODER_H
#include <Arduino.h>
//**********SEE BOTTOM OF THIS FILE FOR #INCLUDES OF SUBCLASSES**********//

class JEncoder {
public:
    /**
     * @brief  reset the counter of how far the encoder has turned
     * @retval (long) returns value of counter before it is reset
     */
    virtual long zeroCounter();

    /**
     * @brief  calculates velocity in distance per second where distance was set by setCountsToDistFactor()
     * @retval (float) velocity
     */
    virtual float getVel();

    /**
     * @brief  returns how far the encoder has turned from the zero position
     * @note  remember that variables overflow
     * @retval (long) encoder ticks
     */
    virtual long getCounter();

    /**
     * @brief  returns how far the encoder has turned from the zero position converted to distance
     * @note   remember that variables overflow (counter) and that floats have limited precision
     * @retval (float) encoder ticks converted to distance
     */
    virtual float getDist();

    /**
     * @brief  returns a conversion factor between encoder ticks and distance that can be set for the encoder
     * @note default is 1.0
     * @retval (float) distPerCountFactor
     */
    virtual float getDistPerCountFactor();

    /**
     * @brief  set the conversion factor between encoder ticks and distance
     * @param  _factor: (float)
     */
    virtual void setDistPerCountFactor(float _factor);

    /**
     * @brief  can this encoder mesure direction or just speed
     * @retval (boolean) true = can measure direction
     */
    virtual boolean hasDirection();

    /**
     * @brief  could be useful for only recalculating a control loop if there's new velocity data
     * @retval (boolean) true if velocity has changed since this function was last called
     */
    virtual boolean isVelNew();

    /**
     * @brief  if an encoder needs to have some code called each loop (like absolute encoder polling encoder and calculating amount turned)
     * @note   interrupt based encoders don't need this to do anything and can define an empty function
     */
    virtual void run();
};

#if defined(ESP32) || defined(CORE_TEENSY)
#include "JEncoderAttachInterruptQuadrature.h"
#include "JEncoderAttachInterruptSingle.h"
#else
#include "JEncoderPinChangeQuadrature.h"
#include "JEncoderPinChangeSingle.h"
#endif
#include "JEncoderAS5048bI2C.h"

#endif