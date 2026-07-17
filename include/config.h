#pragma once
/***********************************************************************
 * config.h
 *
 * Purpose:
 * --------
 * Centralised compile‑time configuration and tuning constants.
 *
 * These values:
 *  - Rarely change at runtime
 *  - Define system limits and ratios
 *  - Are SAFE to tweak for behaviour changes
 *
 * This file should NOT contain:
 *  - State variables
 *  - Timers
 *  - Anything updated in loop()
 **********************************************************************/

#include <stdint.h>   // ✅ REQUIRED for uint8_t, uint16_t, etc.

// ---------------------------------------------------------------------
// LED layout
// ---------------------------------------------------------------------
static const int NUM_LEDS_FB = 2;
static const int NUM_LEDS_OQ = 5;

// ---------------------------------------------------------------------
// Scheduler tuning
// ---------------------------------------------------------------------
static const int NUM_EFFECTS = 11;

static const unsigned long EFFECT_DURATION_MS = 10000;

// Spells are spread evenly across each hour: one slot per effect,
// with each gap jittered so start times are not metronomic.
static const unsigned long EFFECT_INTERVAL_MS = 3600000UL / NUM_EFFECTS;
static const unsigned long EFFECT_INTERVAL_JITTER_MS = 60000;

// Delay from power-on to the first spell
static const unsigned long FIRST_EFFECT_DELAY_MS = 60000;

// ---------------------------------------------------------------------
// Brightness tuning
// ---------------------------------------------------------------------
static const uint8_t OLI_MIN_BRIGHTNESS = 40;
static const uint8_t OLI_MAX_BRIGHTNESS = 255;

// Storefront ratios (percentage of Olivanders)
static const uint8_t QS_RATIO = 60;
static const uint8_t FB_RATIO = 60;
static const uint8_t UP_RATIO = 40;

// Flicker tuning (mapped from pot)
static const uint8_t FLICKER_MIN_LO = 10;
static const uint8_t FLICKER_MIN_HI = 50;
static const uint8_t FLICKER_MAX_LO = 30;
static const uint8_t FLICKER_MAX_HI = 80;
