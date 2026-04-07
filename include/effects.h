#pragma once
/***********************************************************************
 * effects.h
 *
 * Purpose:
 * --------
 * Declares all "spell" and animated LED effect functions.
 *
 * This header represents PURE BEHAVIOUR:
 *  - No setup logic
 *  - No pin configuration
 *  - No scheduling
 *  - No hardware ownership
 *
 * These functions:
 *  - Read shared state (brightness caps, timing)
 *  - Write to LED buffers
 *
 * Ownership model:
 * ----------------
 * - main.cpp owns system flow and scheduling
 * - globals.h exposes shared state
 * - effects.cpp implements visuals only
 *
 * Notes:
 * ------
 * - No FastLED includes here (keeps compile dependencies minimal)
 * - Implementations live in effects.cpp
 **********************************************************************/

// Dispatcher ------------------------------------------------------------
// Selects and runs the active spell effect
void runEffect(int index);

// Individual spell effects ---------------------------------------------
void Lumos();
void Battle();
void WingardiumLeviosa();
void HouseColours();
void ExpectoPatronum();
void AvadaKedavra();
void Incendio();
void Sectumsempra();
``