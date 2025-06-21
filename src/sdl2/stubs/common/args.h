// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_COMMON_ARGS_H
#define GOTHAM_COMMON_ARGS_H

#include <string>
#include <vector>
#include <map>

/**
 * Argument manager for command line parsing
 * This is a stub implementation for the SDL2 GUI build
 */
class ArgsManager
{
public:
    ArgsManager() = default;
    ~ArgsManager() = default;

    /**
     * Parse command line arguments
     */
    bool ParseParameters(int argc, char* argv[], std::string& error);

    /**
     * Check if argument is set
     */
    bool IsArgSet(const std::string& arg) const;

    /**
     * Get string argument value
     */
    std::string GetArg(const std::string& arg, const std::string& default_value) const;

    /**
     * Get boolean argument value
     */
    bool GetBoolArg(const std::string& arg, bool default_value) const;

    /**
     * Get integer argument value
     */
    int64_t GetIntArg(const std::string& arg, int64_t default_value) const;

    /**
     * Get help message
     */
    std::string GetHelpMessage() const;

    /**
     * Add argument definition
     */
    void AddArg(const std::string& name, const std::string& help, unsigned int flags, const std::string& default_value = "");

private:
    std::map<std::string, std::string> m_args;
    std::map<std::string, std::string> m_help;
};

/**
 * Setup help options
 */
void SetupHelpOptions(ArgsManager& args);

// Argument flags
const unsigned int ALLOW_ANY = 0x01;
const unsigned int ALLOW_BOOL = 0x02;
const unsigned int ALLOW_INT = 0x04;
const unsigned int ALLOW_STRING = 0x08;

#endif // GOTHAM_COMMON_ARGS_H