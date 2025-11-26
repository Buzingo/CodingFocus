int LEDD = 7;
  const int LEDR = 3;
  const int LEDG = 5;
  const int LEDB = 6;
  const int LAUTI = 4;
  const int FOTI = A0;
  const int RANDOM = A1;
  int wink;
  int light;


#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494

// Rhythm factors:
// 1 = quarter note, 2 = half, 3 = dotted half, 4 = whole
// Tempo is in quarter-notes per minute.
const int tempo = 180; // adjust to taste

// Melody (72 notes total)
// Structure:
// 1) Old MacDonald had a farm
// 2) E-I-E-I-O
// 3) And on that farm he had a cow
// 4) E-I-E-I-O
// 5) With a moo-moo here ... (5x phrases)
// 6) Old MacDonald had a farm
// 7) E-I-E-I-O (final, long O)
const int melody[] = {
  // Old MacDonald had a farm
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_D4,
  // E-I-E-I-O
  NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,

  // And on that farm he had a cow
  NOTE_D4,
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_D4,
  // E-I-E-I-O
  NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,

  // With a moo-moo here
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,
  // And a moo-moo there
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,
  // Here a moo
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,
  // There a moo
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,
  // Everywhere a moo-moo
  NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4,

  // Old MacDonald had a farm
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_D4,
  // E-I-E-I-O (final)
  NOTE_B4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_G4
};

// Matching rhythm factors for each note
const int rhythm[] = {
  // Old MacDonald had a farm
  1, 1, 1, 1, 1, 1, 2,
  // E-I-E-I-O
  1, 1, 1, 1, 3,

  // And on that farm he had a cow
  1,
  1, 1, 1, 1, 1, 1, 2,
  // E-I-E-I-O
  1, 1, 1, 1, 3,

  // With a moo-moo here
  1, 1, 1, 1, 1, 1, 2,
  // And a moo-moo there
  1, 1, 1, 1, 1, 1, 2,
  // Here a moo
  1, 1, 1, 1, 1, 1, 2,
  // There a moo
  1, 1, 1, 1, 1, 1, 2,
  // Everywhere a moo-moo
  1, 1, 1, 1, 1, 1, 2,

  // Old MacDonald had a farm
  1, 1, 1, 1, 1, 1, 2,
  // E-I-E-I-O (final long note)
  1, 1, 1, 1, 4
};

const int numNotes = sizeof(melody) / sizeof(melody[0]);

  


void setup() {
  // PINS intitialisieren
    pinMode(LEDD,OUTPUT);
    pinMode(LEDR,OUTPUT);
    pinMode(LEDG,OUTPUT);
    pinMode(LEDB,OUTPUT);
    pinMode(LAUTI,OUTPUT);
    pinMode(FOTI,INPUT);
    //Wählt analogen PIN zum Zufallszahl generieren aus
    randomSeed(analogRead(RANDOM));
    Serial.begin(9600);




    digitalWrite(LEDD, HIGH);
    



}

/**
 * @brief Main Arduino loop: handles LED control, sound playback, and light/random-triggered behaviors.
 *
 * This function runs repeatedly on the Arduino and performs the following sequence each iteration:
 * - Clears the RGB LED outputs (sets LEDR, LEDG, LEDB to 0).
 * - Generates a random "wink" value in the range [1, 999] and reads an analog light sensor value from FOTI.
 * - Prints the generated wink value to Serial for debugging/monitoring.
 *
 * Event-driven behaviors:
 * - Dark condition (light < 7):
 *     - Plays a short alarm sequence by toggling a tone on LAUTI 20 times (25 ms on, 25 ms off each cycle).
 *
 * - Random "wink" condition (wink <= 50; roughly a 5% chance per loop):
 *     - Plays a sequence of predefined sounds via helper functions: playMeow(), playHiss(), playMeow(), playSong()
 *       with 300 ms delays between each sound call.
 *     - Runs three RGB color transition phases with an intermediate white flash after each step:
 *         1) Red -> Green: increments i from 0..255 and calls setColor(i, 255-i, 255) with a short delay and a
 *            white flash (setColor(255,255,255)) between steps.
 *         2) Green -> Blue: increments i from 0..255 and calls setColor(255, i, 255-i) with the same flash.
 *         3) Blue -> Red: increments i from 0..255 and calls setColor(255-i, 255, i) with the same flash.
 *
 * Timing and implementation notes:
 * - delay() calls introduce blocking delays: the loop is not responsive while delays execute.
 * - delay(2.5) is used in the color loops; on Arduino delay accepts milliseconds as unsigned long,
 *   so fractional values will be truncated—use delay(2) or delayMicroseconds() if sub-millisecond timing is required.
 * - The light threshold (7) is very low; adjust if sensitivity needs to change.
 * - random(1,1000) produces integer values in [1, 999]; the wink condition is approximately 50/999 ≈ 5.0%.
 *
 * Side effects:
 * - Modifies global hardware state (LED pins, speaker pin).
 * - Calls external helper functions: setColor(r,g,b), playMeow(), playHiss(), playSong().
 * - Emits diagnostic output via Serial.println(wink).
 *
 * Recommended improvements:
 * - Replace blocking delay-based sequencing with a non-blocking state machine using millis() for better responsiveness.
 * - Clarify and document the expected ranges and wiring for LEDR/LEDG/LEDB, FOTI, and LAUTI.
 * - Consider debouncing or smoothing the light sensor reading, and make the darkness threshold configurable.
 *
 * @return void (runs indefinitely)
 */
// --- REPLACED: simplified timing + purr + special-event logic ---
// triggers a special event roughly every EVENT_INTERVAL ms
const unsigned long EVENT_INTERVAL = 30000UL; // 30 seconds
unsigned long lastEventMillis = 0;
bool specialActive = false;
const uint8_t SONG_CHANCE_PERCENT = 20; // 20% chance to play the song instead of meow-hiss sequence

// Darkness / purr parameters
const int DARK_THRESHOLD = 7;
bool purrOn = false;
unsigned long purrNextToggle = 0;
const unsigned long PURR_TOGGLE_MS = 150; // ms for purr on/off bursts

// Replace previous complex loop/state code with this simpler logic
void loop() {
  // keep RGB off baseline
  analogWrite(LEDR, 0);
  analogWrite(LEDG, 0);
  analogWrite(LEDB, 0);

  // sample sensors
  light = analogRead(FOTI);

  unsigned long now = millis();

  // If it's time for a special event and none is running, start it
  if (!specialActive && (now - lastEventMillis >= EVENT_INTERVAL)) {
    specialActive = true;

    // decide: play full song sometimes, otherwise do meow-hiss-meow
    uint8_t r = random(100);
    if (r < SONG_CHANCE_PERCENT) {
      // playSong() is blocking — acceptable for occasional long events
      playSong();
    } else {
      // blocking sequence but short and predictable
      playMeow();
      delay(300);
      playHiss();
      delay(300);
      playMeow();
    }

    // mark event done and reset timer (slightly jitter the next interval a bit)
    lastEventMillis = now + random(0, 5000); // add up to 5s jitter
    specialActive = false;
    // ensure no lingering tone
    noTone(LAUTI);
  }

  // If no special event is running handle darkness -> purr (non-blocking)
  if (!specialActive && light < DARK_THRESHOLD) {
    if (now >= purrNextToggle) {
      purrOn = !purrOn;
      purrNextToggle = now + PURR_TOGGLE_MS;
      if (purrOn) {
        // short low tone purr
        tone(LAUTI, 120); // low-frequency purr
      } else {
        noTone(LAUTI);
      }
    }
  } else {
    // not dark or special active: ensure purr/tone stopped
    if (purrOn) {
      purrOn = false;
      noTone(LAUTI);
    }
    // small idle sleep to avoid busy-looping
    delay(10);
  }
}

/* -- Dark alarm (non-blocking) -- */
void startDarkAlarm() {
  darkCycles = 0;
  darkToneOn = false;
  darkNextToggle = millis();
  appState = STATE_DARK_ALARM;
}

void loopDarkAlarm() {
  unsigned long now = millis();
  if (now >= darkNextToggle) {
    if (!darkToneOn) {
      // turn tone on
      tone(LAUTI, 75);
      darkToneOn = true;
      darkNextToggle = now + DARK_TOGGLE_MS;
    } else {
      // turn tone off
      noTone(LAUTI);
      darkToneOn = false;
      darkCycles++;
      darkNextToggle = now + DARK_TOGGLE_MS;
    }

    if (darkCycles >= DARK_TOTAL_ON_CYCLES) {
      // finished
      noTone(LAUTI);
      appState = STATE_IDLE;
    }
  }
}

/* -- Special sequence (meow, hiss, meow, song, then colors) -- */
void startSpecialSequence() {
  specialStep = 0;
  appState = STATE_SPECIAL_SEQ;
  startMeowNB(); // start first meow
}

void loopSpecialSequence() {
  unsigned long now = millis();

  // Start sub-actions when their scheduled gap (specialNextMillis) has passed.
  if (specialStep == 0) {
    // first meow is started by startSpecialSequence()
    loopMeowNB();
  } else if (specialStep == 1) {
    // after first meow -> start hiss when gap passed
    if (!hissActive && now >= specialNextMillis) {
      startHissNB();
    }
    loopHissNB();
  } else if (specialStep == 2) {
    // after hiss -> start second meow when gap passed
    if (!meowActive && now >= specialNextMillis) {
      startMeowNB();
    }
    loopMeowNB();
  } else if (specialStep == 3) {
    // after second meow -> start song when gap passed
    if (!songPlaying && now >= specialNextMillis) {
      startSongNB();
    }
    loopSongNB();
  } else if (specialStep == 4) {
    // finished sound sequence -> start color transitions
    startColorPhases();
  }
}

/* -- Non-blocking meow approximation -- */
void startMeowNB() {
  meowActive = true;
  meowPhase = 0; // start rising
  meowCurrFreq = meowRiseStart;
  meowNextChange = millis();
  // When this meow finishes, specialStep will be incremented by loopMeowNB()
}

void loopMeowNB() {
  if (!meowActive) return;
  unsigned long now = millis();
  if (now < meowNextChange) return;

  if (meowPhase == 0) {
    // RISING: increment freq until peak
    tone(LAUTI, meowCurrFreq);
    meowCurrFreq += meowRiseStepHz;
    meowNextChange = now + meowRiseStepMs;
    if (meowCurrFreq >= meowRiseEnd) {
      meowPhase = 1;
      meowNextChange = now + 80; // hold at top ~80ms (approx)
      tone(LAUTI, meowRiseEnd);
    }
  } else if (meowPhase == 1) {
    // HOLD -> start fall
    meowPhase = 2;
    meowCurrFreq = meowRiseEnd;
    meowNextChange = now + meowFallStepMs;
  } else {
    // FALL: decrement freq until roughly 400Hz threshold then finish
    tone(LAUTI, meowCurrFreq);
    meowCurrFreq -= meowFallStepHz;
    meowNextChange = now + meowFallStepMs;
    if (meowCurrFreq <= 400) {
      noTone(LAUTI);
      meowActive = false;
      // advance special sequence step and set small gap (other code now starts next action)
      specialStep++;
      specialNextMillis = now + 300; // mirror original small delay between sounds
    }
  }
}

/* -- Non-blocking hiss approximation -- */
void startHissNB() {
  hissActive = true;
  hissEndMillis = millis() + HISS_DURATION_MS;
  hissNextBurst = millis(); // start immediately
}

void loopHissNB() {
  if (!hissActive) return;
  unsigned long now = millis();
  if (now >= hissEndMillis) {
    noTone(LAUTI);
    hissActive = false;
    // small gap, then next meow
    specialStep++;
    specialNextMillis = now + 300;
    return;
  }

  if (now >= hissNextBurst) {
    // play a short random high-frequency burst
    int freq = random(3000, 8000);
    tone(LAUTI, freq);
    // schedule end of this short burst via a simple timeout using millis
    // we use delay-like behavior but non-blocking: leave tone on for HISS_BURST_ON_MS
    unsigned long burstOff = now + HISS_BURST_ON_MS;
    // busy-waiting not used; instead set next burst time and schedule noTone at that time in future loop
    // We'll use hissNextBurst to track both start of next burst and to turn this one off:
    // store an interim marker by setting hissNextBurst = burstOff and use an extra flag
    // but to keep simple, immediately schedule next burst slightly later and turn off after small ms by using micros-based short busy-wait via a tiny non-blocking trick:
    // Instead, use delayMicroseconds because it's very short and acceptable for audio precision.
    delayMicroseconds(500); // tiny yield; keep burst audible (very small)
    noTone(LAUTI);
    hissNextBurst = now + HISS_BURST_INTERVAL_MS;
  }
}

/* -- Non-blocking song player (simple: play each melody note for 90% of noteDur, silence 10%) -- */
void startSongNB() {
  songPlaying = true;
  songIndex = 0;
  songNoteEnd = 0;
  songNextStart = millis();
  quarterNoteMs = 60000 / tempo;
}

void loopSongNB() {
  if (!songPlaying) return;
  unsigned long now = millis();
  if (songIndex >= numNotes) {
    // finished
    noTone(LAUTI);
    songPlaying = false;
    specialStep++;
    specialNextMillis = now + 300;
    return;
  }

  // If currently playing a note and its end not reached, just wait
  if (songNoteEnd != 0) {
    if (now >= songNoteEnd) {
      // end sound for this note
      noTone(LAUTI);
      // compute gap until the full note duration ends; we store songNextStart previously
      // songNextStart already set when note started
      unsigned long fullNoteEnd = songNextStart;
      // The code below ensures we wait for the remainder gap before starting next note
      songNoteEnd = 0;
      // compute when next note should start (it was set at note start)
      // advance index after gap
      songNextStart = fullNoteEnd; // already set
      // we start next when now >= songNextStart
    }
    return;
  }

  // Not currently holding a note; wait for scheduled start
  if (now < songNextStart) return;

  // Start next note
  int r = rhythm[songIndex];
  int noteDur = quarterNoteMs * r;
  int freq = melody[songIndex];
  if (freq > 0) {
    int soundDur = (noteDur * 9) / 10;
    // play the tone for soundDur
    tone(LAUTI, freq);
    songNoteEnd = now + soundDur;
    // schedule full note end (start of next note) at now + noteDur
    songNextStart = now + noteDur;
  } else {
    // rest: just skip and schedule next
    songNoteEnd = 0;
    songNextStart = now + noteDur;
  }

  songIndex++;
}

/* -- Non-blocking color transitions -- */
void startColorPhases() {
  colorPhase = 0;
  colorI = 0;
  colorNextMillis = millis();
  appState = STATE_COLOR_PHASES;
}

void loopColorPhases() {
  unsigned long now = millis();
  if (now < colorNextMillis) return;
  colorNextMillis = now + COLOR_STEP_MS;

  if (colorPhase == 0) {
    // red -> green : setColor(i,255-i,255)
    setColor(colorI, 255 - colorI, 255);
    colorI++;
    if (colorI > COLOR_MAX) {
      colorI = 0;
      colorPhase = 1;
      // short white flash between phases like original: set white then continue
      setColor(255,255,255);
    }
  } else if (colorPhase == 1) {
    // green -> blue : setColor(255,i,255-i)
    setColor(255, colorI, 255 - colorI);
    colorI++;
    if (colorI > COLOR_MAX) {
      colorI = 0;
      colorPhase = 2;
      setColor(255,255,255);
    }
  } else if (colorPhase == 2) {
    // blue -> red : setColor(255-i,255,i)
    setColor(255 - colorI, 255, colorI);
    colorI++;
    if (colorI > COLOR_MAX) {
      // finished full cycle
      setColor(255,255,255);
      // end special sequence and return to idle after a short pause
      appState = STATE_IDLE;
      specialStep = 0;
      // Optionally clear colors
      setColor(0,0,0);
    }
  }
}

/**
 * @brief Set the color of an RGB LED by writing PWM values to each channel.
 *
 * Writes the provided red, green, and blue intensity values to the corresponding
 * LED pins using analogWrite(). The identifiers LEDR, LEDG and LEDB are expected
 * to be defined elsewhere (e.g., as pin numbers).
 *
 * @param r Red channel intensity (0–255). 255 = off, 0 = full brightness.
 * @param g Green channel intensity (0–255). 255 = off, 0 = full brightness.
 * @param b Blue channel intensity (0–255). 255 = off, 0 = full brightness.
 *
 * @note Values outside the 0–255 range are not explicitly clamped here;
 *       behavior depends on the analogWrite() implementation. Provide values
 *       within the expected range for predictable results.
 * @note This function performs immediate, non-blocking PWM writes to the pins.
 */
void setColor (int r, int g, int b){
  analogWrite(LEDR, r);
  analogWrite(LEDG, g);
  analogWrite(LEDB, b);
}

/**
 * @brief Play a short "meow" sound effect on the LAUTI output pin.
 *
 * @details
 * This routine synthesizes a cat-like "meow" by performing a simple pitch
 * sweep: a rising frequency sequence ("Meee..."), a short sustain at the top,
 * then a falling sequence ("...ooow"). It uses the Arduino tone() function to
 * update the output frequency and delay() to control step timing.
 *
 * Behavior summary:
 *  - Rising sweep: frequencies from ~300 Hz up to ~900 Hz in small steps
 *    (fast short steps).
 *  - Hold: sustained tone at ~900 Hz for a short duration.
 *  - Falling sweep: frequencies from ~900 Hz down to ~400 Hz in larger steps
 *    (slower steps).
 *  - Ends by calling noTone(LAUTI) to stop the output.
 *
 * Timing and duration:
 *  - Rising loop: ~61 steps × 8 ms ≈ 488 ms
 *  - Hold: 80 ms
 *  - Falling loop: ~63 steps × 10 ms ≈ 630 ms
 *  - Total approximate duration: ~1.2 seconds
 *
 * Side effects and requirements:
 *  - Blocking: the function uses delay(), so it blocks execution for its
 *    entire duration. Avoid calling from time-critical or interrupt contexts.
 *  - Requires a globally defined LAUTI pin (or macro) connected to a speaker or
 *    piezo buzzer. Ensure appropriate current-limiting/resistor and hardware.
 *  - Uses Arduino API functions: tone(pin, frequency) and noTone(pin).
 *
 * @note
 *  - Frequency range (300–900 Hz) and timing values are hard-coded; adjust
 *    step sizes and delays to change the character or duration of the meow.
 *
 * @warning
 *  - Repeated rapid calls may interrupt or overlap tones; call responsibly.
 *
 * @return void
 */
void playMeow() {
  // A "meow" is kind of a pitch bend up and then down
  // We'll approximate that with a rising then falling frequency sweep

  // "Meee..." part – rising pitch
  for (int freq = 300; freq <= 900; freq += 10) {
    tone(LAUTI, freq);
    delay(8);  // short step
  }

  // Short hold at the top
  tone(LAUTI, 900);
  delay(80);

  // "...ooow" part – falling pitch
  for (int freq = 900; freq >= 400; freq -= 8) {
    tone(LAUTI, freq);
    delay(10);
  }

  // Stop the sound
  noTone(LAUTI);
}


/**
 * playHiss()
 *
 * Produce an angry cat-like "hiss" on the LAUTI output pin using Arduino tone().
 *
 * Behavior:
 *  - Low growl intro: sweeps frequency from ~200 Hz up to (but not including) 400 Hz
 *    in 5 Hz steps, holding each step ~2 ms (≈ 80 ms total).
 *  - Main hiss burst: for ~500 ms emits rapid random high-frequency tones in the
 *    3000–10000 Hz range with ~400 µs spacing to create a noisy hiss.
 *  - Fade-out: 200 iterations of random tones in the 3000–8000 Hz range with an
 *    increasing inter-tone delay (600 µs + iteration index) to slow and decay the sound.
 *  - Ends by calling noTone(LAUTI) to stop output.
 *
 * Notes:
 *  - Uses Arduino functions: tone(), noTone(), delay(), delayMicroseconds(), millis(), random().
 *  - Assumes LAUTI is a globally defined output pin.
 *  - Blocking function: total runtime is roughly 0.7–0.8 seconds. Avoid calling from ISRs
 *    or other time-critical contexts.
 *  - For deterministic behavior across runs, initialize randomness with randomSeed() elsewhere.
 *
 * Parameters: none
 * Returns: void
 */
void playHiss() {
  // low angry growl start
  for (int freq = 200; freq < 400; freq += 5) {
    tone(LAUTI, freq);
    delay(2);
  }

  // main hiss burst
  unsigned long start = millis();
  while (millis() - start < 500) {
    int freq = random(3000, 10000);
    tone(LAUTI, freq);
    delayMicroseconds(400);
  }

  // fade out hiss
  for (int i = 0; i < 200; i++) {
    int freq = random(3000, 8000);
    tone(LAUTI, freq);
    delayMicroseconds(600 + i); // slows down
  }

  noTone(LAUTI);
}



/**
 * @brief Play the configured melody using the LAUTI output.
 *
 * This function iterates over the global melody and rhythm arrays and
 * produces tones via playMeowNote() according to the current tempo.
 * It is a blocking routine that uses Arduino delay() and noTone(), so
 * it will prevent other non-interrupt work from running while playing.
 *
 * Behavior:
 *  - Computes a quarter note duration in milliseconds as 60000 / tempo.
 *  - For each index i in [0, numNotes):
 *      - noteDur = quarterNote * rhythm[i]  (rhythm entry is a multiplier of a quarter note)
 *      - If melody[i] > 0:
 *          - soundDur = 90% of noteDur (played via playMeowNote(frequency, soundDur))
 *          - gapDur   = remaining 10% of noteDur (silence implemented with noTone() + delay(gapDur))
 *      - If melody[i] == 0:
 *          - treat as a rest: noTone() and delay(noteDur)
 *  - Ensures noTone(LAUTI) is called after the loop to stop any sound.
 *
 * Dependencies / Globals:
 *  - tempo      : beats per minute (BPM). Must be > 0 to avoid division by zero.
 *  - numNotes   : number of entries in melody/rhythm to play.
 *  - rhythm[]   : numeric multipliers relative to a quarter note (e.g., 1 = quarter, 0.5 = eighth).
 *  - melody[]   : frequencies in Hz; a value of 0 denotes a rest.
 *  - LAUTI      : output pin used with noTone().
 *  - playMeowNote(int frequency, int duration) : function used to generate the tone for a note.
 *
 * Units:
 *  - tempo is in BPM.
 *  - Durations computed and passed to delay()/playMeowNote() are in milliseconds.
 *  - melody[] entries are interpreted as frequencies in hertz (Hz).
 *
 * Notes & Recommendations:
 *  - This function blocks and uses delay(); for non-blocking playback consider reworking to use
 *    millis()-based scheduling or timer interrupts.
 *  - Ensure tempo is validated (e.g., 20..300 BPM) before calling to prevent extremely long/short durations.
 *  - If fractional rhythm values are required, ensure the rhythm array and arithmetic handle fractions
 *    properly (current implementation multiplies integers; adjust types if needed).
 *  - Rests are supported by setting melody[i] to 0.
 *
 * @return void
 */
void playSong() {
  int quarterNote = 60000 / tempo; // ms per quarter note

  for (int i = 0; i < numNotes; i++) {
    int noteDur = quarterNote * rhythm[i];

    int freq = melody[i];
    if (freq > 0) {
      // Use 90% of duration for the meow, 10% as gap
      int soundDur = (noteDur * 9) / 10;
      int gapDur   = noteDur - soundDur;

      playMeowNote(freq, soundDur);
      noTone(LAUTI);
      delay(gapDur);
    } else {
      // If you ever add rests (freq = 0)
      noTone(LAUTI);
      delay(noteDur);
    }
  }

  noTone(LAUTI);
}


/**
 * playMeowNote(int baseFreq, int totalDurMs)
 *
 * Play a short "meow"-shaped tone on the LAUTI output using Arduino tone() and delay().
 *
 * Parameters:
 *   baseFreq    - Fundamental frequency in Hz around which the meow is shaped (expected > 0).
 *   totalDurMs  - Total duration of the note in milliseconds (non-negative).
 *
 * Behavior:
 *   - For very short notes (totalDurMs < 30 ms) the function simply plays baseFreq for the full duration.
 *   - For longer notes, the total duration is divided into three parts:
 *       * Rise  (≈30% of totalDurMs): frequency sweeps from startFreq (~0.8 * baseFreq) up to peakFreq (~1.125 * baseFreq).
 *       * Hold  (≈40% of totalDurMs): frequency is held at baseFreq.
 *       * Fall  (remaining duration): frequency sweeps from peakFreq down to endFreq (baseFreq).
 *   - Sweeps are performed in fixed time steps (stepMs = 8 ms). Number of steps per sweep is computed as max(1, segmentDur / stepMs).
 *   - Frequencies and durations are computed using integer arithmetic; exact values/timings may differ slightly due to truncation.
 *
 * Side effects and constraints:
 *   - Uses Arduino core functions tone() and delay(); the function is blocking and will pause execution for the note duration.
 *   - Emits on the global/output pin LAUTI (caller must ensure LAUTI is defined and configured).
 *   - No return value (void).
 *   - Not thread-safe or reentrant; do not call from interrupt context that cannot tolerate delay().
 *
 * Purpose:
 *   - Create a natural-sounding short "meow" by combining a rising pitch, a sustained middle, and a falling pitch.
 */
void playMeowNote(int baseFreq, int totalDurMs) {
  if (totalDurMs < 30) {
    // Very short notes: just play straight
    tone(LAUTI, baseFreq);
    delay(totalDurMs);
    return;
  }

  // Split duration into 3 parts: rise, hold, fall
  int riseDur = (totalDurMs * 3) / 10;  // 30%
  int holdDur = (totalDurMs * 4) / 10;  // 40%
  int fallDur = totalDurMs - riseDur - holdDur; // rest

  // Frequencies for meow-shape
  int startFreq = baseFreq - baseFreq / 5; // ~0.8 * base
  int peakFreq  = baseFreq + baseFreq / 8; // ~1.125 * base
  int endFreq   = baseFreq;                // back to base

  // Step size in ms for sweeps
  const int stepMs = 8;

  // RISE: from startFreq -> peakFreq
  int riseSteps = max(1, riseDur / stepMs);
  for (int i = 0; i < riseSteps; i++) {
    int f = startFreq + (peakFreq - startFreq) * i / riseSteps;
    tone(LAUTI, f);
    delay(stepMs);
  }

  // HOLD near baseFreq (middle of the meow)
  tone(LAUTI, baseFreq);
  delay(holdDur);

  // FALL: from peakFreq -> endFreq
  int fallSteps = max(1, fallDur / stepMs);
  for (int i = 0; i < fallSteps; i++) {
    int f = peakFreq - (peakFreq - endFreq) * i / fallSteps;
    tone(LAUTI, f);
    delay(stepMs);
  }
}