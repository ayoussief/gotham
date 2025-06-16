// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/middleman/middleman.h>
#include <mmp/taproot/taproot.h>
#include <key.h>
#include <pubkey.h>
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
    
    std::cout << "Middleman System Test" << std::endl;
    std::cout << "====================" << std::endl << std::endl;
    
    // Generate a key for a middleman
    std::pair<CKey, std::string> key_pair = mmp::GenerateRandomTaprootAddress(params);
    CKey key = key_pair.first;
    CPubKey pubkey = key.GetPubKey();
    
    // Create a middleman
    mmp::Middleman middleman("Dr. Arkham", pubkey);
    
    // Print middleman details
    std::cout << "Middleman ID: " << middleman.id.GetHex() << std::endl;
    std::cout << "Name: " << middleman.name << std::endl;
    std::cout << "Public Key: " << HexStr(middleman.pubkey) << std::endl;
    std::cout << "Trust Score: " << middleman.trust_score << std::endl;
    std::cout << "Taproot Address: " << middleman.GetAddress(params) << std::endl;
    std::cout << "Is Active: " << (middleman.is_active ? "Yes" : "No") << std::endl;
    std::cout << "Accepts New Jobs: " << (middleman.accepts_new_jobs ? "Yes" : "No") << std::endl;
    std::cout << "Effective Reputation: " << middleman.GetEffectiveReputation() << std::endl;
    std::cout << std::endl;
    
    // Get the default middleman
    mmp::Middleman default_middleman = mmp::GetDefaultMiddleman();
    
    // Print default middleman details
    std::cout << "Default Middleman ID: " << default_middleman.id.GetHex() << std::endl;
    std::cout << "Name: " << default_middleman.name << std::endl;
    std::cout << "Public Key: " << HexStr(default_middleman.pubkey) << std::endl;
    std::cout << "Trust Score: " << default_middleman.trust_score << std::endl;
    std::cout << "Taproot Address: " << default_middleman.GetAddress(params) << std::endl;
    std::cout << "Is Active: " << (default_middleman.is_active ? "Yes" : "No") << std::endl;
    std::cout << "Accepts New Jobs: " << (default_middleman.accepts_new_jobs ? "Yes" : "No") << std::endl;
    std::cout << "Effective Reputation: " << default_middleman.GetEffectiveReputation() << std::endl;
    
    return 0;
}