## Quick summary

This repository contains two standalone Arduino sketches: `Ultraschall.ino` (ultrasonic distance sensor + I2C LCD + LED) and `Bogosort.ino` (Serial-only demonstration of bogosort). The guidance below helps an AI coding assistant be immediately productive when editing, troubleshooting, or extending these sketches.

## What matters (big picture)
- Single-file Arduino sketches live at the repository root. Each sketch is a complete program relying on the Arduino runtime (setup()/loop()).
- `Ultraschall.ino` reads an HC-SR04-like ultrasonic sensor, prints to Serial (9600 baud), and writes to a 16x2 I2C LCD (address 0x27). It also drives an LED (pin D2) with distance-based behavior.
- `Bogosort.ino` is an educational demo that runs inside `setup()` and prints progress to Serial (9600 baud). It seeds the PRNG with `analogRead(A0)`.

## Patterns and conventions to follow
- Hardware conventions: Pin constants are defined at top (e.g., `TRIGPIN = 9`, `ECHOPIN = 10`, `LED = 2`). Respect these constants when refactoring — do not hardcode pins in multiple places.
- I/O and timing:
  - Both sketches use `Serial.begin(9600)`; preserve that baud rate unless you change all Serial callers.
  - `Ultraschall.ino` uses `pulseIn(..., 30000)` (30 ms timeout). Expect `duration==0` to indicate no-echo/timeouts.
  - LCD uses LiquidCrystal_I2C at address `0x27` with 16x2 dimensions.
- Randomness: `Bogosort.ino` uses `randomSeed(analogRead(A0))`. To reproduce or test, set A0 to a known value or mock `randomSeed()`.
- Program behavior:
  - `Bogosort.ino` runs to completion in `setup()` and leaves `loop()` empty. Avoid moving long-running or blocking logic into `loop()` without preserving existing behavior.
  - `Ultraschall.ino` performs periodic sensing in `loop()` and uses short delays (50 ms) and blocking `delay(distance*10)` when blinking the LED.

## Build / upload / debug workflows (concrete examples)
- Typical development: open the `.ino` you want in the Arduino IDE, install `LiquidCrystal_I2C` if prompted, select the target board and port, then upload.
- CLI example (PowerShell) using arduino-cli — replace the FQBN and COM port with your board and port:

```powershell
arduino-cli compile --fqbn arduino:avr:uno "Bogosort.ino"
arduino-cli upload -p COM3 --fqbn arduino:avr:uno "Bogosort.ino"
```

- For `Ultraschall.ino` ensure `LiquidCrystal_I2C` is installed before compile. If using the Arduino IDE, use Tools → Manage Libraries.
- Serial debugging: Open Serial Monitor at 9600 baud to see printed warnings and debug messages (e.g., "Warning: No echo received (timeout)").

## Common quick fixes & diagnostics (from code)
- "No echo received (timeout)": check TRIG/ECHO wiring, sensor Vcc/GND, and that nothing is blocking the transducer. Confirm `pulseIn` timeout isn't being triggered by wiring errors.
- LCD blank or garbled: confirm I2C address (defaults to 0x27 here) and wiring (SDA/SCL). On older Unos use A4/A5 for SDA/SCL.
- Bogosort never finishes (or extremely slow): this is expected for large LEN values — do not increase LEN for normal testing.

## Files to reference when making changes
- `Ultraschall.ino` — sensor logic, display, LED behavior, timing constants (`SPEED_OF_SOUND_CM_PER_US`, pulseIn timeout).
- `Bogosort.ino` — PRNG seeding (`analogRead(A0)`), single-run educational logic in `setup()`.
- `README.md` — wiring, pinout, and usage notes (keeps useful context; update it when changing pin constants or behavior).

## What an AI assistant should NOT do automatically
- Do not assume a specific target board or serial port; prompt the user to confirm FQBN and COM port before auto-upload.
- Avoid increasing `LEN` in `Bogosort.ino` or removing PRNG seeding without explicit instruction — that changes runtime complexity and reproducibility.

## Example edits the assistant can safely make
- Small refactors that centralize pin constants or replace magic numbers with named constants (show both new constant and updated call sites in a single patch).
- Add comments linking behavior to hardware (e.g., comment that LED pin D2 blinks with ON time = distance*10 ms).

## Ask the user when uncertain
- Target board FQBN and upload port (for example: FQBN = "arduino:avr:uno", port = COM3) for CLI uploads.
- Whether they want `Bogosort.ino` converted to non-blocking demonstration (e.g., move attempts into `loop()` with a per-iteration delay).

If anything here is unclear or you want more detail (for example, suggested tests or a CI flow using arduino-cli), tell me which part to expand or provide your target board/port and I will update this file accordingly.
