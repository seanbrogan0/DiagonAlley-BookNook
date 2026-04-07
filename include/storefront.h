#pragma once
/***********************************************************************
 * storefront.h
 *
 * Purpose:
 * --------
 * Declares all ALWAYS‑ON ambient storefront lighting behaviour.
 *
 * These functions:
 *  - Provide base lighting when no spell is active
 *  - Continuously update background visuals every frame
 *  - Respect global brightness caps and flicker ranges
 *
 * Design notes:
 * -------------
 * - These are NOT scheduled effects
 * - They do NOT control timing or state
 * - They do NOT clear LEDs globally
 *
 * Ownership:
 * ----------
 * - Implementation: storefront.cpp
 * - Scheduling: main.cpp
 * - Shared state access via globals.h
 **********************************************************************/

// Base ambient animation (used when no spell is running)
void runDefaultAnimation();

// Individual storefront layers
void FlourishAndBlotts();
void QuidditchSupplies();
void UpstairsOlivanders();