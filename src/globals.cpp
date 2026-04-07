/***********************************************************************
 * globals.cpp
 *
 * Purpose:
 * --------
 * Provides the SINGLE definition of all shared runtime globals.
 *
 * If it’s declared in globals.h, it MUST be defined here.
 *
 * This file should contain:
 *  - Variable definitions ONLY
 *  - NO logic
 **********************************************************************/

#include <Arduino.h>
#include <FastLED.h>

#include "globals.h"
#include "config.h"

// ---------------------------------------------------------------------
// LED buffers
// ---------------------------------------------------------------------
CRGB ledsfb[NUM_LEDS_FB];
CRGB ledsoq[NUM_LEDS_OQ];

// ---------------------------------------------------------------------
// Scheduler state
// ---------------------------------------------------------------------
bool effectRunning = false;
int  currentEffect = -1;
unsigned long effectStartTime = 0;

// ---------------------------------------------------------------------
// Brightness caps
// ---------------------------------------------------------------------
uint8_t oliCap = OLI_MAX_BRIGHTNESS;
uint8_t qsCap  = 0;
uint8_t fbCap  = 0;
uint8_t upCap  = 0;

// ---------------------------------------------------------------------
// Flicker windows
// ---------------------------------------------------------------------
uint8_t minFlicker = FLICKER_MIN_LO;
uint8_t maxFlicker = FLICKER_MAX_HI;

// ---------------------------------------------------------------------
// Storefront scaling
// ---------------------------------------------------------------------
float potPercent = 0.0f;
float storefrontPercent = 0.0f;