#pragma once
/***********************************************************************
 * input.h
 *
 * Purpose:
 * --------
 * Declares all functions related to USER INPUT handling.
 *
 * This module is responsible for:
 *  - Reading raw analog inputs
 *  - Applying smoothing / averaging
 *  - Maintaining local input state
 *
 * This module explicitly does NOT:
 *  - Control scheduling
 *  - Modify brightness caps directly
 *  - Perform any LED output
 *
 * Architectural note:
 * -------------------
 * The goal is to keep ALL hardware input logic contained here,
 * so future changes (buttons, encoders, sensors) do NOT affect
 * system flow or visuals.
 **********************************************************************/

// Initialise input hardware / buffers
void initInputs();

// Read and update all user inputs (called once per loop)
void readInputs();

// Retrieve latest averaged potentiometer value (0–1023)
int getPotValue();