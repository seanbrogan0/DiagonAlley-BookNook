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

  // Scale effect intensity using current master brightness
  uint8_t userScale = map(oliCap, 0, 255, 0, 255);

  uint8_t breathA =
      scale8(beatsin8(BREATH_BPM, 0, 255, 0,   0), userScale);
  uint8_t breathB =
      scale8(beatsin8(BREATH_BPM, 0, 255, 0, 120), userScale);

  ledsoq[1] = CHSV(H_LAVENDER, SAT_BASE, breathA);
  ledsoq[2] = CHSV(H_LAVENDER, SAT_BASE, breathB);

  uint8_t lift = scale8(LIFT_RAW, userScale);

  ledsoq[1] += CHSV(H_LAVENDER, SAT_BASE, lift);
  ledsoq[2] += CHSV(H_LAVENDER, SAT_BASE, lift);
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
