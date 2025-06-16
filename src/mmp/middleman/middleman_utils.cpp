// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/middleman/middleman_utils.h>
#include <hash.h>
#include <util/time.h>

namespace mmp {

bool ValidatePubKey(const CPubKey& pubkey) {
    // Check if the public key is valid
    if (!pubkey.IsValid()) {
        return false;
    }
    
    // Additional security checks could be added here
    
    return true;
}

uint256 ComputeMerkleRoot(const uint256& leaf, const std::vector<uint256>& proof) {
    uint256 current = leaf;
    
    for (const uint256& sibling : proof) {
        // Sort the hashes to ensure consistent ordering
        if (current < sibling) {
            // Hash current + sibling
            CHash256 hasher;
            hasher.Write(std::span<const unsigned char>(current.begin(), current.size()));
            hasher.Write(std::span<const unsigned char>(sibling.begin(), sibling.size()));
            hasher.Finalize(std::span<unsigned char>(current.begin(), current.size()));
        } else {
            // Hash sibling + current
            CHash256 hasher;
            hasher.Write(std::span<const unsigned char>(sibling.begin(), sibling.size()));
            hasher.Write(std::span<const unsigned char>(current.begin(), current.size()));
            hasher.Finalize(std::span<unsigned char>(current.begin(), current.size()));
        }
    }
    
    return current;
}

int64_t GetCurrentTimestamp() {
    return GetTime();
}

} // namespace mmp