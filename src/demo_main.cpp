/***********************************************************************
 * demo_main.cpp
 *
 * Purpose:
 * --------
 * Standalone diagnostic build: lights each ledsoq LED one at a time in
 * white, holding for a few seconds, printing its array index and the
 * zone name config.h/CLAUDE.md assign to it. Use this to confirm the
 * physical wiring order matches the documented index mapping:
 *   0 = QQS, 1-2 = Ollivanders downstairs, 3-4 = Upstairs windows
 *
 * Only built by the `demo_effects` PlatformIO environment (see
 * platformio.ini). The `nanoatmega328` environment excludes this file
 * and continues to build main.cpp exactly as before.
 **********************************************************************/

#include <Arduino.h>
#include <FastLED.h>

#include "globals.h"
#include "config.h"

#define DATA_PIN_OQ 10   // Ollivanders & QQS LEDs (same pin as production)

static const char* ZONE_NAMES[NUM_LEDS_OQ] = {
  "QQS",
  "Ollivanders downstairs (1 of 2)",
  "Ollivanders downstairs (2 of 2)",
  "Upstairs windows (1 of 2)",
  "Upstairs windows (2 of 2)"
};

static const unsigned long HOLD_MS = 3000;

static uint8_t currentIndex = 0;
static unsigned long lastSwitch = 0;

void setup() {
  Serial.begin(9600);

  FastLED.addLeds<NEOPIXEL, DATA_PIN_OQ>(ledsoq, NUM_LEDS_OQ);
  FastLED.clear();

  lastSwitch = millis();
  Serial.print("Lit index ");
  Serial.print(currentIndex);
  Serial.print(" = ");
  Serial.println(ZONE_NAMES[currentIndex]);
}

void loop() {
  if (millis() - lastSwitch >= HOLD_MS) {
    lastSwitch = millis();
    currentIndex = (currentIndex + 1) % NUM_LEDS_OQ;

    Serial.print("Lit index ");
    Serial.print(currentIndex);
    Serial.print(" = ");
    Serial.println(ZONE_NAMES[currentIndex]);
  }

  FastLED.clear();
  ledsoq[currentIndex] = CRGB::White;
  FastLED.show();
}
