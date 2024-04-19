#include <Arduino.h>

// drivetrain example
#define portA 1, 26, 25

#include <JMotor.h>

JMotorDriverEsp32L293TwoPins driver = JMotorDriverEsp32L293TwoPins(portA);
String inString = "";
float value = 0;
int mode = 1;

void setup()
{
    Serial.begin(115200);
}
void loop()
{
    while (Serial.available() > 0) {
        int inChar = Serial.read();
        inString += (char)inChar;
        if (inChar == '\n') {
            if (inString.equals("e\n")) {
                driver.enable();
                Serial.println("enabled");
            } else if (inString.equals(" \n")) {
                driver.disable();
                Serial.println("disabled");
            } else if (inString.equals("p\n")) {
                mode = 1;
                Serial.println("power");
            } else {
                value = inString.toFloat();
                if (mode == 1) { // s
                    Serial.println(value);
                    driver.set(value);
                }
            }
            inString = "";
        }
    }
    delay(5);
}
