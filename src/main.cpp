#include <Arduino.h>

#include <FastLED.h>

#include "scheduler.h"
#include "input.h"
#include "storefront.h"
#include "globals.h"
#include "effects.h"

//DEFINE LED PINS
#define DATA_PIN_FB 9   // Flourish & Blotts pin
#define DATA_PIN_OQ 10  // Olivanders & QQS pin

//DEFINE ARRAY SIZES
#define NUM_LEDS_FB 2   // Flourish and Blotts 2 lights
#define NUM_LEDS_OQ 5   // 0=QQS, 1-2=Olivanders Downstairs, 3-4=Olivanders Upstairs

//DEFINE ARRAYS
CRGB ledsfb[NUM_LEDS_FB];      // FB Array
CRGB ledsoq[NUM_LEDS_OQ];      // OQ Array

// ---------- Forward declarations ----------
float getStorefrontPercent(float potPercent);
void updateStorefrontVars();               // single compute hub (camelCase)

// Defaults / storefront helpers
void ReadPots();
void ShowColours();

void setup() {
  randomSeed(analogRead(A1));

  initScheduler();
  initInputs();

  nextEffectTime = millis() + 60000;

  FastLED.addLeds<NEOPIXEL, DATA_PIN_FB>(ledsfb, NUM_LEDS_FB);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_OQ>(ledsoq, NUM_LEDS_OQ);
  // FastLED.setBrightness(255); // optional global ceiling
  FastLED.clear();
}

void loop() {
  

  // Compute once-per-frame storefront variables (caps, flicker ranges)
  updateStorefrontVars();

  updateScheduler();


  // Render
if (isEffectActive()) {
  runEffect(getActiveEffect());
}
  } else {
    readInputs();
    runDefaultAnimation();
  }

  // Storefront updates always run
  EVERY_N_MILLISECONDS(40) { FlourishAndBlotts(); }
  EVERY_N_MILLISECONDS(45) { UpstairsOlivanders(); }
  QuidditchSupplies();
  ShowColours();
}

// Dead-zone mapping (unchanged)
float getStorefrontPercent(float potPercentIn) {
  if (potPercentIn <= 20) return 0;
  if (potPercentIn >= 80) return 100;
  return (potPercentIn - 20) / 60.0 * 100.0;
}

// Centralized updater (called once per frame)
void updateStorefrontVars() {
  // Raw pot to %
  potPercent = (float)getPotValue / 1023.0f * 100.0f;

  // Dead-zoned/clamped percent (0..100)
  storefrontPercent = getStorefrontPercent(potPercent);

  // OPTION A: allow true max brightness (map 40..255)
  oliCap = (uint8_t)map((int)storefrontPercent, 0, 100, 40, 255);

  // Ratios per storefront (tune here)
  qsCap = (uint8_t)((uint16_t)oliCap * 60 / 100); // 60%
  fbCap = (uint8_t)((uint16_t)oliCap * 60 / 100); // 60%
  upCap = (uint8_t)((uint16_t)oliCap * 40 / 100); // 40%

  // Shared flicker window
  minFlicker = map(aRAYavg, 0, 1023, 10, 50);
  maxFlicker = map(aRAYavg, 0, 1023, 30, 80);
  if (minFlicker > maxFlicker) { uint8_t t = minFlicker; minFlicker = maxFlicker; maxFlicker = t; }
}



void ShowColours() {
  FastLED.show();
}
