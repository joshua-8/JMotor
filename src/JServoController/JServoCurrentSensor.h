#ifndef J_SERVO_CURRENT_SENSOR_H
#define J_SERVO_CURRENT_SENSOR_H
#include "JServoStallSensing.h"
#include <Arduino.h>
/**
 * @brief  implements JServoStallSensing interface. takes and filters current readings to calculate how hard a servo is working
 * add a small amount of resistance to the negative power wire of a servo (a small bundle of wire can create enough resistance)
 * connect the measuring pin to the negative wire close to the servo so the voltage drop across the resistance can be measured.
 */
class JServoCurrentSensor : public JServoStallSensing {
protected:
    int measurePin;
    bool justStarted;
    static const byte numReadings = 20;
    int readings[numReadings]; // the readings from the analog input
    byte readIndex; // the index of the current reading
    long total; // the running total
    unsigned long lastMeasurementMillis;
    long measurementRaw;
    int minRange;
    int maxRange;

public:
    /**
     * @brief  
     * @note   
     * @param  _measurePin: 
     * @param  _maxRange: 
     * @param  _minRange: 
     * @retval 
     */
    JServoCurrentSensor(int _measurePin, int _maxRange = 1000, int _minRange = 0)
    {
        measurePin = _measurePin;
        measurementRaw = 0;
        justStarted = true;
        total = 0;
        readIndex = 0;
        lastMeasurementMillis = 0;
        minRange = _minRange;
        maxRange = _maxRange;
    }
    float getMeasurement(bool _run = true)
    {
        if (_run) {
            run();
        }
        return constrain(map(measurementRaw, (long)minRange * numReadings, (long)maxRange * numReadings, 0, 10000), 0, 10000) / 10000.0;
    }
    int getUnscaledMeasurement()
    {
        return measurementRaw;
    }
    bool getJustStarted()
    {
        return justStarted;
    }
    int getMinRange()
    {
        return minRange;
    }
    int getMaxRange()
    {
        return maxRange;
    }
    void setMinRange(int min)
    {
        minRange = min;
    }
    void setMaxRange(int max)
    {
        maxRange = max;
    }
    void setMeasurementPin(byte _newPin)
    {
        measurePin = _newPin;
    }
    byte getMeasurementPin()
    {
        return measurePin;
    }
    void run()
    {
        if (millis() - lastMeasurementMillis > 4) {
            lastMeasurementMillis = millis();
            if (!justStarted) {
                // subtract the last reading:
                total = total - readings[readIndex];
            }
            // read from the sensor:
            readings[readIndex] = analogRead(measurePin);
            // add the reading to the total:
            total = total + readings[readIndex];
            // advance to the next position in the array:
            readIndex = readIndex + 1;

            // if we're at the end of the array...
            if (readIndex >= numReadings) {
                justStarted = false;
                // ...wrap around to the beginning:
                readIndex = 0;
            }
            if (!justStarted) {
                // calculate the average:
                measurementRaw = total;
            } else {
                measurementRaw = 0;
            }
        }
    }
};
#endif
