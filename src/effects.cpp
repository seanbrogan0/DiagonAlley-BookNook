/***********************************************************************
 * effects.cpp
 *
 * Purpose:
 * --------
 * Implements ALL animated LED effects ("spells").
 *
 * Responsibilities:
 * -----------------
 * - Update LED buffers only
 * - Use shared state for timing / brightness
 *
 * Explicitly does NOT:
 * --------------------
 * - Choose when effects run
 * - Manage scheduling
 * - Read inputs
 * - Initialize hardware
 *
 * All effects assume:
 * -------------------
 * - LED buffers are valid
 * - Brightness caps are precomputed
 * - Timing variables are up-to-date
 **********************************************************************/

#include <Arduino.h>
#include <FastLED.h>

#include "effects.h"
#include "globals.h"

// =====================================================================
// Effect dispatcher
// =====================================================================
void runEffect(int index) {
  switch (index) {
    case 0: Lumos(); break;
    case 1: Battle(); break;
    case 2: WingardiumLeviosa(); break;
    case 3: HouseColours(); break;
    case 4: ExpectoPatronum(); break;
    case 5: AvadaKedavra(); break;
    case 6: Incendio(); break;
    case 7: Sectumsempra(); break;
  }
}

// =====================================================================
// SPELL EFFECT IMPLEMENTATIONS
// =====================================================================

void Lumos() {
  // Gentle white bloom from centre LEDs
  uint32_t elapsed = millis() - effectStartTime;
  uint8_t oscillation = beatsin8(45, 0, 40);
  uint16_t value = constrain((oscillation * 5) + (elapsed / 100), 0, 255);

  ledsoq[1] = CRGB(value, value, value);
  ledsoq[2] = CRGB(value, value, value);
}

void Battle() {
  // Opposing red/green pulses
  uint8_t sinBeat1 = beatsin8(60, 0, 50, 0, 0);
  uint8_t sinBeat2 = beatsin8(60, 0, 50, 0, 127);

  EVERY_N_MILLISECONDS(10) {
    ledsoq[1] = CHSV(120, sinBeat1 * (-1), sinBeat1 - 10);
    ledsoq[2] = CHSV(0,   sinBeat2 * (-1), sinBeat2 - 20);
  }
}

void WingardiumLeviosa() {
  // Lavender breathing + pulses
  const uint8_t H_LAVENDER = 230;
  const uint8_t SAT_BASE   = 200;
  const uint8_t SAT_PULSE  = 220;

  const uint16_t PULSE_PERIOD_MS  = 333;
  const uint16_t PULSE_WIDTH_MS   = 90;
  const uint16_t ACCENT_PERIOD_MS = 4000;
  const uint16_t ACCENT_WIDTH_MS  = 90;

  const uint8_t PULSE_MAX_RAW  = 180;
  const uint8_t ACCENT_MAX_RAW = 140;
  const uint8_t LIFT_BASE_RAW  = 80;
  const uint8_t BREATH_BPM     = 56;

  uint8_t userScale = map(oliCap, 0, 255, 0, 255);

  uint8_t breath1 = scale8(beatsin8(BREATH_BPM), userScale);
  uint8_t breath2 = scale8(beatsin8(BREATH_BPM, 0, 255, 0, 120), userScale);

  ledsoq[1] = CHSV(H_LAVENDER, SAT_BASE, breath1);
  ledsoq[2] = CHSV(H_LAVENDER, SAT_BASE, breath2);

  uint8_t lift = scale8(LIFT_BASE_RAW, userScale);
  ledsoq[1] += CHSV(H_LAVENDER, SAT_BASE, lift);
  ledsoq[2] += CHSV(H_LAVENDER, SAT_BASE, lift);
}

void AvadaKedavra() {
  static unsigned long lastFlash = 0;
  static bool flashing = false;
  static int flashLED = 1;

  unsigned long now = millis();

  if (flashing) {
    ledsoq[flashLED] = CRGB::DarkRed;
    if (now - lastFlash > 100) flashing = false;
  } else {
    ledsoq[1] = CHSV(100, 255, random(150, 255));
    ledsoq[2] = CHSV(100, 255, random(150, 255));

    if (random8() < 5) {
      flashing = true;
      flashLED = random8() < 128 ? 1 : 2;
      lastFlash = now;
    }
  }
}

void Sectumsempra() {
  static unsigned long lastFlash = 0;
  static bool flashing = false;
  static int flashLED = 1;

  unsigned long now = millis();

  if (flashing) {
    ledsoq[flashLED] = CHSV(100, 255, 255);
    if (now - lastFlash > 100) flashing = false;
  } else {
    ledsoq[1] = CRGB::DarkRed;
    ledsoq[2] = CRGB::DarkRed;

    if (random8() < 5) {
      flashing = true;
      flashLED = random8() < 128 ? 1 : 2;
      lastFlash = now;
    }
  }
}

void HouseColours() {
  unsigned long elapsed = millis() - effectStartTime;

  CRGB colours[4] = {
    CRGB::Red,
    CRGB::Green,
    CRGB::Blue,
    CRGB::Yellow
  };

  int segment = elapsed / 2500;
  CRGB colour = colours[segment % 4];

  ledsoq[1] = colour;
  ledsoq[2] = colour;
}

void ExpectoPatronum() {
  uint8_t pulse = beatsin8(20, 80, 255);
  ledsoq[1] = CRGB(0, pulse / 2, pulse);
  ledsoq[2] = CRGB(0, pulse / 2, pulse);
}

void Incendio() {
  for (int i = 1; i <= 2; i++) {
    if (random8() < 20)
      ledsoq[i] = CRGB::White;
    else
      ledsoq[i] = CRGB(random(220, 255), random(80, 120), 0);
  }
}