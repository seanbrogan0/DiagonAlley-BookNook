# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Arduino firmware (C++) for a Diagon Alley–themed book nook: addressable LEDs (FastLED) provide always-on ambient storefront lighting plus randomly scheduled animated "spell" effects. Target hardware is an Arduino Nano (ATmega328), with a single potentiometer as user input. The `BookNook_Parts/` STLs and `images/` are supporting assets, not code.

This is a PlatformIO-native project — it is not meant to be built with the Arduino IDE.

## Commands

Requires the PlatformIO CLI (`pio`). The single environment is `nanoatmega328` (see `platformio.ini`).

- Build: `pio run`
- Upload to board: `pio run -t upload`
- Serial monitor: `pio device monitor`
- Clean: `pio run -t clean`
- Tests: `pio test` (the `test/` directory currently contains no tests)

The only library dependency is FastLED, resolved automatically by PlatformIO from `platformio.ini`.

## Architecture

The firmware is deliberately layered, with strict separation of concerns. Each module's header in `include/` documents its ownership rules — preserve these boundaries when making changes:

- **`src/main.cpp`** — orchestration only. Owns the frame loop: read inputs → derive brightness/flicker parameters → update scheduler → render spell effect *or* ambient animation → run always-on storefront overlays → `FastLED.show()`. Also owns hardware pin assignments (data pins 9/10, pot on A0 in input.cpp). Contains no animation, scheduling, or smoothing logic.
- **`src/scheduler.cpp`** — decides *when* spell effects start/end. Spells are spread evenly across the hour (`EFFECT_INTERVAL_MS` ± `EFFECT_INTERVAL_JITTER_MS` between starts) and drawn from a shuffled deck so every effect plays exactly once per cycle with no repeats. Enforces `EFFECT_DURATION_MS`. No LED access.
- **`src/effects.cpp`** — defines *what* the 11 spell effects look like (Lumos, Battle, WingardiumLeviosa, etc., dispatched via `runEffect(index)`). Writes to LED buffers only; no scheduling or hardware setup. Every spell renders exclusively on the Ollivanders downstairs pair (`ledsoq[1]`–`ledsoq[2]`); all other zones stay in ambient mode during a spell.
- **`src/storefront.cpp`** — persistent ambient lighting (candle flicker, per-storefront layers). Runs every frame regardless of spell state; never clears LEDs globally.
- **`src/input.cpp`** — potentiometer reading with ring-buffer smoothing. Self-contained; exposes `getPotValue()` (0–1023) and nothing visual.

Shared state and configuration:

- **`include/config.h`** — single source of truth for all compile-time tuning: LED counts, effect count/timing/rate limits, brightness ratios, flicker ranges. Tweak behaviour here first, not in module code. Must never hold runtime state.
- **`include/globals.h` / `src/globals.cpp`** — all shared *runtime* state (LED buffers, scheduler flags, brightness caps, flicker window), declared `extern` in the header and defined once in globals.cpp. Anything updated in `loop()` and shared across modules belongs here, not in config.h.

LED zone mapping (must stay aligned with the physical build and `config.h` counts): `ledsfb` (pin 9, 2 LEDs) is Flourish & Blotts; `ledsoq` (pin 10, 5 LEDs) is index 0 = Quality Quidditch Supplies, 1–2 = Ollivanders downstairs, 3–4 = upstairs windows.

Brightness model: the pot maps to a master Ollivanders brightness cap (`oliCap`), and the other storefronts are fixed percentage ratios of it (`QS_RATIO`, `FB_RATIO`, `UP_RATIO`); flicker min/max ranges also scale with the pot. Effects and storefront code must respect these caps rather than writing absolute brightness.

Frame-loop render order matters: spell/ambient rendering happens first, then the storefront overlays (Flourish & Blotts every 40 ms, upstairs every 45 ms via `EVERY_N_MILLISECONDS`, QQS every frame), so overlays always win on the zones they own. `runDefaultAnimation()` deliberately fills only `ledsoq[1]`–`ledsoq[2]` — filling other indices there would stomp the overlay levels between their update ticks.

## Adding a spell effect

Three files must change together: bump `NUM_EFFECTS` in `include/config.h` (this automatically re-spreads slots across the hour, since `EFFECT_INTERVAL_MS` is derived from it), declare the function in `include/effects.h`, and implement it plus add a `case` to the `runEffect()` dispatcher in `src/effects.cpp`. The scheduler needs no changes — its shuffled deck sizes itself from `NUM_EFFECTS`.

## Hardware notes

`setup()` seeds `random()` from `analogRead(A1)` — A1 must remain a floating (unconnected) pin. All timing is non-blocking `millis()`-based; never add `delay()` calls to the frame loop.
