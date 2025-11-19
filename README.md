# UltraschallSensor

Lightweight README for the UltraschallSensor (ultrasonic sensor) project.

## Overview
This repository contains example code, wiring notes and basic usage instructions for ultrasonic distance sensing (e.g., HC-SR04) with microcontrollers and single-board computers.

## Contents
- `README.md` — this file  
- `arduino/` — Arduino example sketches  
- `raspberrypi/` — Raspberry Pi (Python) examples  
- `schematics/` — wiring diagrams (optional)

## Requirements
- Hardware: HC-SR04 (or compatible ultrasonic sensor) and a microcontroller or SBC (Arduino, ESP32, Raspberry Pi...).  
- Software: Arduino IDE for sketches, Python 3 with GPIO library for Raspberry Pi.

## Wiring (HC-SR04 typical)
- VCC -> 5V (or 3.3V depending on sensor/board)  
- GND -> GND  
- TRIG -> Digital output pin  
- ECHO -> Digital input pin (use level shifting if board is 3.3V)

Example:
- Arduino UNO: TRIG = D9, ECHO = D10  
- Raspberry Pi: TRIG = GPIO23, ECHO = GPIO24 (use voltage divider on ECHO)

## Example: Arduino (HC-SR04)
```cpp
const int trigPin = 9;
const int echoPin = 10;

void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distanceCm = (duration / 2.0) * 0.0343;
    Serial.print(distanceCm);
    Serial.println(" cm");
    delay(200);
}
```

## Example: Raspberry Pi (Python, RPi.GPIO)
```python
import time
import RPi.GPIO as GPIO

TRIG = 23
ECHO = 24

GPIO.setmode(GPIO.BCM)
GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

try:
        while True:
                GPIO.output(TRIG, False)
                time.sleep(0.05)

                GPIO.output(TRIG, True)
                time.sleep(0.00001)
                GPIO.output(TRIG, False)

                while GPIO.input(ECHO) == 0:
                        pulse_start = time.time()
                while GPIO.input(ECHO) == 1:
                        pulse_end = time.time()

                pulse_duration = pulse_end - pulse_start
                distance = pulse_duration * 17150  # cm
                print(f"Distance: {distance:.1f} cm")
                time.sleep(0.2)
finally:
        GPIO.cleanup()
```

## Troubleshooting
- Incorrect readings: check wiring, power supply, and use proper level shifting for ECHO on 3.3V boards.  
- Unstable values: add averaging or median filter; ensure sensor has clear line-of-sight.

## License
MIT — see LICENSE file or add one if needed.

## Notes
- Adjust pins and voltage levels for your hardware.  
- Keep measurements within sensor range (usually 2 cm – 400 cm for HC-SR04).

Feel free to add project-specific sketches, diagrams, and test logs in the repository.