/***********************************************************************
 * main.cpp
 *
 * Purpose:
 * --------
 * Top‑level orchestration file for Diagon Alley Book Nook firmware.
 *
 * Responsibilities:
 *  - Hardware initialisation
 *  - Per‑frame application flow
 *  - Delegating work to domain modules:
 *      - input     → readInputs()
 *      - scheduler → updateScheduler()
 *      - effects   → runEffect()
 *      - storefront→ ambient lighting
 *
 * This file intentionally contains:
 *  - NO animation logic
 *  - NO scheduling policy
 *  - NO input smoothing code
 *
 * Those concerns live in their own modules.
 **********************************************************************/

#include <Arduino.h>
#include <FastLED.h>

// ---------------------------------------------------------------------
// Module interfaces
// ---------------------------------------------------------------------
#include "scheduler.h"
#include "input.h"
#include "storefront.h"
#include "effects.h"
#include "globals.h"

// ---------------------------------------------------------------------
// Hardware configuration
// ---------------------------------------------------------------------
#define DATA_PIN_FB 9    // Flourish & Blotts LEDs
#define DATA_PIN_OQ 10   // Ollivanders & QQS LEDs

#define NUM_LEDS_FB 2    // Flourish & Blotts lights
#define NUM_LEDS_OQ 5    // 0=QQS, 1–2=Ollivanders Downstairs, 3–4=Upstairs

// ---------------------------------------------------------------------
// LED buffers (OWNED by main.cpp, shared via globals.h)
// ---------------------------------------------------------------------
CRGB ledsfb[NUM_LEDS_FB];
CRGB ledsoq[NUM_LEDS_OQ];

// ---------------------------------------------------------------------
// Forward declarations (main‑local helpers only)
// ---------------------------------------------------------------------
float getStorefrontPercent(float potPercent);
void  updateStorefrontVars();
void  ShowColours();

// =====================================================================
// SETUP
// =====================================================================
void setup() {
  randomSeed(analogRead(A1));

  initInputs();       // initialise user input smoothing
  initScheduler();    // initialise effect scheduling state

  // Initialise LED strips
  FastLED.addLeds<NEOPIXEL, DATA_PIN_FB>(ledsfb, NUM_LEDS_FB);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_OQ>(ledsoq, NUM_LEDS_OQ);
  FastLED.clear();
}

// =====================================================================
// LOOP
// =====================================================================
void loop() {

  // --------------------------------------------------
  // 1. Always read inputs first (fresh data every frame)
  // --------------------------------------------------
  readInputs();

  // --------------------------------------------------
  // 2. Update derived visual parameters
  // --------------------------------------------------
  updateStorefrontVars();

  // --------------------------------------------------
  // 3. Update scheduler state
  // --------------------------------------------------
  updateScheduler();

  // --------------------------------------------------
  // 4. Render either spell OR idle visuals
  // --------------------------------------------------
  if (isEffectActive()) {
    runEffect(getActiveEffect());
  } else {
    runDefaultAnimation();   // ambient base layer
  }

  // --------------------------------------------------
  // 5. Storefront overlays always run
  // --------------------------------------------------
  EVERY_N_MILLISECONDS(40) { FlourishAndBlotts(); }
  EVERY_N_MILLISECONDS(45) { UpstairsOlivanders(); }
  QuidditchSupplies();

  // --------------------------------------------------
  // 6. Push LEDs to hardware
  // --------------------------------------------------
  ShowColours();
}

// =====================================================================
// STORE‑LEVEL BRIGHTNESS DERIVATION
// =====================================================================

float getStorefrontPercent(float potPercentIn) {
  if (potPercentIn <= 20) return 0;
  if (potPercentIn >= 80) return 100;
  return (potPercentIn - 20) / 60.0f * 100.0f;
}

void updateStorefrontVars() {
  // Convert potentiometer reading to percentage
  potPercent = (float)getPotValue() / 1023.0f * 100.0f;

  storefrontPercent = getStorefrontPercent(potPercent);

  // Olivanders master brightness (40–255)
  oliCap = (uint8_t)map((int)storefrontPercent, 0, 100, 40, 255);

  // Per‑store ratios
  qsCap = (uint8_t)((uint16_t)oliCap * 60 / 100);
  fbCap = (uint8_t)((uint16_t)oliCap * 60 / 100);
  upCap = (uint8_t)((uint16_t)oliCap * 40 / 100);

  // Flicker tuning based on input
  int pot = getPotValue();
  minFlicker = map(pot, 0, 1023, 10, 50);
  maxFlicker = map(pot, 0, 1023, 30, 80);

  if (minFlicker > maxFlicker) {
    uint8_t t = minFlicker;
    minFlicker = maxFlicker;
    maxFlicker = t;
  }
}

// =====================================================================
// LED COMMIT
// =====================================================================
void ShowColours() {
  FastLED.show();
}