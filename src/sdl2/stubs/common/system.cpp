// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "system.h"
#include <cstdlib>

std::string GetDefaultDataDir()
{
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.gotham";
    }
    return "./gotham_data";
}

std::string GetConfigFile(const std::string& conf_path)
{
    if (!conf_path.empty()) {
        return conf_path;
    }
    return GetDefaultDataDir() + "/gotham.conf";
}

bool IsTestNet()
{
    // TODO: Implement actual testnet detection
    return false;
}

bool IsRegTest()
{
    // TODO: Implement actual regtest detection
    return false;
}