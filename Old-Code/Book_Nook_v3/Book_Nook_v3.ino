
#include <FastLED.h>

//DEFINE LED PINS
#define DATA_PIN_FB 9   // Flourish & Blotts pin
#define DATA_PIN_OQ 10  // Olivanders & QQS pin

//DEFINE ARRAY SIZES
#define NUM_LEDS_FB 2   // Flourish and Blotts 2 lights
#define NUM_LEDS_OQ 5   // 0=QQS, 1-2=Olivanders Downstairs, 3-4=Olivanders Upstairs

//DEFINE ARRAYS
CRGB ledsfb[NUM_LEDS_FB];      // FB Array
CRGB ledsoq[NUM_LEDS_OQ];      // OQ Array

// Gradient Palette for candles
DEFINE_GRADIENT_PALETTE(candles_gp) {
  0,   120, 15, 15,
  64,  255, 50, 0,
  128, 255,120, 0,
  192, 220, 90, 0,
  255, 160, 60, 0
};
CRGBPalette16 candlePalette;

// Potentiometer smoothing
const int aVARPIN = A0;
const int numReadings = 10;
int aRAY[numReadings];
int aRAYdex = 0;
int aRAYtot = 0;
int aRAYavg = 0;

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

// ---------- Centralized per-frame storefront vars
static float   potPercent = 0.0f;          // 0..100 from pot
static float   storefrontPercent = 0.0f;   // dead-zoned/clamped 0..100
static uint8_t oliCap = 180;               // Olivanders cap (Option A: 40..255)
static uint8_t qsCap  = 108;               // QQS = ratio of oliCap
static uint8_t fbCap  = 108;               // F&B = ratio of oliCap
static uint8_t upCap  = 72;                // Upstairs = ratio of oliCap
static uint8_t minFlicker = 10;            // shared flicker min
static uint8_t maxFlicker = 80;            // shared flicker max

// ---------- Forward declarations ----------
float getStorefrontPercent(float potPercent);
void updateStorefrontVars();               // single compute hub (camelCase)
void runEffect(int index);

// spells
void Lumos();
void Battle();
void WingardiumLeviosa();
void HouseColours();
void ExpectoPatronum();
void AvadaKedavra();
void Incendio();
void Sectumsempra();

// Defaults / storefront helpers
void runDefaultAnimation();
void FlourishAndBlotts();
void QuidditchSupplies();
void ReadPots();
void UpstairsOlivanders();
void ShowColours();

void setup() {
  randomSeed(analogRead(A1));
  candlePalette = candles_gp;
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
    ReadPots();
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
  potPercent = (float)aRAYavg / 1023.0f * 100.0f;

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

void runEffect(int index) {
  switch (index) {
    case 0: Lumos(); break;
    case 1: Battle(); break;
    case 2: WingardiumLeviosa(); break;
    case 3: HouseColours(); break;
    case 4: ExpectoPatronum(); break;
    case 5: AvadaKedavra(); break;
    case 6: Incendio(); break;
    case 7: Sectumsempra(); break;
  }
}

// ==================== spells ====================
void Lumos() {
  uint32_t elapsed = millis() - effectStartTime;
  uint8_t oscillation = beatsin8(45, 0, 40);
  uint16_t value = constrain((oscillation * 5) + (elapsed / 100), 0, 255);
  ledsoq[1] = CRGB(value, value, value);
  ledsoq[2] = CRGB(value, value, value);
}

void Battle() {
  uint8_t sinBeat1 = beatsin8(60, 0, 50, 0, 0);
  uint8_t sinBeat2 = beatsin8(60, 0, 50, 0, 127);
  EVERY_N_MILLISECONDS(10) {
    ledsoq[1] = CHSV(120, sinBeat1 * (-1), sinBeat1 - 10);
    ledsoq[2] = CHSV(0,   sinBeat2 * (-1), sinBeat2 - 20);
  }
}

void WingardiumLeviosa() {
  const uint8_t H_LAVENDER = 230;
  const uint8_t SAT_BASE   = 200;
  const uint8_t SAT_PULSE  = 220;

  const uint16_t PULSE_PERIOD_MS = 333;
  const uint16_t PULSE_WIDTH_MS  = 90;
  const uint16_t ACCENT_PERIOD_MS= 4000;
  const uint16_t ACCENT_WIDTH_MS = 90;

  const uint8_t PULSE_MAX_RAW = 180;
  const uint8_t ACCENT_MAX_RAW= 140;
  const uint8_t LIFT_BASE_RAW = 80;
  const uint8_t BREATH_BPM    = 56;

  // Convert oliCap (0..255) to scale8 domain (0..255)
  uint8_t userScale = map(oliCap, 0, 255, 0, 255);

  uint8_t breath1Raw = beatsin8(BREATH_BPM, 0, 255, 0, 0);
  uint8_t breath2Raw = beatsin8(BREATH_BPM, 0, 255, 0, 120);
  uint8_t breath1 = scale8(breath1Raw, userScale);
  uint8_t breath2 = scale8(breath2Raw, userScale);

  ledsoq[1] = CHSV(H_LAVENDER, SAT_BASE, breath1);
  ledsoq[2] = CHSV(H_LAVENDER, SAT_BASE, breath2);

  uint8_t liftScaled = scale8(LIFT_BASE_RAW, userScale);
  ledsoq[1] += CHSV(H_LAVENDER, SAT_BASE, liftScaled);
  ledsoq[2] += CHSV(H_LAVENDER, SAT_BASE, liftScaled);

  uint32_t now = millis();
  uint16_t phase = now % PULSE_PERIOD_MS;
  bool pulseOnWindow1 = ((now / PULSE_PERIOD_MS) % 2 == 0);
  if (phase < PULSE_WIDTH_MS) {
    uint8_t t = map(phase, 0, PULSE_WIDTH_MS, 0, 255);
    uint8_t env = sin8(t);
    uint8_t pulseMax = scale8(PULSE_MAX_RAW, userScale);
    uint8_t pulseV   = scale8(pulseMax, env);
    uint8_t target = pulseOnWindow1 ? 1 : 2;
    ledsoq[target] += CHSV(H_LAVENDER, SAT_PULSE, pulseV);
  }

  uint16_t acc = now % ACCENT_PERIOD_MS;
  if (acc < ACCENT_WIDTH_MS) {
    uint8_t t = map(acc, 0, ACCENT_WIDTH_MS, 0, 255);
    uint8_t env = sin8(t);
    uint8_t accentMax = scale8(ACCENT_MAX_RAW, userScale);
    uint8_t accentV   = scale8(accentMax, env);
    ledsoq[1] += CHSV(H_LAVENDER, SAT_PULSE, accentV);
    ledsoq[2] += CHSV(H_LAVENDER, SAT_PULSE, accentV);
  }

  ledsoq[1].fadeToBlackBy(6);
  ledsoq[2].fadeToBlackBy(6);
}

void AvadaKedavra() {
  static unsigned long lastFlash = 0;
  static bool flashing = false;
  static int  flashLED = 1; // 1 or 2
  unsigned long now = millis();

  if (flashing) {
    ledsoq[flashLED] = CRGB::DarkRed;
    ledsoq[(flashLED == 1) ? 2 : 1] = CHSV(100, 255, random(150, 255));
    if (now - lastFlash > 100) flashing = false;
  } else {
    ledsoq[1] = CHSV(100, 255, random(150, 255));
    ledsoq[2] = CHSV(100, 255, random(150, 255));
    if (random8() < 5) {
      flashing = true;
      flashLED = (random8() < 128) ? 1 : 2;
      lastFlash = now;
    }
  }
}

void Sectumsempra() {
  static unsigned long lastFlash = 0;
  static bool flashing = false;
  static int  flashLED = 1;
  unsigned long now = millis();

  if (flashing) {
    ledsoq[flashLED] = CHSV(100, 255, 255);
    ledsoq[(flashLED == 1) ? 2 : 1] = CRGB::DarkRed;
    if (now - lastFlash > 100) flashing = false;
  } else {
    ledsoq[1] = CRGB::DarkRed;
    ledsoq[2] = CRGB::DarkRed;
    if (random8() < 5) {
      flashing = true;
      flashLED = (random8() < 128) ? 1 : 2;
      lastFlash = now;
    }
  }
}

void HouseColours() {
  unsigned long elapsed = millis() - effectStartTime;
  const unsigned long totalDuration = 10000;
  const int numColors = 4;
  const unsigned long segmentDuration = totalDuration / numColors;
  CRGB colors[numColors] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow};
  int currentSegment = elapsed / segmentDuration;
  int nextSegment = (currentSegment + 1) % numColors;
  float progress = (elapsed % segmentDuration) / (float)segmentDuration;
  CRGB currentColor = blend(colors[currentSegment], colors[nextSegment], (uint8_t)(progress * 255));
  ledsoq[1] = currentColor;
  ledsoq[2] = currentColor;
}

void ExpectoPatronum() {
  uint8_t pulse = beatsin8(20, 80, 255);
  ledsoq[1] = CRGB(0, pulse / 2, pulse);
  ledsoq[2] = CRGB(0, pulse / 2, pulse);
}

void Incendio() {
  for (int i = 1; i <= 2; i++) {
    if (random8() < 20) {
      ledsoq[i] = CRGB::White;
    } else {
      ledsoq[i] = CRGB(random(220, 255), random(80, 120), 0);
    }
  }
}

// ==================== Default animations & helpers ====================
void runDefaultAnimation() {
  // Idle base glow: warm amber at Olivanders cap
  // QQS (0) and Upstairs (3-4) will overwrite with their own updaters each frame.
  fill_solid(ledsoq, NUM_LEDS_OQ, CHSV(20, 95, oliCap));
}

void FlourishAndBlotts() {
  for (uint8_t i = 0; i < NUM_LEDS_FB; i++) {
    uint8_t v = random(minFlicker, maxFlicker);
    if (v > fbCap) v = fbCap;
    ledsfb[i] = ColorFromPalette(candlePalette, random8(), v);
  }
}

void QuidditchSupplies() {
  // QQS uses 60% of Olivanders cap
  ledsoq[0] = CHSV(20, 95, qsCap);
}

void UpstairsOlivanders() {
  // Upstairs uses 40% of Olivanders cap, candle-like flicker
  for (uint8_t i = 3; i < NUM_LEDS_OQ; i++) {
    uint8_t v = random(minFlicker, maxFlicker);
    if (v > upCap) v = upCap;
    ledsoq[i] = ColorFromPalette(candlePalette, random8(), v);
  }
}

void ReadPots() {
  aRAYtot = aRAYtot - aRAY[aRAYdex];
  aRAY[aRAYdex] = analogRead(aVARPIN);
  aRAYtot = aRAYtot + aRAY[aRAYdex];
  aRAYdex++;
  if (aRAYdex >= numReadings) aRAYdex = 0;
  aRAYavg = aRAYtot / numReadings;
}

void ShowColours() {
  FastLED.show();
}
