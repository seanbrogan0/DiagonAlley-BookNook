/***********************************************************************
 * scheduler.cpp
 *
 * Purpose:
 * --------
 * Implements the scheduling policy for spell effects.
 *
 * This module:
 *  - Decides WHEN a spell starts
 *  - Decides WHEN a spell ends
 *  - Spreads spells evenly across the hour (jittered slots)
 *  - Guarantees each spell plays once per cycle (shuffled deck)
 *
 * It does NOT:
 *  - Render LEDs
 *  - Contain visual logic
 *  - Contain hard-coded tuning values
 *
 * All tuning values come from config.h
 **********************************************************************/

#include <Arduino.h>

#include "scheduler.h"
#include "globals.h"
#include "config.h"

// =====================================================================
// Internal scheduler state (runtime only)
// =====================================================================

static unsigned long nextEffectTime = 0;
static unsigned long currentTime    = 0;

// Shuffled deck of effect indices: each effect is drawn exactly once
// per NUM_EFFECTS-spell cycle, so nothing repeats within the hour.
static uint8_t deck[NUM_EFFECTS];
static uint8_t deckPos    = NUM_EFFECTS;   // >= NUM_EFFECTS forces a shuffle
static int     lastEffect = -1;

// =====================================================================
// Deck handling
// =====================================================================

static void shuffleDeck() {
  for (uint8_t i = 0; i < NUM_EFFECTS; i++) {
    deck[i] = i;
  }

  // Fisher-Yates shuffle
  for (uint8_t i = NUM_EFFECTS - 1; i > 0; i--) {
    uint8_t j = random(i + 1);
    uint8_t t = deck[i];
    deck[i]   = deck[j];
    deck[j]   = t;
  }
}

static int drawEffect() {
  if (deckPos >= NUM_EFFECTS) {
    shuffleDeck();
    deckPos = 0;

    // Avoid a back-to-back repeat across the deck boundary
    if (deck[0] == lastEffect && NUM_EFFECTS > 1) {
      uint8_t j = random(1, NUM_EFFECTS);
      uint8_t t = deck[0];
      deck[0]   = deck[j];
      deck[j]   = t;
    }
  }

  return deck[deckPos++];
}

// =====================================================================
// Public API
// =====================================================================

void initScheduler() {
  currentTime    = millis();
  nextEffectTime = currentTime + FIRST_EFFECT_DELAY_MS;

  deckPos       = NUM_EFFECTS;
  lastEffect    = -1;

  effectRunning = false;
  currentEffect = -1;
}

void updateScheduler() {
  currentTime = millis();

  // --------------------------------------------------
  // Start the next effect when its slot arrives
  // --------------------------------------------------
  if (!effectRunning && currentTime >= nextEffectTime) {

    currentEffect   = drawEffect();
    lastEffect      = currentEffect;
    effectStartTime = currentTime;
    effectRunning   = true;

    // Next slot: one evenly sized share of the hour, jittered so
    // the gaps flex rather than tick like a clock.
    long jitter = random(-(long)EFFECT_INTERVAL_JITTER_MS,
                          (long)EFFECT_INTERVAL_JITTER_MS + 1);

    nextEffectTime = currentTime + EFFECT_INTERVAL_MS + jitter;
  }

  // --------------------------------------------------
  // Stop effect after duration expires
  // --------------------------------------------------
  if (effectRunning &&
      currentTime - effectStartTime > EFFECT_DURATION_MS) {

    effectRunning = false;
    currentEffect = -1;
  }
}

bool isEffectActive() {
  return effectRunning;
}

int getActiveEffect() {
  return currentEffect;
}
