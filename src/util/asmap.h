// Copyright (c) 2019-2021 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_UTIL_ASMAP_H
#define GOTHAM_UTIL_ASMAP_H

#include <util/fs.h>

#include <cstdint>
#include <vector>

uint32_t Interpret(const std::vector<bool> &asmap, const std::vector<bool> &ip);

bool SanityCheckASMap(const std::vector<bool>& asmap, int bits);

/** Read asmap from provided binary file */
std::vector<bool> DecodeAsmap(fs::path path);

#endif // GOTHAM_UTIL_ASMAP_H
