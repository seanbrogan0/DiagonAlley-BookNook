/***********************************************************************
 * scheduler.cpp
 *
 * Purpose:
 * --------
 * Implements scheduling rules for spell effects.
 *
 * This file defines:
 *  - Effect duration
 *  - Effect frequency limits
 *  - Cooldown behaviour
 *
 * It does NOT implement effect visuals.
 *
 * Timing model:
 * -------------
 * - Scheduler runs continuously every loop()
 * - Uses millis() for all timing
 * - Transitions between IDLE and ACTIVE states
 *
 * Design intent:
 * --------------
 * Make scheduling predictable, testable, and configurable
 * without interleaving timing logic with rendering logic.
 **********************************************************************/

#include <Arduino.h>

#include "scheduler.h"
#include "globals.h"

// =====================================================================
// Configuration constants (scheduler policy)
// =====================================================================

static const unsigned long EFFECT_DURATION_MS = 10000;   // Spell length
static const unsigned long HOUR_MS            = 3600000;
static const unsigned long MIN_INTERVAL_MS    = 60000;   // Minimum gap

static const int MAX_CALLS_PER_HOUR = 10;
static const int NUM_EFFECTS        = 8;

// =====================================================================
// Internal scheduler state
// =====================================================================

static unsigned long lastResetTime   = 0;
static unsigned long nextEffectTime  = 0;
static unsigned long currentTime     = 0;
static int callsThisHour             = 0;

// =====================================================================
// Public API
// =====================================================================

void initScheduler() {
  currentTime = millis();
  nextEffectTime = currentTime + MIN_INTERVAL_MS;
  callsThisHour = 0;
  effectRunning = false;
  currentEffect = -1;
}

void updateScheduler() {
  currentTime = millis();

  // --------------------------------------------------
  // Hourly reset of rate limiting
  // --------------------------------------------------
  if (currentTime - lastResetTime > HOUR_MS) {
    callsThisHour = 0;
    lastResetTime = currentTime;
  }

  // --------------------------------------------------
  // Start a new effect if conditions allow
  // --------------------------------------------------
  if (!effectRunning &&
      callsThisHour < MAX_CALLS_PER_HOUR &&
      currentTime >= nextEffectTime) {

    currentEffect = random(NUM_EFFECTS);
    effectStartTime = currentTime;
    effectRunning = true;
    callsThisHour++;

    unsigned long maxInterval =
      HOUR_MS / max(1, MAX_CALLS_PER_HOUR);

    nextEffectTime =
      currentTime + random(MIN_INTERVAL_MS, maxInterval);
  }

  // --------------------------------------------------
  // Stop effect when duration expires
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