// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/taproot/taproot.h>
#include <key.h>
#include <pubkey.h>
#include <script/script.h>
#include <chainparams.h>
#include <util/strencodings.h>
#include <util/chaintype.h>
#include <util/translation.h>
#include <iostream>

// Define the translation function
#ifdef DEFINE_G_TRANSLATION_FUN_AS_NULLPTR
const TranslateFn G_TRANSLATION_FUN{nullptr};
#endif

// Initialize ECC context
static ECC_Context ecc_context;

int main(int argc, char* argv[])
{
    SelectParams(ChainType::MAIN);
    
    // Get the chain parameters for mainnet
    const CChainParams& params = Params();
    
    std::cout << "Taproot Address Generation Examples:" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    
    // Example 1: Generate a random key-only Taproot address
    std::pair<CKey, std::string> random_address = mmp::GenerateRandomTaprootAddress(params);
    std::cout << "1. Random Taproot Address (key-only):" << std::endl;
    std::cout << "   Private key: " << HexStr(random_address.first.GetPrivKey()) << std::endl;
    std::cout << "   Address: " << random_address.second << std::endl << std::endl;
    
    // Example 2: Create a Taproot address with a specific key
    CKey key;
    key.MakeNewKey(true);
    CPubKey pubkey = key.GetPubKey();
    XOnlyPubKey xonly_pubkey(pubkey);
    
    uint256 empty_merkle_root;
    std::string key_address = mmp::CreateTaprootAddressFromKey(xonly_pubkey, &empty_merkle_root, params);
    
    std::cout << "2. Taproot Address from specific key:" << std::endl;
    std::cout << "   XOnly pubkey: " << HexStr(xonly_pubkey) << std::endl;
    std::cout << "   Address: " << key_address << std::endl << std::endl;
    
    // Example 3: Create a Taproot address with a script
    CScript script;
    script << OP_1 << ToByteVector(pubkey) << OP_1 << OP_CHECKSIG;
    
    std::string script_address = mmp::CreateTaprootAddressFromScript(script, xonly_pubkey, params);
    
    std::cout << "3. Taproot Address with script:" << std::endl;
    std::cout << "   Script: " << HexStr(script) << std::endl;
    std::cout << "   Address: " << script_address << std::endl;
    
    return 0;
}