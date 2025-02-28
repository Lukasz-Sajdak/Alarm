# Alarm System on FRDM-KL05Z

## Overview

This project implements an alarm system on the FRDM-KL05Z development board. The system detects movement and responds by triggering an alert when necessary.

![20250123_161817 (1)](https://github.com/user-attachments/assets/c0dedf2d-489f-4c77-a1b0-d6f46485e8b5)

## System Capabilities

- **Arming and disarming the alarm using a code entered via the keypad.**
- **Notifying users when motion or position change is detected.**
- **Administrator mode for changing the arming code.**

## Hardware Requirements
- **FRDM-KL05Z development board**
- **LCD display for output**
- **Keypad for user input**
- **PIR sensor for motion detection**

## Features
- **LCD display for system status and user interaction**
- **Keypad for entering access codes**
- **Motion detection using an accelerometer and a PIR sensor**
- **Hardware implementation using FRDM-KL05Z**

## System Operation

1. Arming and Disarming the Alarm
  -The alarm is armed after entering the correct code.
  -The code is entered using buttons S2, S3, and S4.
  -Once activated, the system monitors the PIR sensor and the accelerometer.
  -Default arming code: S2 -> S3 -> S4.

2. Motion Detection
  -If the alarm is active and the PIR sensor detects movement, the LCD display shows the message "Motion Detected!" and the blue LED lights up.

3. Position Change Detection
  -After arming, the base position from the accelerometer (X, Y, Z) is recorded.
  -If the position of the board changes, the alarm is triggered (red LED lights up) and the message "Warning: Intruder!" appears on the LCD display.

4. Administrator Mode
  -A special key combination allows access to administrator mode (must be entered while holding button S1).
  -In this mode, the arming code can be changed.
  -The LCD display shows the message "Admin Mode".
  -Code: S4 -> S3 -> S2 (while holding button S1).
