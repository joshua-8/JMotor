# JMotor Library

## work in progress! Bug reports/fixes welcomed

reference docs (made with Doxygen): https://joshua-8.github.io/JMotor/md_readme.html


list of all classes: https://joshua-8.github.io/JMotor/hierarchy.html


This library defines common interfaces for reading encoders and controlling motors allowing for easy open or closed loop motor control.

Supports reading the following kinds of encoders: quadrature, single pin, absolute with PWM output, AS5048b with I2C. Supports controlling the following kinds of motor controllers: single PWM pin, PWM and direction pins, servo signal, L293 driver. 


This library uses lots of floating point math. This makes it easy to use, but if you need speed optimized code this library isn't for you.
