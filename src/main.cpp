#include <Arduino.h>
#include <JMotor.h>
#define port1Pin 32
#define port2Pin 33
#define port3Pin 25
#define port4Pin 26
#define port5Pin 27
//          PWM_CH, PIN
#define port1 8, port1Pin
#define port2 7, port2Pin
#define port3 6, port3Pin
#define port4 5, port4Pin
#define port5 4, port5Pin
//           PWM_CH, EN_PIN, PIN1, PIN2
#define portA 3, 23, 22, 19
#define portB 1, 5, 18, 21
#define portC 2, 17, 16, 12
#define portD 0, 13, 4, 14

#define inport1 39
#define inport2 34
#define inport3 35

#define batMonitorPin 36

#define dacUnitsPerVolt 413

JMotorDriverEsp32L293 myDriver = JMotorDriverEsp32L293(portA);

JVoltageCompMeasure<20> voltageComp = JVoltageCompMeasure<20>(batMonitorPin, dacUnitsPerVolt, 1.0);
JMotorCompStandardConfig ttConfig = JMotorCompStandardConfig(1.4, .652, 2.6, 1.3, 4, 1.6, 100);
JMotorCompStandard myMotorCompensator = JMotorCompStandard(voltageComp, ttConfig, 1.0);
JMotorControllerBasicOpen myController = JMotorControllerBasicOpen(myDriver, myMotorCompensator, .5);
// JEncoderPWMAbsoluteAttachInterrupt encoder = JEncoderPWMAbsoluteAttachInterrupt(inport1, JEncoderPWMAbsolute_AS5048settings, true, 1, 50000, 1000);
// jENCODER_MAKE_ISR_MACRO(encoder);

float vel = 0;
bool inc = true;
void setup()
{
    Serial.begin(9600);
    myController.enable();
    // encoder.setUpInterrupts(encoder_jENCODER_ISR);
}
void loop()
{
    // if (Serial.available()) {
    //     vel = Serial.parseFloat();
    //     while (Serial.available()) {
    //         Serial.read();
    //     }
    // }
    inc ? vel += .01 : vel -= .01;
    if (abs(vel) > 1)
        inc = !inc;
    myDriver.set(vel);
    //    myController.setVelTarget(vel);
    // encoder.run();
    // Serial.print(vel);
    // Serial.print(",");
    // Serial.print(myController.getVel());
    // Serial.print(",");
    // Serial.print(myController.getDriverSetVal());
    // Serial.print(",");
    // Serial.print(encoder.getVel());
    Serial.println();
    delay(15);
}
