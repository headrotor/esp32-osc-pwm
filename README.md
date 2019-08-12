# esp32-osc-pwm
Control ESP32 ledc PWM channels via Open Sound Control (OSC) messages

Created as a class demonstration to see the effect of resolution and frequency on PWM dimming of LEDs.

Uses ToouchOsc "simple" layout: First page sliders 1-3 control PWM of ledc channels 0-2. Slider 5 controls frequency. 
Second page pushbuttons 1 and 2 increment PWM resolution from 0 to 16. 
