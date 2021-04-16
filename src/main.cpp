#include "JMotor.h"
#include <Arduino.h>
JEncoderPinChangeQuadrature encoder(7, 5, 0.25, 500000, false);
ENCODER_PCQ_MAKE_ISRS_MACRO(encoder);

void setup()
{
    Serial.begin(9600);
    encoder.setUpInterrupts(encoder_jENCODER_ISR_A, encoder_jENCODER_ISR_B);
}

void loop()
{
    // Serial.println(encoder.getDist());
    if (encoder.isVelNew()) {
        Serial.println(encoder.getVel());
    }
}
