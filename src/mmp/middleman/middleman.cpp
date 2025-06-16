// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/middleman/middleman.h>
#include <mmp/taproot/taproot.h>
#include <key_io.h>
#include <hash.h>
#include <chainparams.h>

namespace mmp {

uint256 Middleman::GenerateMiddlemanId(const std::string& name, const CPubKey& pubkey) {
    HashWriter hasher;
    hasher << name << pubkey;
    return hasher.GetHash();
}

std::string Middleman::GetAddress(const CChainParams& params) const {
    XOnlyPubKey xonly_pubkey(pubkey);
    WitnessV1Taproot taproot(xonly_pubkey);
    return EncodeDestination(taproot);
}

Middleman GetDefaultMiddleman() {
    // Create a deterministic key for the default middleman
    // In a real implementation, this would be a well-known public key
    CKey key;
    key.MakeNewKey(true);
    CPubKey pubkey = key.GetPubKey();
    
    Middleman middleman("DefaultMiddleman", pubkey);
    
    // Set additional properties for the default middleman
    middleman.is_active = true;
    middleman.accepts_new_jobs = true;
    middleman.fee_sats = 10000; // 0.0001 BTC
    middleman.bond_amount_sats = 10000000; // 0.1 BTC
    middleman.description = "Default middleman for testing purposes";
    middleman.contact_info = "default@example.com";
    middleman.response_time_blocks = 144; // 24 hours
    
    return middleman;
}

} // namespace mmp