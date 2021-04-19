#include "JMotor.h"
#include <Arduino.h>
JEncoderSingleAttachInterrupt enc = JEncoderSingleAttachInterrupt(1, 2);
int readEncoder(JEncoder encoder)
{
    return encoder.getDist();
}

void setup()
{
    Serial.println(readEncoder(enc));
}

void loop()
{
}
