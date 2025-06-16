// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_UTILS_H
#define GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_UTILS_H

#include <pubkey.h>
#include <uint256.h>
#include <vector>
#include <cstdint>

namespace mmp {

/**
 * Validate a public key for security
 */
bool ValidatePubKey(const CPubKey& pubkey);

/**
 * Compute a merkle root from a leaf and a proof
 */
uint256 ComputeMerkleRoot(const uint256& leaf, const std::vector<uint256>& proof);

/**
 * Get the current timestamp
 */
int64_t GetCurrentTimestamp();

} // namespace mmp

#endif // GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_UTILS_H