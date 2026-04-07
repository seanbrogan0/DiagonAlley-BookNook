#pragma once
/***********************************************************************
 * globals.h
 *
 * Purpose:
 * --------
 * Declares SHARED RUNTIME STATE across firmware modules.
 *
 * Rules:
 * ------
 * - Variables here are UPDATED during runtime
 * - Storage lives in globals.cpp
 * - Access is explicit via `extern`
 *
 * This prevents:
 *  - hidden dependencies
 *  - duplicated state
 *  - accidental shadowing
 **********************************************************************/

#include <FastLED.h>

// =====================================================================
// LED buffers (owned by main.cpp via globals.cpp)
// =====================================================================
extern CRGB ledsfb[];
extern CRGB ledsoq[];

// =====================================================================
// Scheduler runtime state
// =====================================================================
extern bool effectRunning;
extern int  currentEffect;
extern unsigned long effectStartTime;

// =====================================================================
// Brightness caps (updated every frame)
// =====================================================================
extern uint8_t oliCap;
extern uint8_t qsCap;
extern uint8_t fbCap;
extern uint8_t upCap;

// =====================================================================
// Flicker tuning (updated every frame)
// =====================================================================
extern uint8_t minFlicker;
extern uint8_t maxFlicker;

// =====================================================================
// Storefront scaling
// =====================================================================
extern float potPercent;
extern float storefrontPercent;