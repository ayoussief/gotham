// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "args.h"
#include <iostream>
#include <sstream>

bool ArgsManager::ParseParameters(int argc, char* argv[], std::string& error)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.find("--") == 0) {
            // Long argument
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = arg.substr(2, eq_pos - 2);
                std::string value = arg.substr(eq_pos + 1);
                m_args[key] = value;
            } else {
                std::string key = arg.substr(2);
                m_args[key] = "1";
            }
        } else if (arg.find("-") == 0) {
            // Short argument
            std::string key = arg.substr(1);
            m_args[key] = "1";
        }
    }
    
    return true;
}

bool ArgsManager::IsArgSet(const std::string& arg) const
{
    std::string key = arg;
    if (key.find("-") == 0) {
        key = key.substr(key.find_first_not_of("-"));
    }
    return m_args.find(key) != m_args.end();
}

std::string ArgsManager::GetArg(const std::string& arg, const std::string& default_value) const
{
    std::string key = arg;
    if (key.find("-") == 0) {
        key = key.substr(key.find_first_not_of("-"));
    }
    
    auto it = m_args.find(key);
    return (it != m_args.end()) ? it->second : default_value;
}

bool ArgsManager::GetBoolArg(const std::string& arg, bool default_value) const
{
    std::string value = GetArg(arg, default_value ? "1" : "0");
    return value == "1" || value == "true" || value == "yes";
}

int64_t ArgsManager::GetIntArg(const std::string& arg, int64_t default_value) const
{
    std::string value = GetArg(arg, std::to_string(default_value));
    try {
        return std::stoll(value);
    } catch (...) {
        return default_value;
    }
}

std::string ArgsManager::GetHelpMessage() const
{
    std::ostringstream oss;
    oss << "Options:\n";
    oss << "  -h, --help          Show this help message\n";
    oss << "  --datadir=<dir>     Specify data directory\n";
    oss << "  --testnet           Use testnet\n";
    oss << "  --regtest           Use regression test network\n";
    oss << "  --fullscreen        Start in fullscreen mode\n";
    oss << "  --width=<pixels>    Window width (default: 1200)\n";
    oss << "  --height=<pixels>   Window height (default: 800)\n";
    return oss.str();
}

void ArgsManager::AddArg(const std::string& name, const std::string& help, unsigned int flags, const std::string& default_value)
{
    m_help[name] = help;
}

void SetupHelpOptions(ArgsManager& args)
{
    args.AddArg("-h", "Show help message", ALLOW_BOOL);
    args.AddArg("-help", "Show help message", ALLOW_BOOL);
    args.AddArg("-?", "Show help message", ALLOW_BOOL);
    args.AddArg("datadir", "Specify data directory", ALLOW_STRING);
    args.AddArg("testnet", "Use testnet", ALLOW_BOOL);
    args.AddArg("regtest", "Use regression test network", ALLOW_BOOL);
    args.AddArg("fullscreen", "Start in fullscreen mode", ALLOW_BOOL);
    args.AddArg("width", "Window width", ALLOW_INT, "1200");
    args.AddArg("height", "Window height", ALLOW_INT, "800");
}