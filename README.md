# CodingFocus

Overview
- This folder contains several Arduino sketches for different demos and small projects:
  - `Ultraschall.ino` — ultrasonic distance measurement using an HC‑SR04‑like sensor, an I2C 16x2 LCD, and an indicator LED.
  - `Ultraschall_plus_Piezo.ino` — same as above, with an added piezo/buzzer for audible feedback.
  - `Bogosort.ino` — Serial-only demonstration of bogosort (random shuffle sort). Runs in setup() and prints attempts to Serial.
  - `PotiMotor.ino` — simple potentiometer-controlled motor (PWM) demo.
  - `7SegmentRechner.ino` — two-digit 7-segment demo: select two hex digits via a potentiometer and button, shows sum (0–F) and error blink for overflow.

Repository layout
- Ultraschall.ino
- Ultraschall_plus_Piezo.ino
- Bogosort.ino
- PotiMotor.ino
- 7SegmentRechner.ino
- README.md (this file)

Files (what they do)
- Ultraschall.ino — measures distance, shows the numeric result on an I2C LCD, prints warnings to Serial (9600), and drives an LED based on distance thresholds.
- Ultraschall_plus_Piezo.ino — same as Ultraschall.ino but also drives a piezo/buzzer (pin 4) for distance-dependent tones.
- Bogosort.ino — generates a small random array, repeatedly shuffles it until sorted, and logs attempts to Serial (9600). Seeds PRNG via analogRead(A0).
- PotiMotor.ino — reads a potentiometer (A0) and maps the value to two PWM outputs to drive a motor or demonstrate bi‑directional control.
- 7SegmentRechner.ino — lets you pick two 0–9 hex digits using a potentiometer and a single button (debounced). Pressing the button stores the first digit, pressing again stores the second digit and computes the sum. Displays the live selection or the result on a 7‑segment (segments driven directly). If the sum exceeds 15 (0xF) the sketch blinks all segments as an error and resets. Also prints simple debug messages to Serial (9600).

Hardware & Components (Ultraschall / Ultraschall_plus_Piezo)
- Arduino Uno (or compatible)
- Ultrasonic sensor HC‑SR04 (or equivalent)
- I2C 16x2 LCD (LiquidCrystal_I2C). Common I2C addresses: 0x27 or 0x3F — run an I2C scanner if uncertain.
- LED + resistor (connected to digital pin 2)
- (Ultraschall_plus_Piezo) Piezo buzzer on digital pin 4
- Wires / breadboard

Pinout (Ultraschall & Ultraschall_plus_Piezo)
- TRIGPIN: D9
- ECHOPIN: D10
- LED: D2
- BUZZER (Ultraschall_plus_Piezo): D4
- I2C: SDA / SCL pins (A4/A5 on older Unos or dedicated SDA/SCL pins on newer boards)

Hardware & Components (PotiMotor)
- Potentiometer connected to A0
- Two PWM output pins (MRED = D3, MBLACK = D5 in PotiMotor.ino)
- Small motor / driver circuit (observe current limits)

Hardware & Components (7SegmentRechner)
- Arduino Uno (or compatible)
- Two-digit 7‑segment display or single-digit display wired to segment pins (common-cathode assumed in sketch)
- Potentiometer on A0 for digit selection
- Pushbutton on digital pin 12 (INPUT_PULLUP)
- Wires / breadboard / current-limiting resistors for segments as required

Bogosort.ino hardware
- No special hardware required. It uses Serial for input/output and analog A0 for PRNG seed.

Libraries
- LiquidCrystal_I2C — required for Ultraschall*.ino sketches. Install via Arduino Library Manager if needed.

How Ultraschall.ino works (brief)
- Sends a 10µs trigger pulse on TRIGPIN to initiate an ultrasonic ping.
- Measures echo pulse width on ECHOPIN via pulseIn() (microseconds).
- Computes distance (cm) as (duration * SPEED_OF_SOUND_CM_PER_US) / 2 (default SPEED_OF_SOUND_CM_PER_US = 0.0343).
- Displays distance on the I2C LCD and prints warnings to Serial (9600 baud).
- LED and buzzer behavior:
  - distance >= 100 cm: LED off, no tone
  - 10 cm <= distance < 100 cm: LED blinks; buzzer (if present) emits distance-dependent tones
  - distance < 10 cm: LED stays ON; buzzer may emit a continuous/high tone
  - No echo (timeout): LCD shows "No echo" and LED is turned off

How Bogosort.ino works (brief)
- Allocates a small array (LEN = 7 by default) of random integers.
- Uses bogosort: repeatedly shuffles the array until it is sorted, logging each attempt to Serial (9600 baud).
- Seeds PRNG with analogRead(A0); for better randomness leave A0 floating or connect a noisy source.
- Runs in setup(); loop() is empty.

How PotiMotor.ino works (brief)
- Reads analog value from A0.
- Maps value to PWM duty cycle for two outputs to simulate bidirectional motor control.
- Logs raw and converted PWM values to Serial (9600).

How 7SegmentRechner.ino works (brief)
- Reads potentiometer (A0) and maps to 0–9 for live selection.
- Uses a debounced button (INPUT_PULLUP) to capture: first press selects first digit, second press selects second digit and triggers sum calculation.
- If sum <= 15 the result is shown (supports 0–F); if sum > 15 the sketch blinks all segments as an error and resets to the initial state.
- Serial debug messages are printed at 9600 baud.

Usage — Ultraschall / Ultraschall_plus_Piezo
1. Open the relevant `.ino` file in the Arduino IDE.
2. Install LiquidCrystal_I2C via Library Manager if needed.
3. Run an I2C scanner if your LCD is blank to confirm the address (commonly 0x27 or 0x3F) and update the constructor in the sketch if required.
4. Select board/port and upload.
5. Open Serial Monitor at 9600 baud for debug messages.

Usage — Bogosort
1. Open `Bogosort.ino` in the Arduino IDE.
2. Select board/port and upload (no additional libraries required).
3. Open Serial Monitor at 9600 baud to watch shuffles and the final sorted array.

Usage — PotiMotor
1. Open `PotiMotor.ino`.
2. Connect potentiometer and motor/driver per sketch comments.
3. Upload and use Serial Monitor (9600) to observe values.

Usage — 7SegmentRechner
1. Open `7SegmentRechner.ino` in the Arduino IDE.
2. Wire the segments and button as described in the sketch comments (check pin defines).
3. Upload, open Serial Monitor at 9600 baud to view debug output.
4. Use the potentiometer to choose digits; press the button to store first and second digits and to reset.

Troubleshooting
- "Warning: No echo received (timeout)" — check sensor wiring, power (Vcc/GND), and that nothing is blocking the sensor.
- If LCD shows nothing or gibberish, run an I2C scanner and confirm the address and wiring (SDA/SCL, VCC, GND, contrast).
- pulseIn timeout is 30 ms by default; beyond that (~5 m) objects won't be detected.
- Beware of bogosort: it's intentionally inefficient. Use small LEN only for demonstration.

Notes
- SPEED_OF_SOUND_CM_PER_US assumes ~20°C. For higher accuracy compute speed from local temperature: c (m/s) ≈ 331.4 + 0.6 * T(°C) → convert to cm/µs by dividing by 10,000.
- Reuse and adapt sketches for learning; check pin assignments before wiring.

License / Attribution
- Example Arduino sketches for learning and experimentation. Reuse as needed.
