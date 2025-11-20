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

const byte SEG_PINS[7] = {
  LED_SEG_A_PIN, // bit0
  LED_SEG_B_PIN, // bit1
  LED_SEG_C_PIN, // bit2
  LED_SEG_D_PIN, // bit3
  LED_SEG_E_PIN, // bit4
  LED_SEG_F_PIN, // bit5
  LED_SEG_G_PIN  // bit6
};


const uint8_t DIGIT_MASK[16] = {
  0x3F, // 0 
  0x06, // 1
  0x5B, // 2
  0x4F, // 3
  0x66, // 4
  0x6D, // 5
  0x7D, // 6
  0x07, // 7
  0x7F, // 8
  0x6F, // 9
  0x77, // A (10)
  0x7C, // b (11)
  0x39, // C (12)
  0x5E, // d (13)
  0x79, // E (14)
  0x71  // F (15)
};

void setup()
{
  for (byte i = 0; i < 7; ++i) {
    pinMode(SEG_PINS[i], OUTPUT);
  }
  pinMode(LED_SEG_DOT_PIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
}

// turn off all segments and dot
void turnOffDisplay()
{
  for (byte i = 0; i < 7; ++i) {
    digitalWrite(SEG_PINS[i], LOW);
  }
  digitalWrite(LED_SEG_DOT_PIN, LOW);
}

//set segments according to 7-bit mask
void setSegmentsFromMask(uint8_t mask)
{
  for (byte i = 0; i < 7; ++i) {
    digitalWrite(SEG_PINS[i], (mask & (1 << i)) ? HIGH : LOW);
  }
}

//  blink all segments and dot (used for error)
void blinkAllSegments(byte times, unsigned long msDelay)
{
  for (byte i = 0; i < times; ++i) {
    // all segments on
    setSegmentsFromMask(0x7F);
    digitalWrite(LED_SEG_DOT_PIN, HIGH);
    delay(msDelay);
    turnOffDisplay();
    delay(msDelay);
  }
}

// blink dot n times (used for confirmation)
void blinkDot(byte times, unsigned long msDelay)
{
  for (byte i = 0; i < times; ++i) {
    digitalWrite(LED_SEG_DOT_PIN, HIGH);
    delay(msDelay);
    digitalWrite(LED_SEG_DOT_PIN, LOW);
    delay(msDelay);
  }
}


/**
 * @brief Main Arduino loop: reads input, debounces button, manages selection state machine and updates 7-segment display.
 *
 * Behavior:
 *  - Reads an analog potentiometer (POTI) and maps its 0..1023 value to a single decimal digit 0..9 (stored in potVal).
 *  - Reads a button (BUTTON) configured with INPUT_PULLUP (active LOW) and performs debounce logic:
 *      - Tracks lastButtonReading and lastDebounceTime; only accepts a stable change after debounceDelay ms.
 *      - Detects a falling edge (HIGH -> LOW) to treat a button press event.
 *  - Implements a simple three-state UI stored in the global `state`:
 *      - PICK_FIRST: user picks the first digit. On live operation the current potentiometer-mapped digit is shown.
 *      - PICK_SECOND: user picks the second digit. Shows live potentiometer value until the second digit is confirmed.
 *      - SHOW_RESULT: displays the computed result (0x0..0xF) on the 7-segment display until user resets.
 *  - On button press:
 *      - In PICK_FIRST: stores potVal into dig1 and transitions to PICK_SECOND. Logs selection via Serial.
 *      - In PICK_SECOND: stores potVal into dig2, computes unsigned sum s = (unsigned)dig1 + (unsigned)dig2.
 *          - If s > 15: error condition — logs error, blinks all segments briefly (blinkAllSegments), and resets to PICK_FIRST.
 *          - Else: saves sumResult (byte), blinks the decimal point briefly (blinkDot) as feedback, transitions to SHOW_RESULT.
 *      - In SHOW_RESULT: a button press resets the UI to PICK_FIRST and logs the reset.
 *  - Display update:
 *      - While in PICK_FIRST or PICK_SECOND: calls displayDigit(potVal) to show the live selection.
 *      - While in SHOW_RESULT: calls displayDigit(sumResult) to show the final result (0..F).
 *
 * Side effects / globals used:
 *  - Reads: POTI (analog), BUTTON (digital).
 *  - Uses and updates globals: lastButtonReading, lastDebounceTime, debounceDelay, buttonState, state,
 *    dig1, dig2, sumResult.
 *  - Calls helper functions with visible side effects: blinkAllSegments(count, ms), blinkDot(count, ms), displayDigit(value).
 *  - Writes human-readable diagnostics to Serial for selections, errors and resets.
 *  - Contains a fixed delay(50) at the end of each loop iteration to pace sampling and display updates.
 *
 * Notes / assumptions:
 *  - Button hardware uses INPUT_PULLUP so a pressed button reads LOW.
 *  - The sum is constrained to a 4-bit display (0..15). Values above 15 are treated as an error and cause a reset.
 *  - Debounce logic assumes lastButtonReading and lastDebounceTime are maintained across iterations.
 *
 * @return void
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
            // Fehler: Ergebnis zu groß -> Segmente kurz und reset
            Serial.print("Fehler: Summe zu groß: ");
            Serial.println(s);
            blinkAllSegments(6, 200);
            // Reset auf Anfang
            state = PICK_FIRST;
          } else {
            sumResult = (byte)s;
            // Punkt kurz blinken als Anzeige
            blinkDot(3, 150);
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

// Refactored displayDigit using mask table
void displayDigit(byte digit)
{
  turnOffDisplay();

  // Nur 0x0–0xF zulassen
  if (digit > 15) {
    return;
  }

  setSegmentsFromMask(DIGIT_MASK[digit]);
}