#include "JMotor.h"
#include <Arduino.h>
JEncoderPinChangeQuadrature enc(3, 4);
ENCODER_PCQ_MAKE_ISRS_MACRO(enc);

void setup()
{
    enc.setUpInterrupts(enc_jENCODER_ISR_A, enc_jENCODER_ISR_B);
}

void loop()
{
    enc.run();
}
