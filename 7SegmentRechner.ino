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
    displayDigit(sumResult); // Ergebnis 0-F anzeigen (displayDigit unterstützt 0-15)
  }

  delay(50); // kurze Schleifenpause
}

void displayDigit(byte digit)
{
  turnOffDisplay();

  // Nur 0x0–0xF (0–15) zulassen
  if (digit > 15) {
    return;
  }

  // Ziffern 0–9 wie bisher
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