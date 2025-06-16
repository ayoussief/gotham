// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_TAPROOT_TAPROOT_H
#define GOTHAM_MMP_TAPROOT_TAPROOT_H

#include <key.h>
#include <pubkey.h>
#include <script/script.h>
#include <addresstype.h>
#include <key_io.h>
#include <chainparams.h>

#include <string>
#include <vector>

namespace mmp {

/**
 * Create a Taproot address from a given internal key and optional script tree.
 * 
 * @param internal_key The internal key to use for the Taproot output
 * @param merkle_root Optional merkle root of the script tree (nullptr for key-only Taproot)
 * @param params The chain parameters to use for address encoding
 * @return The Taproot address as a string
 */
std::string CreateTaprootAddressFromKey(const XOnlyPubKey& internal_key, const uint256* merkle_root, const CChainParams& params);

/**
 * Create a Taproot address from a given CScript.
 * 
 * @param cscript The script to use for the Taproot output
 * @param internal_key The internal key to use for the Taproot output
 * @param params The chain parameters to use for address encoding
 * @return The Taproot address as a string
 */
std::string CreateTaprootAddressFromScript(const CScript& cscript, const XOnlyPubKey& internal_key, const CChainParams& params);

/**
 * Generate a new random key and create a Taproot address from it.
 * 
 * @param params The chain parameters to use for address encoding
 * @return A pair containing the private key and the Taproot address
 */
std::pair<CKey, std::string> GenerateRandomTaprootAddress(const CChainParams& params);

} // namespace mmp

#endif // GOTHAM_MMP_TAPROOT_TAPROOT_H