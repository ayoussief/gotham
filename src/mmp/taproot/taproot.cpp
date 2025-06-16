// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/taproot/taproot.h>

#include <key.h>
#include <pubkey.h>
#include <script/script.h>
#include <addresstype.h>
#include <key_io.h>
#include <chainparams.h>
#include <util/strencodings.h>
#include <hash.h>

namespace mmp {

std::string CreateTaprootAddressFromKey(const XOnlyPubKey& internal_key, const uint256* merkle_root, const CChainParams& params)
{
    if (!internal_key.IsFullyValid()) {
        return "";
    }

    // Create the Taproot output key by tweaking the internal key with the merkle root
    std::optional<std::pair<XOnlyPubKey, bool>> taproot_key = internal_key.CreateTapTweak(merkle_root);
    if (!taproot_key) {
        return "";
    }

    // Create a WitnessV1Taproot destination from the tweaked key
    WitnessV1Taproot taproot(taproot_key->first);
    
    // Encode the destination as a Bech32m address
    return EncodeDestination(taproot);
}

std::string CreateTaprootAddressFromScript(const CScript& cscript, const XOnlyPubKey& internal_key, const CChainParams& params)
{
    if (!internal_key.IsFullyValid()) {
        return "";
    }

    // Hash the script to create a leaf
    HashWriter hasher;
    hasher << uint8_t(0xc0) /* LEAF_VERSION_TAPSCRIPT */ << cscript;
    uint256 script_hash = hasher.GetHash();
    
    // Use the script hash as the merkle root (single script tree)
    return CreateTaprootAddressFromKey(internal_key, &script_hash, params);
}

std::pair<CKey, std::string> GenerateRandomTaprootAddress(const CChainParams& params)
{
    // Generate a new random key
    CKey key = GenerateRandomKey();
    
    // Get the XOnlyPubKey from the full pubkey
    CPubKey pubkey = key.GetPubKey();
    XOnlyPubKey xonly_pubkey(pubkey);
    
    // Create a key-only Taproot address (no script tree)
    uint256 empty_merkle_root;
    std::string address = CreateTaprootAddressFromKey(xonly_pubkey, &empty_merkle_root, params);
    
    return std::make_pair(key, address);
}

} // namespace mmp