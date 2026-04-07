#pragma once
/***********************************************************************
 * globals.h
 *
 * Purpose:
 * --------
 * Declares ALL shared global state used across firmware modules.
 *
 * This file exists to:
 *  - Make shared dependencies explicit
 *  - Prevent hidden coupling between .cpp files
 *  - Clearly define who OWNS data vs who USES data
 *
 * Rule:
 * -----
 * - Variables are DECLARED here using `extern`
 * - Variables are DEFINED exactly once in main.cpp
 *
 * This keeps linkage clean and predictable.
 **********************************************************************/

#include <FastLED.h>

// ---------------------------------------------------------------------
// LED buffers (owned by main.cpp)
// ---------------------------------------------------------------------
extern CRGB ledsfb[];
extern CRGB ledsoq[];

// ---------------------------------------------------------------------
// LED strip sizes
// ---------------------------------------------------------------------
extern const int NUM_LEDS_FB;
extern const int NUM_LEDS_OQ;

// ---------------------------------------------------------------------
// Effect timing
// ---------------------------------------------------------------------
extern unsigned long effectStartTime;

// ---------------------------------------------------------------------
// Brightness caps (computed once per frame)
// ---------------------------------------------------------------------
extern uint8_t oliCap;
extern uint8_t qsCap;
extern uint8_t fbCap;
extern uint8_t upCap;

// ---------------------------------------------------------------------
// Flicker range (shared visual tuning)
// ---------------------------------------------------------------------
extern uint8_t minFlicker;
extern uint8_t maxFlicker;

// ---------------------------------------------------------------------
// Scheduler state
// ---------------------------------------------------------------------
extern bool effectRunning;
extern int  currentEffect;
``