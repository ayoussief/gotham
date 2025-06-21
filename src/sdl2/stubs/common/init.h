// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_COMMON_INIT_H
#define GOTHAM_COMMON_INIT_H

/**
 * Initialization functions for Gotham Core
 * This is a stub implementation for the SDL2 GUI build
 */

class ArgsManager;

/**
 * Initialize Gotham Core systems
 */
bool InitGotham(ArgsManager& args);

/**
 * Shutdown Gotham Core systems
 */
void ShutdownGotham();

#endif // GOTHAM_COMMON_INIT_H