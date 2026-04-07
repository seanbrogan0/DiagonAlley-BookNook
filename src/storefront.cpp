/***********************************************************************
 * storefront.cpp
 *
 * Purpose:
 * --------
 * Implements the persistent ambient lighting for storefronts.
 *
 * This module is responsible for:
 *  - Idle base glow
 *  - Candle flicker effects
 *  - Per‑storefront brightness scaling
 *
 * These effects are:
 *  - ALWAYS ACTIVE
 *  - Layered underneath spell animations
 *
 * Architectural intent:
 * ---------------------
 * This file represents the "world state":
 * what the book nook looks like when nothing magical is happening.
 *
 * Dependencies:
 * -------------
 * - globals.h for LED buffers & brightness caps
 * - FastLED for colour generation and palettes
 **********************************************************************/

#include <Arduino.h>
#include <FastLED.h>

#include "config.h"
#include "storefront.h"
#include "globals.h"

// =====================================================================
// Candle colour palette (used by multiple storefronts)
// =====================================================================

DEFINE_GRADIENT_PALETTE(candles_gp) {
  0,   120, 15, 15,
  64,  255, 50, 0,
  128, 255,120, 0,
  192, 220, 90, 0,
  255, 160, 60, 0
};

CRGBPalette16 candlePalette = candles_gp;

// =====================================================================
// Default idle animation
// =====================================================================

void runDefaultAnimation() {
  // Base warm amber glow applied to all Olivanders LEDs.
  // Individual storefronts will overwrite their specific LEDs afterward.
  fill_solid(ledsoq, NUM_LEDS_OQ, CHSV(20, 95, oliCap));
}

// =====================================================================
// Flourish & Blotts candles
// =====================================================================

void FlourishAndBlotts() {
  // Candle‑style flicker for the two Flourish & Blotts LEDs
  for (uint8_t i = 0; i < NUM_LEDS_FB; i++) {
    uint8_t v = random(minFlicker, maxFlicker);
    if (v > fbCap) v = fbCap;

    ledsfb[i] = ColorFromPalette(
      candlePalette,
      random8(),
      v
    );
  }
}

// =====================================================================
// Quality Quidditch Supplies
// =====================================================================

void QuidditchSupplies() {
  // QQS uses a steady warm glow scaled from Olivanders brightness
  ledsoq[0] = CHSV(20, 95, qsCap);
}

// =====================================================================
// Upstairs Olivanders candles
// =====================================================================

void UpstairsOlivanders() {
  // Upstairs shop windows flicker independently at lower brightness
  for (uint8_t i = 3; i < NUM_LEDS_OQ; i++) {
    uint8_t v = random(minFlicker, maxFlicker);
    if (v > upCap) v = upCap;

    ledsoq[i] = ColorFromPalette(
      candlePalette,
      random8(),
      v
    );
  }
}