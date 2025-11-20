#define LED_SEG_A_PIN   10
#define LED_SEG_B_PIN   11
#define LED_SEG_C_PIN   5
#define LED_SEG_D_PIN   6
#define LED_SEG_E_PIN   7
#define LED_SEG_F_PIN   9
#define LED_SEG_G_PIN   8
#define LED_SEG_DOT_PIN 4
#define POTI A0
#define BUTTON 12

byte dig1 = 0;
byte dig2 = 0;
byte sumResult = 0;

enum State {
  PICK_FIRST,
  PICK_SECOND,
  SHOW_RESULT
};

State state = PICK_FIRST;

int lastButtonReading = HIGH;
int buttonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms

void setup()
{
  pinMode(LED_SEG_A_PIN, OUTPUT);
  pinMode(LED_SEG_B_PIN, OUTPUT);
  pinMode(LED_SEG_C_PIN, OUTPUT);
  pinMode(LED_SEG_D_PIN, OUTPUT);
  pinMode(LED_SEG_E_PIN, OUTPUT);
  pinMode(LED_SEG_F_PIN, OUTPUT);
  pinMode(LED_SEG_G_PIN, OUTPUT);
  pinMode(LED_SEG_DOT_PIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
}

/**
 * loop
 *
 * Main runtime loop for a simple two-number picker and 7-segment display adder.
 *
 * Behavior overview:
 * - Reads an analog potentiometer (POTI) and maps its 0..1023 reading to a digit range 0..9.
 * - Reads a push-button (configured with INPUT_PULLUP; pressed = LOW) with debounce.
 * - Implements a small state machine with three states:
 *     PICK_FIRST  - user selects the first digit with the potentiometer; pressing the button
 *                   confirms the choice and moves to PICK_SECOND.
 *     PICK_SECOND - user selects the second digit with the potentiometer; pressing the button
 *                   confirms the second digit and triggers the sum calculation.
 *     SHOW_RESULT - displays the computed sum (0x0..0xF supported); pressing the button
 *                   resets back to PICK_FIRST.
 *
 * Detailed behavior and side effects:
 * - Potentiometer mapping:
 *     * analogRead(POTI) is mapped to an integer byte potVal in the range 0..9 using map().
 *     * This potVal is used for "live" display in the PICK_FIRST and PICK_SECOND states.
 *
 * - Button debouncing and edge detection:
 *     * The raw button reading is compared to lastButtonReading; a change starts/restarts a
 *       debounce timer (lastDebounceTime).
 *     * Only after debounceDelay milliseconds of stable reading is the new buttonState adopted.
 *     * A falling edge (HIGH -> LOW, i.e. button pressed when using INPUT_PULLUP) is the event
 *       that triggers state transitions and actions.
 *
 * - State transitions and actions on button press:
 *     * PICK_FIRST:
 *         - Stores dig1 = potVal.
 *         - Transitions to PICK_SECOND.
 *         - Emits a Serial.print message indicating the first chosen digit.
 *     * PICK_SECOND:
 *         - Stores dig2 = potVal.
 *         - Computes unsigned int s = (unsigned int)dig1 + (unsigned int)dig2.
 *         - If s > 15 (sum cannot be displayed in a single 7-segment nibble):
 *             + Emits an error message via Serial.print with the offending sum.
 *             + Performs a visible error indication: blink all segments and the decimal dot
 *               several times (blocking delays).
 *             + Resets state back to PICK_FIRST.
 *         - Else (s <= 15):
 *             + Stores sumResult = (byte)s.
 *             + Briefly blinks the decimal dot as confirmation.
 *             + Transitions to SHOW_RESULT to display the result.
 *     * SHOW_RESULT:
 *         - A press resets the state to PICK_FIRST and prints "Reset auf Anfang" to Serial.
 *
 * - Display updates:
 *     * When in PICK_FIRST or PICK_SECOND, the current potVal is shown via displayDigit(potVal)
 *       for live selection feedback.
 *     * When in SHOW_RESULT, displayDigit(sumResult) is shown (supports values 0..15).
 *     * Error blinking uses direct digitalWrite() calls to turn on all segment pins and
 *       calls turnOffDisplay() between blinks. The decimal dot is also toggled as part of
 *       the blink pattern.
 *
 * - Timing and blocking behavior:
 *     * The loop uses delay() in several places:
 *         - Small fixed delay(50) at the end of each loop iteration.
 *         - Blocking delays during dot blink confirmation and during the multi-blink error
 *           routine. These delays stop other processing while running.
 *
 * - Globals and external dependencies (expected to exist in the surrounding program):
 *     * Hardware/IO:
 *         - POTI: analog input pin for the potentiometer.
 *         - BUTTON: digital input pin for the button (configured INPUT_PULLUP externally).
 *         - LED_SEG_A_PIN, LED_SEG_B_PIN, LED_SEG_C_PIN, LED_SEG_D_PIN,
 *           LED_SEG_E_PIN, LED_SEG_F_PIN, LED_SEG_G_PIN, LED_SEG_DOT_PIN: segment pin defines.
 *     * State & timing variables:
 *         - lastButtonReading, lastDebounceTime, debounceDelay, buttonState
 *         - state (enum or integer), PICK_FIRST, PICK_SECOND, SHOW_RESULT
 *         - dig1, dig2 (selected digits), sumResult (byte)
 *     * Helper functions:
 *         - displayDigit(byte value): draws a 0..15 value on the 7-segment display.
 *         - turnOffDisplay(): turns off all display segments.
 *     * Serial: used for debug/info prints.
 *
 * Notes and cautions:
 * - The function is blocking during blink/error/confirmation sequences due to delay(),
 *   which will pause button responsiveness and other tasks.
 * - The sum overflow case (>15) is handled by a visual error and state reset; no numeric
 *   wrapping occurs.
 * - The code assumes the button is wired with pull-up logic (INPUT_PULLUP) so pressed =
 *   logic LOW.
 *
 * Return:
 * - void (no return value). The function runs repeatedly as the Arduino main loop.
 */
void loop()
{
  // Aktuellen Poti-Wert auf 0-9 mappen
  byte potVal = map(analogRead(POTI), 0, 1023, 0, 9);

  // Button lesen und entprellen (INPUT_PULLUP -> LOW = gedrückt)
  int reading = digitalRead(BUTTON);
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      // Fallende Flanke (HIGH -> LOW): Button gedrückt
      if (buttonState == LOW) {
        // Ereignis je nach Zustand
        if (state == PICK_FIRST) {
          dig1 = potVal;
          state = PICK_SECOND;
          Serial.print("erste Zahl gewählt: ");
          Serial.println(dig1);
        } else if (state == PICK_SECOND) {
          dig2 = potVal;
          // Summe berechnen und prüfen
          unsigned int s = (unsigned int)dig1 + (unsigned int)dig2;
          if (s > 15u) {
            // Fehler: Ergebnis zu groß -> blinke alle Segmente kurz und reset
            Serial.print("Fehler: Summe zu groß: ");
            Serial.println(s);
            for (int i=0;i<6;i++) {
              // alle Segmente an
              digitalWrite(LED_SEG_A_PIN, HIGH);
              digitalWrite(LED_SEG_B_PIN, HIGH);
              digitalWrite(LED_SEG_C_PIN, HIGH);
              digitalWrite(LED_SEG_D_PIN, HIGH);
              digitalWrite(LED_SEG_E_PIN, HIGH);
              digitalWrite(LED_SEG_F_PIN, HIGH);
              digitalWrite(LED_SEG_G_PIN, HIGH);
              digitalWrite(LED_SEG_DOT_PIN, HIGH);
              delay(200);
              turnOffDisplay();
              digitalWrite(LED_SEG_DOT_PIN, LOW);
              delay(200);
            }
            // Reset auf Anfang
            state = PICK_FIRST;
          } else {
            sumResult = (byte)s;
            // Punkt kurz blinken als Anzeige
            for (int i=0;i<3;i++) {
              digitalWrite(LED_SEG_DOT_PIN, HIGH);
              delay(150);
              digitalWrite(LED_SEG_DOT_PIN, LOW);
              delay(150);
            }
            state = SHOW_RESULT;
          }
        } else if (state == SHOW_RESULT) {
          // Reset und von vorn
          state = PICK_FIRST;
          Serial.println("Reset auf Anfang");
        }
      }
    }
  }
  lastButtonReading = reading;

  // Verhalten je nach Zustand
  if (state == PICK_FIRST) {
    displayDigit(potVal); // live Auswahl anzeigen
  } else if (state == PICK_SECOND) {
    displayDigit(potVal); // live zweite Auswahl anzeigen
  } else if (state == SHOW_RESULT) {
    displayDigit(sumResult); // Ergebnis 0-F anzeigen 
  }

  delay(50); 
}

void displayDigit(byte digit)
{
  turnOffDisplay();

  // Nur 0x0–0xF zulassen
  if (digit > 15) {
    return;
  }

  if (digit <= 9) {

    // Segment a
    if (digit != 1 && digit != 4) {
      digitalWrite(LED_SEG_A_PIN, HIGH);
    }

    // Segment b
    if (digit != 5 && digit != 6) {
      digitalWrite(LED_SEG_B_PIN, HIGH);
    }

    // Segment c
    if (digit != 2) {
      digitalWrite(LED_SEG_C_PIN, HIGH);
    }

    // Segment d
    if (digit != 1 && digit != 4 && digit != 7) {
      digitalWrite(LED_SEG_D_PIN, HIGH);
    }

    // Segment e
    if (digit == 2 || digit == 6 || digit == 8 || digit == 0) {
      digitalWrite(LED_SEG_E_PIN, HIGH);
    }

    // Segment f
    if (digit != 1 && digit != 2 && digit != 3 && digit != 7) {
      digitalWrite(LED_SEG_F_PIN, HIGH);
    }

    // Segment g
    if (digit != 0 && digit != 1 && digit != 7) {
      digitalWrite(LED_SEG_G_PIN, HIGH);
    }

  } else {
    // Hex-Buchstaben A–F für 10–15

    switch (digit) {
      case 10: // A: a, b, c, e, f, g
        digitalWrite(LED_SEG_A_PIN, HIGH);
        digitalWrite(LED_SEG_B_PIN, HIGH);
        digitalWrite(LED_SEG_C_PIN, HIGH);
        digitalWrite(LED_SEG_E_PIN, HIGH);
        digitalWrite(LED_SEG_F_PIN, HIGH);
        digitalWrite(LED_SEG_G_PIN, HIGH);
        break;

      case 11: // b: c, d, e, f, g
        digitalWrite(LED_SEG_C_PIN, HIGH);
        digitalWrite(LED_SEG_D_PIN, HIGH);
        digitalWrite(LED_SEG_E_PIN, HIGH);
        digitalWrite(LED_SEG_F_PIN, HIGH);
        digitalWrite(LED_SEG_G_PIN, HIGH);
        break;

      case 12: // C: a, d, e, f
        digitalWrite(LED_SEG_A_PIN, HIGH);
        digitalWrite(LED_SEG_D_PIN, HIGH);
        digitalWrite(LED_SEG_E_PIN, HIGH);
        digitalWrite(LED_SEG_F_PIN, HIGH);
        break;

      case 13: // d: b, c, d, e, g
        digitalWrite(LED_SEG_B_PIN, HIGH);
        digitalWrite(LED_SEG_C_PIN, HIGH);
        digitalWrite(LED_SEG_D_PIN, HIGH);
        digitalWrite(LED_SEG_E_PIN, HIGH);
        digitalWrite(LED_SEG_G_PIN, HIGH);
        break;

      case 14: // E: a, d, e, f, g
        digitalWrite(LED_SEG_A_PIN, HIGH);
        digitalWrite(LED_SEG_D_PIN, HIGH);
        digitalWrite(LED_SEG_E_PIN, HIGH);
        digitalWrite(LED_SEG_F_PIN, HIGH);
        digitalWrite(LED_SEG_G_PIN, HIGH);
        break;

      case 15: // F: a, e, f, g
        digitalWrite(LED_SEG_A_PIN, HIGH);
        digitalWrite(LED_SEG_E_PIN, HIGH);
        digitalWrite(LED_SEG_F_PIN, HIGH);
        digitalWrite(LED_SEG_G_PIN, HIGH);
        break;
    }
  }
}


void turnOffDisplay()
{
  digitalWrite(LED_SEG_A_PIN, LOW);
  digitalWrite(LED_SEG_B_PIN, LOW);
  digitalWrite(LED_SEG_C_PIN, LOW);
  digitalWrite(LED_SEG_D_PIN, LOW);
  digitalWrite(LED_SEG_E_PIN, LOW);
  digitalWrite(LED_SEG_F_PIN, LOW);
  digitalWrite(LED_SEG_G_PIN, LOW);
}