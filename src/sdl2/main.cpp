// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "gotham_city_app.h"
#include <common/args.h>
#include <common/init.h>
#include <common/system.h>
#include <util/translation.h>
#include <iostream>
#include <memory>

/** Translate string to current locale using basic implementation */
extern const TranslateFn G_TRANSLATION_FUN = [](const char* psz) {
    return std::string(psz);
};

const std::function<std::string()> G_TEST_GET_FULL_NAME{};

int main(int argc, char* argv[])
{
    try {
        // Initialize argument parsing
        ArgsManager args;
        SetupHelpOptions(args);
        
        // Parse command line arguments
        std::string error;
        if (!args.ParseParameters(argc, argv, error)) {
            std::cerr << "Error parsing arguments: " << error << std::endl;
            return EXIT_FAILURE;
        }

        // Show help if requested
        if (args.IsArgSet("-?") || args.IsArgSet("-h") || args.IsArgSet("-help")) {
            std::cout << "Gotham City - SDL2 GUI for Gotham Core" << std::endl;
            std::cout << "Usage: gotham-city [options]" << std::endl;
            std::cout << args.GetHelpMessage() << std::endl;
            return EXIT_SUCCESS;
        }

        // Create and run the application
        auto app = std::make_unique<GothamCityApp>();
        
        if (!app->Initialize(argc, argv)) {
            std::cerr << "Failed to initialize Gotham City application" << std::endl;
            return EXIT_FAILURE;
        }

        int result = app->Run();
        
        app->Shutdown();
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Gotham City error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}