#pragma once
#include <FastLED.h>

// LED arrays
extern CRGB ledsfb[];
extern CRGB ledsoq[];

// Sizes
extern const int NUM_LEDS_FB;
extern const int NUM_LEDS_OQ;

// Effect timing
extern unsigned long effectStartTime;

// Brightness caps
extern uint8_t oliCap;
extern uint8_t qsCap;
extern uint8_t fbCap;
extern uint8_t upCap;

// Flicker
extern uint8_t minFlicker;
extern uint8_t maxFlicker;

// Scheduler
extern bool effectRunning;
extern int currentEffect;
``