# STM32-Ultrasonic-Parking-Assistant-
STM32 Nucleo-F446RE ultrasonic parking assistant using HC-SR04, I2C LCD, LEDs and buzzer for real-time distance based obstacle warnings.
# STM32 Ultrasonic Parking Assistant

An embedded parking assistance system developed using the **STM32 Nucleo-F446RE** and an **HC-SR04 ultrasonic sensor**.

The system continuously measures the distance between a vehicle and an obstacle and provides real-time visual and audible feedback using LEDs, a buzzer, and a 16×2 LCD.

## Features

- Real-time obstacle distance measurement
- STM32 Nucleo-F446RE based control
- HC-SR04 ultrasonic distance sensing
- 16×2 LCD distance display
- Green, yellow, and red LED warning levels
- Variable buzzer warning frequency
- Critical-distance continuous warning
- Sensor timeout/error handling
- Automatic buzzer muting when the measured distance remains unchanged

## Hardware

| Component | Quantity |
|---|---:|
| STM32 Nucleo-F446RE | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| 16×2 LCD Display | 1 |
| Green LED | 1 |
| Yellow LED | 1 |
| Red LED | 1 |
| Piezo Buzzer | 1 |
| 220Ω / 330Ω Resistors | 6 |
| Breadboard | 1 |
| Jumper Wires | As required |

## System Overview

The HC-SR04 sends an ultrasonic pulse toward an obstacle.

The STM32 measures the echo pulse duration and calculates distance using:


The measured distance is then used to determine the parking warning level.

Parking Warning Logic
Distance	LED	Buzzer
> 30 cm	Green ON	OFF
10–30 cm	Yellow blinking	Slow beep
5–10 cm	Red blinking	Fast beep
≤ 5 cm	Red ON	Continuous tone

The slow warning interval is approximately 500 ms, while the fast warning interval is approximately 150 ms.

Working Principle
The HC-SR04 continuously measures the distance to an obstacle.
The STM32 processes the measured echo duration.
The calculated distance is displayed on the LCD.
When the obstacle is far away, the green LED indicates a safe condition.
As the obstacle becomes closer, the yellow LED and buzzer provide a warning.
At shorter distances, the red LED and faster buzzer indicate danger.
At 5 cm or below, the red LED remains ON and the buzzer generates a continuous warning.
Software

The project was developed using STM32 HAL libraries.

Main modules include:

## main.c Contains:

GPIO initialization
Timer initialization
I2C initialization
HC-SR04 measurement
Parking warning logic
LCD output
Main control loop

## lcd_i2c.c

Contains the I2C LCD driver including:

LCD initialization
Command transmission
Data transmission
Cursor positioning
Display clearing
String output
Main Functions
HCSR04_Read()

Generates the ultrasonic trigger pulse, measures the echo duration, and converts the measured time into distance.



## Distance Measurement

The ultrasonic distance is calculated from the time required for the transmitted sound wave to travel to the obstacle and return.

d = v × t / 2

Where:

d = distance
v = speed of sound
t = total travel time

For the HC-SR04:
Distance (cm) = Echo Time (µs) / 58

# Circuit Diagram

the project circuit diagram:

<img width="754" height="567" alt="image" src="https://github.com/user-attachments/assets/87fb1185-9979-4d88-b0ac-0dfe22301d3a" />

# Prototype

<img width="523" height="367" alt="image" src="https://github.com/user-attachments/assets/28bc0a87-0bc6-4ee5-8aff-43d214120991" />




