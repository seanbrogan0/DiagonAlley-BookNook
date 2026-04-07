
#include <FastLED.h>

// -------------------- Pins & LED layout --------------------
#define DATA_PIN_FB     9   // Flourish & Blotts
#define DATA_PIN_OQ     10  // Ollivanders & QQS
#define DATA_PIN_STREET 11  // Street lamp

#define NUM_LEDS_FB      2
#define NUM_LEDS_OQ      3  // ledsoq[0] = QQS, ledsoq[1..2] = Ollivanders
#define NUM_LEDS_STREET  1

CRGB ledsfb[NUM_LEDS_FB];
CRGB ledsoq[NUM_LEDS_OQ];
CRGB ledsStreet[NUM_LEDS_STREET];

// -------------------- Test-cycle scheduler --------------------
#define NUM_EFFECTS 3
uint8_t currentEffect = 0;
unsigned long effectStartTime = 0;
const unsigned long effectDurationMs = 8000; // cycle every 8 seconds

// Forward declarations
void runEffect(uint8_t index);
void Battle();
void WingardiumLeviosa();
void Incendio();

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN_FB>(ledsfb, NUM_LEDS_FB);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_OQ>(ledsoq, NUM_LEDS_OQ);
  FastLED.addLeds<NEOPIXEL, DATA_PIN_STREET>(ledsStreet, NUM_LEDS_STREET);
  FastLED.clear(true);

  effectStartTime = millis();

  static const char* names[NUM_EFFECTS] = {
    "Battle", "WingardiumLeviosa", "Incendio"
  };
  Serial.println(F("=== Spell-Only (Ollivanders) Cycle Test — 3 Spells ==="));
  Serial.print(F("Starting with: ")); Serial.println(names[currentEffect]);
}

// -------------------- Loop --------------------
void loop() {
  // Keep all non-Ollivanders lights OFF every frame
  fill_solid(ledsfb, NUM_LEDS_FB, CRGB::Black); // F&B off
  ledsStreet[0] = CRGB::Black;                  // Street lamp off

  // Run the current spell (affects only ledsoq[1] and [2])
  runEffect(currentEffect);

  // Ensure QQS remains OFF even if a spell touches ledsoq[0]
  ledsoq[0] = CRGB::Black;

  // Push frame
  FastLED.show();

  // Advance spell after effectDurationMs
  if (millis() - effectStartTime >= effectDurationMs) {
    effectStartTime = millis();
    currentEffect = (currentEffect + 1) % NUM_EFFECTS;

    static const char* names[NUM_EFFECTS] = {
      "Battle", "WingardiumLeviosa", "Incendio"
    };
    Serial.print(F("Now running: "));
    Serial.println(names[currentEffect]);

    // Optional: clear Ollivanders windows between spells
    ledsoq[1] = CRGB::Black;
    ledsoq[2] = CRGB::Black;
  }

  delay(5); // small pacing delay
}

// -------------------- Dispatcher --------------------
void runEffect(uint8_t index) {
  switch (index) {
    case 0: Battle(); break;
    case 1: WingardiumLeviosa(); break;
    case 2: Incendio(); break;
  }
}

// -------------------- SPELLS --------------------

// Battle (kept as in your current code)
void Battle() {
  uint8_t sinBeat1 = beatsin8(60, 0, 50, 0, 0);
  uint8_t sinBeat2 = beatsin8(60, 0, 50, 0, 127);
  EVERY_N_MILLISECONDS(10) {
    // Preserving original math/feel
    ledsoq[1] = CHSV(120, sinBeat1 * (-1), sinBeat1 - 10);
    ledsoq[2] = CHSV(0,   sinBeat2 * (-1), sinBeat2 - 20);
  }
}

// Wingardium Leviosa — uniform, metered glints; bright lavender (glints dimmed)


void WingardiumLeviosa() {
  // ---- Tunables (feel free to tweak) ----
  const uint8_t  H_LAVENDER      = 230;   // lavender/violet hue (raise toward 235 for deeper violet)
  const uint8_t  SAT_BASE        = 200;   // base saturation (rich purple)
  const uint8_t  SAT_PULSE       = 220;   // pulse saturation (slightly richer than base)
  const uint8_t  FADE_PER_FRAME  = 6;     // lower = more persistence
  const uint8_t  LIFT_BASE       = 80;    // additive lift at breath troughs
  const uint8_t  BREATH_BPM      = 56;    // lavender "breathing" speed
  const uint8_t  BREATH_MIN      = 160;   // breath brightness min
  const uint8_t  BREATH_MAX      = 255;   // breath brightness max

  // Cadence & pulse envelope
  const uint16_t PULSE_PERIOD_MS  = 333;  // spacing between pulses (alternates windows)
  const uint16_t PULSE_WIDTH_MS   = 90;   // pulse length
  const uint16_t ACCENT_PERIOD_MS = 4000; // both windows pulse together
  const uint16_t ACCENT_WIDTH_MS  = 90;   // accent pulse length

  // Brightness caps (lavender only)
  const uint8_t  PULSE_MAX        = 180;  // peak for alternating pulse (lavender)
  const uint8_t  ACCENT_MAX       = 140;  // peak for synchronized accent (lavender)

  uint32_t now = millis();

  // ---- Breathing lavender base ----
  uint8_t v1 = beatsin8(BREATH_BPM, BREATH_MIN, BREATH_MAX, 0,   0);
  uint8_t v2 = beatsin8(BREATH_BPM, BREATH_MIN, BREATH_MAX, 0, 120);

  ledsoq[1] = CHSV(H_LAVENDER, SAT_BASE, v1);
  ledsoq[2] = CHSV(H_LAVENDER, SAT_BASE, v2);

  // Keep windows luminous at troughs
  ledsoq[1] += CHSV(H_LAVENDER, SAT_BASE, LIFT_BASE);
  ledsoq[2] += CHSV(H_LAVENDER, SAT_BASE, LIFT_BASE);

  // ---- Alternating lavender pulse (smooth envelope) ----
  uint16_t phase = now % PULSE_PERIOD_MS;
  bool pulseOnWindow1 = ((now / PULSE_PERIOD_MS) % 2 == 0); // alternate 1 ↔ 2

  if (phase < PULSE_WIDTH_MS) {
    // Smooth ease-in/ease-out
    uint8_t t      = map(phase, 0, PULSE_WIDTH_MS, 0, 255);
    uint8_t env    = sin8(t);                  // 0→255→0
    uint8_t pulseV = scale8(PULSE_MAX, env);   // lavender-only pulse

    uint8_t target = pulseOnWindow1 ? 1 : 2;
    ledsoq[target] += CHSV(H_LAVENDER, SAT_PULSE, pulseV);  // single-color lavender pulse
  }

  // ---- Synchronized lavender accent pulse (both windows) ----
  uint16_t acc = now % ACCENT_PERIOD_MS;
  if (acc < ACCENT_WIDTH_MS) {
    uint8_t t       = map(acc, 0, ACCENT_WIDTH_MS, 0, 255);
    uint8_t env     = sin8(t);
    uint8_t accentV = scale8(ACCENT_MAX, env);
    ledsoq[1] += CHSV(H_LAVENDER, SAT_PULSE, accentV);
    ledsoq[2] += CHSV(H_LAVENDER, SAT_PULSE, accentV);
  }

  // ---- Afterglow fade ----
  ledsoq[1].fadeToBlackBy(FADE_PER_FRAME);
  ledsoq[2].fadeToBlackBy(FADE_PER_FRAME);
}


// Incendio (kept as in your code)
void Incendio() {
  // Warm flame/sparks
  for (int i = 1; i <= 2; i++) {
    if (random8() < 20) {
      ledsoq[i] = CRGB::White; // Occasional spark
    } else {
      ledsoq[i] = CRGB(random(220, 255), random(80, 120), 0); // Brighter flames
    }
  }
}
