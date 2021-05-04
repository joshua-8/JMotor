#ifndef J_VOLTAGE_COMP_MEASURE_H
#define J_VOLTAGE_COMP_MEASURE_H
#include "JVoltageCompensator.h"
#include <Arduino.h>

/**
 * @note  N (the value in the template) is how many measurements to average together to smooth voltage readings
 */
template <byte N>
class JVoltageCompMeasure : public JVoltageCompensator {
private:
    byte measurePin;
    float supplyVoltage;
    /**
     * until N readings have been made, supplyVoltage is just the instantaneous measurement
     */
    boolean justStarted;
    int readings[N]; // the readings from the analog input
    byte readIndex; // the index of the current reading
    long total; // the running total
    float DACUnitsPerVolt;
    unsigned long lastMeasurementMillis;
    int measurementInterval;

public:
    /**
     * @brief  constructor
     * @param  _measurePin: pin to use to measure battery voltage (you probably want to make a voltage divider with a couple resistors)
     * @param  _DACUnitsPerVolt:  the value read through analogRead is divided by this value to get voltage
     * @param  _measurementIntervalMillis:  wait this many milliseconds before taking another reading (default: 10)
     */
    JVoltageCompMeasure(byte _measurePin, float _DACUnitsPerVolt, float _driverRange = 1.0, int _measurementIntervalMillis = 10)
        : JVoltageCompensator(_driverRange)
    {
        measurePin = _measurePin;
        supplyVoltage = 10;
        justStarted = true;
        total = 0;
        readIndex = 0;
        DACUnitsPerVolt = _DACUnitsPerVolt;
        lastMeasurementMillis = 0;
        measurementInterval = _measurementIntervalMillis;
    }
    float adjust(float voltage)
    {
        if (millis() - lastMeasurementMillis > measurementInterval) {
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
            if (readIndex >= N) {
                justStarted = false;
                // ...wrap around to the beginning:
                readIndex = 0;
            }
            if (!justStarted) {
                // calculate the average:
                supplyVoltage = 1.0 / DACUnitsPerVolt * total / N;
            } else {
                supplyVoltage = 1.0 / DACUnitsPerVolt * readings[readIndex - 1];
            }
        }
        return voltage / supplyVoltage * driverRange;
    }
    float getSupplyVoltage()
    {
        return supplyVoltage;
    }
};
#endif
