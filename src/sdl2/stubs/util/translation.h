// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_UTIL_TRANSLATION_H
#define GOTHAM_UTIL_TRANSLATION_H

#include <string>
#include <functional>

/**
 * Translation utilities for Gotham Core
 * This is a stub implementation for the SDL2 GUI build
 */

using TranslateFn = std::function<std::string(const char*)>;

/**
 * Translation function - for now just returns the input string
 */
extern const TranslateFn G_TRANSLATION_FUN;

/**
 * Translate a string
 */
inline std::string _(const char* psz) {
    return G_TRANSLATION_FUN ? G_TRANSLATION_FUN(psz) : std::string(psz);
}

/**
 * Translate a string (overload for std::string)
 */
inline std::string _(const std::string& str) {
    return _(str.c_str());
}

#endif // GOTHAM_UTIL_TRANSLATION_H