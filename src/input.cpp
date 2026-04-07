/***********************************************************************
 * input.cpp
 *
 * Purpose:
 * --------
 * Implements user input handling for the Book Nook project.
 *
 * Current implementation:
 * -----------------------
 * - Single analog potentiometer
 * - Ring-buffer smoothing (moving average)
 *
 * Design intent:
 * --------------
 * - Input handling is SELF-CONTAINED
 * - No LED logic appears in this file
 * - Returned values are raw / neutral
 *
 * Future expansion:
 * -----------------
 * - Additional pots
 * - Digital buttons
 * - Encoders
 **********************************************************************/

#include <Arduino.h>
#include "input.h"

// =====================================================================
// Hardware configuration
// =====================================================================
static const int POT_PIN = A0;

// =====================================================================
// Smoothing configuration
// =====================================================================
static const int NUM_READINGS = 10;

// =====================================================================
// Internal state (PRIVATE to this module)
// =====================================================================
static int readings[NUM_READINGS];
static int indexPos = 0;
static int runningTotal = 0;
static int averagedValue = 0;

// =====================================================================
// Public API
// =====================================================================

void initInputs() {
  // Initialise smoothing buffer
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }

  indexPos = 0;
  runningTotal = 0;
  averagedValue = 0;
}

void readInputs() {
  // Remove oldest reading from total
  runningTotal -= readings[indexPos];

  // Read new value
  readings[indexPos] = analogRead(POT_PIN);

  // Add into running sum
  runningTotal += readings[indexPos];

  // Advance ring buffer
  indexPos++;
  if (indexPos >= NUM_READINGS) {
    indexPos = 0;
  }

  // Compute average
  averagedValue = runningTotal / NUM_READINGS;
}

int getPotValue() {
  return averagedValue;
}