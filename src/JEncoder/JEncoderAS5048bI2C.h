/**
 * @brief reads a type of absolute encoder https://ams.com/as0548b (uses I2C) \n
 * based on work by sosandroid https://github.com/sosandroid/AMS_AS5048B 
 * @note remember to add pull up resistors on I2C lines (~4.7k for 5v, ~2.4k for 3.3v microcontrollers). \n
 * Use #define J_ENCODER_AS5048_I2C_USE_SOFTWIRE to use SoftWire
 */
#ifndef J_ENCODER_AS5048B_I2C_H
#define J_ENCODER_AS5048B_I2C_H
#include "JEncoder.h"
#include <Arduino.h>
#ifdef J_ENCODER_AS5048_I2C_USE_SOFTWIRE
#include <SoftWire.h>
#else
#include <Wire.h>
#endif
class JEncoderAS5048bI2C : private JEncoder {
private:
#ifdef J_ENCODER_AS5048_I2C_USE_SOFTWIRE
    SoftWire* wire;
#else
    TwoWire* wire;
#endif

    byte address;
    long turns;
    uint16_t angle;
    int8_t reverse;
    float velocity;
    float distPerCountFactor;
    boolean newSpeed;
    uint16_t lastAngle;
    uint16_t lastVelAngle;
    long lastVelTurns;
    unsigned long lastVelTimeMicros;
    unsigned long velEnoughTime;
    unsigned long velEnoughTicks;

    const byte AS5048B_ZEROMSB_REG = 0x16;
    const byte AS5048B_ZEROLSB_REG = 0x17;
    const byte AS5048B_ADDR_REG = 0x15;
    const byte AS5048B_ANGLMSB_REG = 0xFE;
    const byte AS5048B_GAIN_REG = 0xFA;

    void writeRegister8(uint8_t reg, uint8_t value)
    {
        wire->beginTransmission(address);
        wire->write(reg);
        wire->write(value);
        wire->endTransmission();
    }

    uint16_t readRegister14(uint8_t reg)
    {
        //16 bit value got from 2 8bits registers (7..0 MSB + 5..0 LSB) => 14 bits value
        uint16_t readValue = 0;
        byte readArray[2];

        wire->beginTransmission(address);
        wire->write(reg);
        wire->endTransmission(false);

        wire->requestFrom(address, (uint8_t)2);
        readArray[0] = wire->read();
        readArray[1] = wire->read();

        readValue = (((uint16_t)readArray[0]) << 6);
        readValue += (readArray[1] & 0b111111);
        return readValue;
    }

    uint8_t readRegister8(uint8_t reg)
    {
        uint8_t readValue;

        wire->beginTransmission(address);
        wire->write(reg);
        wire->endTransmission(false);

        wire->requestFrom(address, (uint8_t)1);
        readValue = (uint8_t)wire->read();

        return readValue;
    }

    void writeToZeroRegister(uint16_t val)
    {
        writeRegister8(AS5048B_ZEROMSB_REG, (uint8_t)(val >> 6));
        writeRegister8(AS5048B_ZEROLSB_REG, (uint8_t)(val & 0b00111111));
    }

    /**
    * @note   bad if 255 (magnet too far) or 0 (magnet too close)
    */
    uint8_t getAutoGain()
    {
        return readRegister8(AS5048B_GAIN_REG);
    }

    uint16_t readAngle()
    {
        return readRegister14(AS5048B_ANGLMSB_REG);
    }

public:
    const unsigned int STEPS_PER_TURN = 16384;
    static const byte AS5048B_DEFAULT_ADDRESS = 0x40;

    /**
     * @brief  sets pins and settings for reading the encoder
     * @param  _reverse: (boolean) reverse positive direction, default=false 
     * @param  _distPerCountFactor: (float) for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder
     * @param  _address: (byte) I2C address of 
     * @param  _velEnoughTime: (default=0, no limit) shortest interval between velocity calculations, if run() is called faster the calculation will wait to run
     * @param  _velEnoughTicks: (default=0, no limit) if the encoder turns more than this number of steps velocity calculations will be done even if velEnoughTime hasn't been reached
     */
    JEncoderAS5048bI2C(boolean _reverse = false, float _distPerCountFactor = 1.0, uint8_t _address = 0x40, unsigned long _velEnoughTime = 0, unsigned long _velEnoughTicks = 0)
        : address(_address)
    {
        if (_reverse) {
            reverse = -1;
        } else {
            reverse = 1;
        }
        turns = 0;
        angle = 0;
        velocity = 0;
        setDistPerCountFactor(_distPerCountFactor);
        newSpeed = false;
        lastVelAngle = 0;
        lastVelTurns = 0;
        lastVelTimeMicros = 0;
        velEnoughTime = _velEnoughTime;
        velEnoughTicks = _velEnoughTicks;
    }

/**
     * @brief  set up communication with encoder
     * @note  call Wire.begin() yourself before using this function
     * @param  _wire: TwoWire or SoftWire (I2C) object to use for communicating
     */
#ifdef J_ENCODER_AS5048_I2C_USE_SOFTWIRE
    void begin(SoftWire* _wire)
#else
    void begin(TwoWire* _wire)
#endif
    {
        wire = _wire;
    }
    /**
     * @brief  set up communication with encoder
     * @note   uses default Wire object, not available if J_ENCODER_AS5048_I2C_USE_SOFTWIRE is defined
     * @param begin: true(default) call wire.begin(), false means call it yourself before using this function
     */

#ifndef J_ENCODER_AS5048_I2C_USE_SOFTWIRE
    void begin(boolean begin = true)
    {
        wire = &Wire;
        if (begin) {
            wire->begin();
        }
    }
#endif

    /**
     * @brief  communication is done over I2C and requires constant polling instead of being able to use interrupts
     * @note   call this function as frequently as possible or this code can't keep track of how many times the encoder has turned
     */
    void run()
    {
        angle = readAngle();
        if (abs(angle - lastAngle) > STEPS_PER_TURN / 2) { // angle jump over half of circle is assummed to be the shorter crossing of 0
            if (angle > lastAngle) {
                turns--;
            } else {
                turns++;
            }
        }

        long velDist = (angle - lastVelAngle) + (turns - lastVelTurns) * STEPS_PER_TURN;
        if (micros() - lastVelTimeMicros > velEnoughTime || abs(velDist) > velEnoughTicks) {
            velocity = (double)1000000.0 * velDist / (micros() - lastVelTimeMicros) * distPerCountFactor * reverse;
            lastVelTimeMicros = micros();
            newSpeed = true;
            lastVelAngle = angle;
            lastVelTurns = turns;
        } else {
            newSpeed = false;
        }

        lastAngle = angle;
    }

    /**
     * @brief  change the i2c address of the encoder (see the datasheet https://ams.com/as0548b)
     *      from addressRegW() in https://github.com/sosandroid/AMS_AS5048B/blob/master/ams_as5048b.cpp
     * @note you can also get 4 different addresses by setting the A1 and A2 pins
     * @param  val: 
     * @retval new address
     */
    uint8_t setAddressRegister(uint8_t val)
    {
        writeRegister8(AS5048B_ADDR_REG, val);
        // update our chip address with our 5 programmable bits
        // the MSB is internally inverted, so we flip the leftmost bit
        address = ((val << 2) | (address & 0b11)) ^ (1 << 6);
        return address;
    }

    /**
     * @brief  The current angle of the sensor is read and set to zero.
     */
    void setEncoderZero()
    {
        writeToZeroRegister(0);
        uint16_t newZero = readRegister14(AS5048B_ANGLMSB_REG);
        writeToZeroRegister(newZero);
    }
    /**
     * @brief  A custom (repeatable) angle can be set for what the sensor calls zero relative to default zero.
     * @param zeroAngle (int) [0, STEPS_PER_TURN)
     */
    void setEncoderZero(int zeroAngle)
    {
        zeroAngle = constrain(zeroAngle, 0, (long)STEPS_PER_TURN - 1);
        writeToZeroRegister(zeroAngle);
    }

    /**
     * @retval the raw angle reading from the sensor (0 to 16,383) (but negative if reverse is true)
     * @note divide by STEPS_PER_TURN to get fraction of full turn
     */

    int rawReading()
    {
        return angle * reverse;
    }

    /**
     * @brief  is the magnet in the optimal position
     *          Unlike other functions, this function does not rely on run()
     * @note  if this is false, the encoder may still be providing angle information
     * @retval  (boolean) 
     */
    boolean isMagnetInRange()
    {
        uint8_t gain = getAutoGain();
        return (gain != 0) && (gain != 255);
    }

    /**
     * @brief  how many full turns the encoder has made.
     * @note   this and rawReading can be used as an alternative to getDist if the limited precision of a float is a concert
     * @retval (long) how far the encoder has turned
     */
    long intTurns()
    {
        return turns * reverse;
    }

    long zeroCounter()
    {
        long tempTurns = turns * reverse;
        turns = 0;
        angle = 0;
        return tempTurns;
    }

    float getVel()
    {
        return velocity * reverse;
    }

    long getCounter()
    {
        return (turns * STEPS_PER_TURN + angle) * reverse;
    }

    float getDist()
    {
        return (turns * STEPS_PER_TURN + angle) * reverse * distPerCountFactor;
    }

    float getDistPerCountFactor()
    {
        return distPerCountFactor;
    }

    /**
     * @note for the purposes of setting this factor a "count" is considered a full revolution of the absolute encoder
     *      it is converted to actual counts within this function
     */
    void setDistPerCountFactor(float _factor)
    {
        distPerCountFactor = _factor / STEPS_PER_TURN;
    }

    boolean hasDirection()
    {
        return true;
    }

    boolean isVelNew()
    {
        return newSpeed;
    }
};
#endif
