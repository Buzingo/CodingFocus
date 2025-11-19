# CodingFocus

Overview
- This folder contains two separate Arduino sketches with different purposes:
  - `Ultraschall.ino` — ultrasonic distance measurement using an HC-SR04-like sensor, an I2C 16x2 LCD (LiquidCrystal_I2C), and an indicator LED.
  - `Bogosort.ino` — a Serial-only demonstration of bogosort (random shuffle sort). It runs in setup() and prints attempts to Serial. No special sensor or display required.

Files
- `Ultraschall.ino` — measures distance, shows result on I2C LCD, prints warnings to Serial, and drives an LED.
- `Bogosort.ino` — generates a random array, repeatedly shuffles until sorted, and logs each attempt to Serial (for demonstration/education).

Hardware & Components (Ultraschall.ino)
- Arduino Uno (or compatible)
- Ultrasonic sensor HC-SR04 (or equivalent)
- I2C 16x2 LCD with address 0x27 (LiquidCrystal_I2C)
- LED + resistor (connected to digital pin 2)
- Wires / breadboard

Pinout (Ultraschall.ino)
- TRIGPIN: D9
- ECHOPIN: D10
- LED: D2
- I2C: SDA / SCL pins (A4/A5 on older Unos or dedicated SDA/SCL pins on newer boards)

Bogosort.ino hardware
- No special hardware required. It uses Serial for input/output.
- Uses analog pin A0 to seed the PRNG (randomSeed(analogRead(A0))). For better randomness, leave A0 floating or connect a noisy analog source.

Libraries
- LiquidCrystal_I2C — required only for `Ultraschall.ino`. Install via Arduino Library Manager if needed.

How Ultraschall.ino works (brief)
- Sends a 10µs trigger pulse on TRIGPIN to initiate an ultrasonic ping.
- Measures echo pulse width on ECHOPIN via pulseIn() (microseconds).
- Computes distance (cm) as (duration * SPEED_OF_SOUND_CM_PER_US) / 2 (default SPEED_OF_SOUND_CM_PER_US = 0.0343).
- Displays the numeric distance on the I2C LCD and prints warnings to Serial (9600 baud).
- LED behavior:
  - distance >= 100 cm: LED off
  - 10 cm <= distance < 100 cm: LED blinks (ON time = distance * 10 ms)
  - distance < 10 cm: LED stays ON
  - No echo (timeout): LCD shows "No echo" and LED is turned off

How Bogosort.ino works (brief)
- Allocates a small array (LEN = 7) of random integers.
- Uses bogosort: repeatedly shuffles the array until it is sorted, logging each attempt to Serial (9600 baud).
- Runs in setup(); loop() is empty. Intended for educational/demonstration use only.

Usage — Ultraschall.ino
1. Wire the ultrasonic sensor, I2C LCD, and LED per the Pinout section.
2. Open `Ultraschall.ino` in Arduino IDE.
3. Install LiquidCrystal_I2C library if not present.
4. Select board/port and upload.
5. Open Serial Monitor at 9600 baud to see warnings and debug messages.

Usage — Bogosort.ino
1. Open `Bogosort.ino` in Arduino IDE.
2. Select board/port and upload (no extra libraries required).
3. Open Serial Monitor at 9600 baud to watch attempts and final sorted array.

Notes & Troubleshooting
- If `Ultraschall.ino` prints "Warning: No echo received (timeout)", check sensor wiring, power (Vcc/GND), and that nothing is blocking the sensor.
- pulseIn timeout is 30 ms in the sketch; objects farther than this timeout (~5 m) won't be detected.
- SPEED_OF_SOUND_CM_PER_US assumes ~20°C dry air. For better accuracy, compute speed from local temperature:
  c (m/s) ≈ 331.4 + 0.6 * T(°C) → convert to cm/µs by dividing by 1e4.
- Bogosort is intentionally inefficient; for arrays of moderate size it can take an astronomically long time. Use small LEN (current is 7) only for demonstration.

License / Attribution
- Example Arduino sketches for learning and experimentation. Reuse as needed.
