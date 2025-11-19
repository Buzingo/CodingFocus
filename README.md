# UltraschallSensor

Overview
- This folder contains two Arduino sketches that use an HC-SR04-like ultrasonic sensor and an I2C LCD (LiquidCrystal_I2C) to measure and display distance.
- Files:
  - `Ultraschall.ino` — primary ultrasonic distance measurement sketch.
  - `Bogosort.ino` — duplicate sketch with the same functionality (kept for comparison/history).

Hardware & Components
- Arduino Uno (or compatible)
- Ultrasonic sensor (HC-SR04 or equivalent)
- I2C 16x2 LCD with address 0x27 (LiquidCrystal_I2C)
- LED (with appropriate resistor) connected to digital pin 2
- Wires / breadboard

Pinout (as used in the sketches)
- TRIGPIN: D9
- ECHOPIN: D10
- LED: D2
- I2C: SDA / SCL pins (connect to A4/A5 on older Uno or dedicated SDA/SCL pins on newer boards)

Libraries
- LiquidCrystal_I2C — install via the Arduino Library Manager if not already present.

How it works (brief)
- The sketch toggles the TRIG pin to send a 10µs pulse to the ultrasonic sensor.
- It measures the echo pulse width on ECHOPIN using `pulseIn()` (microseconds).
- Distance (cm) = (duration * SPEED_OF_SOUND_CM_PER_US) / 2, where SPEED_OF_SOUND_CM_PER_US is 0.0343 by default.
- The value is displayed on the I2C LCD and printed to Serial (9600 baud).
- The LED behavior:
  - distance >= 100 cm: LED off
  - 10 cm <= distance < 100 cm: LED blinks; ON time = distance * 10 ms, then OFF
  - distance < 10 cm: LED stays ON
  - If no echo is received (timeout), LCD shows "No echo" and LED is turned off.

Calibrating speed of sound
- The constant SPEED_OF_SOUND_CM_PER_US = 0.0343 assumes ~20°C dry air.
- For improved accuracy, adjust the constant based on ambient temperature or compute speed using:
  - c (m/s) ≈ 331.4 + 0.6 * T(°C) → convert to cm/µs by dividing by 1e4.

Usage
1. Wire components as described in Pinout.
2. Open either `Ultraschall.ino` or `Bogosort.ino` in the Arduino IDE.
3. Select correct board and COM port.
4. Install LiquidCrystal_I2C if required.
5. Upload the sketch.
6. Open Serial Monitor at 9600 baud to see debug/warning messages.

Notes & Troubleshooting
- If you see "Warning: No echo received (timeout)", ensure wiring is correct and sensor is powered.
- Timeout in `pulseIn` is set to 30 ms; objects farther than ~5 m will not be detected with this timeout.
- Both files are functionally the same; keep one or rename as needed.
- The LED timing uses simple delays — if you need non-blocking behavior, consider using millis()-based timing.

License / Attribution
- This repository contains small Arduino example sketches; reuse as needed.
