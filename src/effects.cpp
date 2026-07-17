/***********************************************************************
 * effects.cpp
 *
 * Purpose:
 * --------
 * Implements all spell-based LED animation effects.
 *
 * This module:
 *  - Modifies LED buffers only
 *  - Uses shared runtime state from globals.h
 *  - Does NOT perform scheduling, input handling, or FastLED.show()
 *
 * Architectural role:
 * -------------------
 * Defines "what magic looks like" while remaining stateless with
 * respect to timing policy and input sourcing.
 **********************************************************************/

#include <Arduino.h>
#include <FastLED.h>

#include "effects.h"
#include "globals.h"
#include "config.h"

// =====================================================================
// Effect dispatcher
// =====================================================================

void runEffect(int index) {
  switch (index) {
    case 0: Lumos();             break;
    case 1: Battle();            break;
    case 2: WingardiumLeviosa(); break;
    case 3: HouseColours();      break;
    case 4: ExpectoPatronum();   break;
    case 5: AvadaKedavra();      break;
    case 6: Incendio();          break;
    case 7: Sectumsempra();      break;
    case 8: Riddikulus();        break;
    case 9: Alohomora();         break;
    case 10: Reducto();          break;
    default:                     break;
  }
}

// =====================================================================
// SPELL IMPLEMENTATIONS
// =====================================================================

void Lumos() {
  /*********************************************************************
   * Lumos
   *
   * White light bloom with gentle oscillation and slow ramp-up.
   *********************************************************************/

  uint32_t elapsed = millis() - effectStartTime;

  uint8_t oscillation = beatsin8(45, 0, 40);
  uint8_t brightness =
      constrain((oscillation * 5) + (elapsed / 100), 0, 255);

  ledsoq[1] = CRGB(brightness, brightness, brightness);
  ledsoq[2] = CRGB(brightness, brightness, brightness);
}

void Battle() {
  /*********************************************************************
   * Battle
   *
   * Opposing red and green pulses simulating magical combat.
   *********************************************************************/

  uint8_t beatA = beatsin8(60, 0, 50, 0,   0);
  uint8_t beatB = beatsin8(60, 0, 50, 0, 127);

  EVERY_N_MILLISECONDS(10) {
    ledsoq[1] = CHSV(120, beatA * (-1), beatA - 10);
    ledsoq[2] = CHSV(0,   beatB * (-1), beatB - 20);
  }
}

void WingardiumLeviosa() {
  /*********************************************************************
   * Wingardium Leviosa
   *
   * Soft lavender breathing effect representing levitation.
   * Slow, calm, ambient by design — no accent pulses.
   *********************************************************************/

  const uint8_t H_LAVENDER = 230;
  const uint8_t SAT_BASE   = 200;
  const uint8_t BREATH_BPM = 56;
  const uint8_t LIFT_RAW   = 80;

  uint8_t breathA = beatsin8(BREATH_BPM, 0, 255, 0,   0);
  uint8_t breathB = beatsin8(BREATH_BPM, 0, 255, 0, 120);

  ledsoq[1] = CHSV(H_LAVENDER, SAT_BASE, breathA);
  ledsoq[2] = CHSV(H_LAVENDER, SAT_BASE, breathB);

  ledsoq[1] += CHSV(H_LAVENDER, SAT_BASE, LIFT_RAW);
  ledsoq[2] += CHSV(H_LAVENDER, SAT_BASE, LIFT_RAW);
}

void HouseColours() {
  /*********************************************************************
   * House Colours
   *
   * Cycles through Hogwarts house colours over time.
   *********************************************************************/

  static const CRGB houses[] = {
    CRGB::Red,
    CRGB::Green,
    CRGB::Blue,
    CRGB::Yellow
  };

  unsigned long elapsed = millis() - effectStartTime;
  uint8_t index = (elapsed / 2500) % 4;

  ledsoq[1] = houses[index];
  ledsoq[2] = houses[index];
}

void ExpectoPatronum() {
  /*********************************************************************
   * Expecto Patronum
   *
   * Ethereal blue-white pulse symbolising a patronus charm.
   *********************************************************************/

  uint8_t pulse = beatsin8(20, 80, 255);

  ledsoq[1] = CRGB(0, pulse / 2, pulse);
  ledsoq[2] = CRGB(0, pulse / 2, pulse);
}

void AvadaKedavra() {
  /*********************************************************************
   * Avada Kedavra
   *
   * Violent green energy with rare dark-red flashes.
   *********************************************************************/

  static bool flashing = false;
  static unsigned long flashTime = 0;
  static uint8_t flashLED = 1;

  unsigned long now = millis();

  if (flashing) {
    ledsoq[flashLED] = CRGB::DarkRed;

    if (now - flashTime > 100) {
      flashing = false;
    }
  } else {
    ledsoq[1] = CHSV(100, 255, random(150, 255));
    ledsoq[2] = CHSV(100, 255, random(150, 255));

    if (random8() < 5) {
      flashing = true;
      flashLED = random8() < 128 ? 1 : 2;
      flashTime = now;
    }
  }
}

void Incendio() {
  /*********************************************************************
   * Incendio
   *
   * Chaotic fire flicker with occasional white-hot sparks.
   *********************************************************************/

  for (uint8_t i = 1; i <= 2; i++) {
    if (random8() < 20) {
      ledsoq[i] = CRGB::White;
    } else {
      ledsoq[i] = CRGB(
        random(220, 255),
        random(80, 120),
        0
      );
    }
  }
}

void Sectumsempra() {
  /*********************************************************************
   * Sectumsempra
   *
   * Aggressive red effect with sharp white slashing accents.
   *********************************************************************/

  static bool flashing = false;
  static unsigned long flashTime = 0;
  static uint8_t flashLED = 1;

  unsigned long now = millis();

  if (flashing) {
    ledsoq[flashLED] = CHSV(0, 0, 255);

    if (now - flashTime > 100) {
      flashing = false;
    }
  } else {
    ledsoq[1] = CRGB::DarkRed;
    ledsoq[2] = CRGB::DarkRed;

    if (random8() < 5) {
      flashing = true;
      flashLED = random8() < 128 ? 1 : 2;
      flashTime = now;
    }
  }
}

void Riddikulus() {
  /*********************************************************************
   * Riddikulus
   *
   * A boggart forced through absurd forms: each LED holds a random
   * saturated colour, then snaps to a new one at irregular intervals
   * with a brief white "crack" on the change. The two LEDs always
   * wear clashing colours.
   *********************************************************************/

  static unsigned long seenStart  = 0;
  static unsigned long lastChange = 0;
  static unsigned int  holdMs     = 0;
  static uint8_t hueA = 0, hueB = 128;
  static uint8_t valA = 255, valB = 255;

  unsigned long now = millis();

  // Statics persist between activations — reset on a fresh run
  if (seenStart != effectStartTime) {
    seenStart  = effectStartTime;
    lastChange = now;
    holdMs     = 0;   // force an immediate first costume change
  }

  if (now - lastChange >= holdMs) {
    lastChange = now;
    holdMs = random(200, 701);

    hueA = random8();
    hueB = hueA + 64 + random8(128);   // wraps; always visibly different
    valA = random(180, 256);
    valB = random(180, 256);
  }

  if (now - lastChange < 40) {
    // The crack of the boggart transforming
    ledsoq[1] = CRGB::White;
    ledsoq[2] = CRGB::White;
  } else {
    ledsoq[1] = CHSV(hueA, 255, valA);
    ledsoq[2] = CHSV(hueB, 255, valB);
  }
}

void Alohomora() {
  /*********************************************************************
   * Alohomora
   *
   * A lock clicking open in three acts:
   *   0-3 s  three amber tumbler clicks, each brighter than the last
   *   3-4 s  near-dark hesitation
   *   4-10 s slow golden swell as the door opens, held with a shimmer
   *********************************************************************/

  uint32_t elapsed = millis() - effectStartTime;

  if (elapsed < 3000) {
    uint8_t  click = elapsed / 1000;    // 0..2
    uint16_t local = elapsed % 1000;

    uint8_t v = 0;
    if (local < 250) {
      // 125 ms rise, 125 ms fall
      uint8_t peak = 120 + click * 60;  // 120, 180, 240
      v = (local < 125)
            ? map(local, 0, 124, 0, peak)
            : map(local, 125, 249, peak, 0);
    }

    uint8_t hue = 30 + click * 6;       // amber drifting toward gold
    ledsoq[1] = CHSV(hue, 230, v);
    ledsoq[2] = CHSV(hue, 230, v);

  } else if (elapsed < 4000) {
    // Did it work?
    ledsoq[1] = CHSV(36, 230, 8);
    ledsoq[2] = CHSV(36, 230, 8);

  } else {
    // Light spills through the opening door
    uint8_t base = (elapsed < 7000)
                     ? map(elapsed, 4000, 6999, 8, 255)
                     : 255;

    uint8_t v = qsub8(base, beatsin8(12, 0, 30));
    ledsoq[1] = CHSV(42, 180, v);
    ledsoq[2] = CHSV(42, 180, v);
  }
}

void Reducto() {
  /*********************************************************************
   * Reducto
   *
   * Charge, blast, aftermath:
   *   0-4 s     ember-orange ramp with a tremble that grows as the
   *             charge builds, whitening toward the blast
   *   4-4.4 s   flat white explosion, second LED struck 50 ms later
   *   4.4-10 s  embers flickering under a decaying ceiling, cooling
   *             from orange toward deep red
   *********************************************************************/

  uint32_t elapsed = millis() - effectStartTime;

  if (elapsed < 4000) {
    uint8_t base       = map(elapsed, 0, 3999, 40, 220);
    uint8_t trembleAmp = map(elapsed, 0, 3999, 5, 70);
    uint8_t v          = qadd8(base, random8(trembleAmp));

    uint8_t hue = map(elapsed, 0, 3999, 10, 30);
    uint8_t sat = map(elapsed, 0, 3999, 255, 170);

    ledsoq[1] = CHSV(hue, sat, v);
    ledsoq[2] = CHSV(hue, sat, v);

  } else if (elapsed < 4400) {
    ledsoq[1] = CRGB::White;
    ledsoq[2] = (elapsed >= 4050) ? CRGB::White : CRGB(255, 160, 40);

  } else {
    // The final frame can land a moment past the nominal duration
    uint32_t t = (elapsed > 9999) ? 9999 : elapsed;

    uint8_t ceiling = map(t, 4400, 9999, 200, 4);
    uint8_t hueMax  = map(t, 4400, 9999, 24, 6);

    ledsoq[1] = CHSV(random8(hueMax), 255, random8(ceiling));
    ledsoq[2] = CHSV(random8(hueMax), 255, random8(ceiling));
  }
}
