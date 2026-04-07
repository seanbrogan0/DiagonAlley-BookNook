#include <Arduino.h>

#include <FastLED.h>

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

// Effect control
unsigned long lastResetTime = 0;
unsigned long effectStartTime = 0;
unsigned long currentTime;
int callsThisHour = 0;
int MAX_CALLS_PER_HOUR = 10;
unsigned long nextEffectTime = 0;
bool effectRunning = false;
int currentEffect = -1;
#define NUM_EFFECTS 8 // Total effects

// ---------- Forward declarations ----------
float getStorefrontPercent(float potPercent);
void updateStorefrontVars();               // single compute hub (camelCase)

// Defaults / storefront helpers
void ReadPots();
void ShowColours();

void setup() {
  randomSeed(analogRead(A1));

  initInputs();

  nextEffectTime = millis() + 60000;

  FastLED.addLeds<NEOPIXEL, DATA_PIN_FB>(ledsfb, NUM_LEDS_FB);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_OQ>(ledsoq, NUM_LEDS_OQ);
  // FastLED.setBrightness(255); // optional global ceiling
  FastLED.clear();
}

void loop() {
  currentTime = millis();

  // Compute once-per-frame storefront variables (caps, flicker ranges)
  updateStorefrontVars();

  // Hourly call limit reset
  if (currentTime - lastResetTime > 3600000) {
    callsThisHour = 0;
    lastResetTime = currentTime;
  }

  // Auto-scheduler
  if (!effectRunning && callsThisHour < MAX_CALLS_PER_HOUR && currentTime >= nextEffectTime) {
    currentEffect = random(NUM_EFFECTS);
    effectStartTime = currentTime;
    effectRunning = true;
    callsThisHour++;

    unsigned long minInterval = 60000;
    unsigned long maxInterval = 3600000 / max(1, MAX_CALLS_PER_HOUR);
    nextEffectTime = currentTime + random(minInterval, maxInterval);
  }

  // Render
  if (effectRunning) {
    if (currentTime - effectStartTime < 10000) {
      runEffect(currentEffect);
    } else {
      effectRunning = false;
      currentEffect = -1;
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
