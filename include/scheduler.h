#pragma once
/***********************************************************************
 * scheduler.h
 *
 * Purpose:
 * --------
 * Declares all functions related to EFFECT SCHEDULING.
 *
 * The scheduler is responsible for:
 *  - Deciding WHEN spell effects start
 *  - Enforcing time limits and cooldowns
 *  - Deciding WHEN to return to idle state
 *
 * It explicitly does NOT:
 *  - Render LEDs
 *  - Read user input
 *  - Define visual behaviour
 *
 * Ownership:
 * ----------
 * - Implementation: scheduler.cpp
 * - State storage: globals.h / main.cpp
 * - Invocation: main.cpp
 *
 * This separation allows future changes to:
 *  - effect durations
 *  - cooldown behaviour
 *  - scheduling strategies
 * without touching visual code.
 **********************************************************************/

// Initialise the scheduler (called once in setup)
void initScheduler();

// Update scheduler state (called once per loop)
void updateScheduler();

// Returns true if a spell effect should currently be running
bool isEffectActive();

// Returns the index of the currently selected effect
int getActiveEffect();
