// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_COMMON_SYSTEM_H
#define GOTHAM_COMMON_SYSTEM_H

#include <string>

/**
 * System utilities for Gotham Core
 * This is a stub implementation for the SDL2 GUI build
 */

/**
 * Get default data directory
 */
std::string GetDefaultDataDir();

/**
 * Get configuration file path
 */
std::string GetConfigFile(const std::string& conf_path = "");

/**
 * Check if running on testnet
 */
bool IsTestNet();

/**
 * Check if running on regtest
 */
bool IsRegTest();

#endif // GOTHAM_COMMON_SYSTEM_H