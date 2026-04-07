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
 *  - Enforces rate limits using config values
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

static unsigned long lastResetTime  = 0;
static unsigned long nextEffectTime = 0;
static unsigned long currentTime    = 0;
static int callsThisHour            = 0;

// =====================================================================
// Public API
// =====================================================================

void initScheduler() {
  currentTime     = millis();
  lastResetTime   = currentTime;
  nextEffectTime  = currentTime + MIN_EFFECT_INTERVAL_MS;
  callsThisHour   = 0;

  effectRunning   = false;
  currentEffect   = -1;
}

void updateScheduler() {
  currentTime = millis();

  // --------------------------------------------------
  // Hourly reset for rate limiting
  // --------------------------------------------------
  if (currentTime - lastResetTime > 3600000UL) {
    callsThisHour = 0;
    lastResetTime = currentTime;
  }

  // --------------------------------------------------
  // Start a new effect if allowed
  // --------------------------------------------------
  if (!effectRunning &&
      callsThisHour < MAX_CALLS_PER_HOUR &&
      currentTime >= nextEffectTime) {

    currentEffect  = random(NUM_EFFECTS);
    effectStartTime = currentTime;
    effectRunning   = true;
    callsThisHour++;

    unsigned long maxInterval =
        3600000UL / max(1, MAX_CALLS_PER_HOUR);

    nextEffectTime =
        currentTime + random(MIN_EFFECT_INTERVAL_MS, maxInterval);
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