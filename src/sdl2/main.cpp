// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include <gotham-build-config.h> // IWYU pragma: keep

#include "gotham_city_app.h"
#include <chainparams.h>
#include <clientversion.h>
#include <common/args.h>
#include <common/init.h>
#include <common/system.h>
#include <compat/compat.h>
#include <init.h>
#include <interfaces/chain.h>
#include <interfaces/init.h>
#include <kernel/context.h>
#include <node/context.h>
#include <node/interface_ui.h>
#include <node/warnings.h>
#include <noui.h>
#include <util/check.h>
#include <util/exception.h>
#include <util/signalinterrupt.h>
#include <util/strencodings.h>
#include <util/syserror.h>
#include <util/threadnames.h>
#include <util/tokenpipe.h>
#include <util/translation.h>

#include <any>
#include <functional>
#include <optional>
#include <iostream>
#include <memory>

using node::NodeContext;

const TranslateFn G_TRANSLATION_FUN{nullptr};

static bool ParseArgs(interfaces::Init& init, int argc, char* argv[])
{
    SetupServerArgs(gArgs, init.canListenIpc());
    std::string error;
    if (!gArgs.ParseParameters(argc, argv, error)) {
        return InitError(Untranslated(strprintf("Error parsing command line arguments: %s", error)));
    }

    if (auto error = common::InitConfig(gArgs)) {
        return InitError(error->message, error->details);
    }

    // Error out when loose non-argument tokens are encountered on command line
    for (int i = 1; i < argc; i++) {
        if (!IsSwitchChar(argv[i][0])) {
            return InitError(Untranslated(strprintf("Command line contains unexpected token '%s', see gotham-city -h for a list of options.", argv[i])));
        }
    }
    return true;
}

static bool ProcessInitCommands()
{
    // Process help and version before taking care about datadir
    if (HelpRequested(gArgs) || gArgs.GetBoolArg("-version", false)) {
        std::string strUsage = "Gotham City - SDL2 GUI version " + FormatFullVersion() + "\n";

        if (gArgs.GetBoolArg("-version", false)) {
            strUsage += FormatParagraph(LicenseInfo());
        } else {
            strUsage += "\n"
                "Gotham City is a graphical user interface for Gotham Core.\n"
                "It provides a user-friendly way to interact with the Gotham network.\n"
                "\n"
                "Usage: gotham-city [options]\n"
                "\n";
            strUsage += gArgs.GetHelpMessage();
        }

        tfm::format(std::cout, "%s", strUsage);
        return true;
    }

    return false;
}

static bool AppInit(NodeContext& node)
{
    bool fRet = false;

    std::any context{&node};
    try
    {
        InitLogging(gArgs);
        InitParameterInteraction(gArgs);
        if (!AppInitBasicSetup(gArgs, node.exit_status)) {
            // InitError will have been called with detailed error, which ends up on console
            return false;
        }
        if (!AppInitParameterInteraction(gArgs)) {
            // InitError will have been called with detailed error, which ends up on console
            return false;
        }

        node.warnings = std::make_unique<node::Warnings>();
        node.kernel = std::make_unique<kernel::Context>();
        node.ecc_context = std::make_unique<ECC_Context>();
        
        if (!AppInitSanityChecks(*node.kernel))
        {
            // InitError will have been called with detailed error, which ends up on console
            return false;
        }

        // Lock critical directories after daemonization
        if (!AppInitLockDirectories())
        {
            // If locking a directory failed, exit immediately
            return false;
        }
        
        // Initialize interfaces but don't start the node yet
        fRet = AppInitInterfaces(node);
    }
    catch (const std::exception& e) {
        PrintExceptionContinue(&e, "AppInit()");
    } catch (...) {
        PrintExceptionContinue(nullptr, "AppInit()");
    }

    return fRet;
}

int main(int argc, char* argv[])
{
    try {
        NodeContext node;
        int exit_status;
        std::unique_ptr<interfaces::Init> init = interfaces::MakeGuiInit(argc, argv);
        if (!init) {
            return EXIT_FAILURE;
        }
        
        node.init = init.get();

        SetupEnvironment();

        // Connect GUI signal handlers (similar to noui_connect for daemon)
        noui_connect();

        util::ThreadSetInternalName("init");

        // Interpret command line arguments
        if (!ParseArgs(*init, argc, argv)) return EXIT_FAILURE;
        
        // Process early info return commands such as -help or -version
        if (ProcessInitCommands()) return EXIT_SUCCESS;

        // Initialize the core components
        if (!AppInit(node)) {
            node.exit_status = EXIT_FAILURE;
        } else {
            // Create and run the SDL2 application
            auto app = std::make_unique<GothamCityApp>();
            
            if (!app->Initialize(argc, argv)) {
                std::cerr << "Failed to initialize Gotham City application" << std::endl;
                node.exit_status = EXIT_FAILURE;
            } else {
                node.exit_status = app->Run();
                app->Shutdown();
            }
        }

        Interrupt(node);
        Shutdown(node);

        return node.exit_status;
        
    } catch (const std::exception& e) {
        PrintExceptionContinue(&e, "main()");
        return EXIT_FAILURE;
    } catch (...) {
        PrintExceptionContinue(nullptr, "main()");
        return EXIT_FAILURE;
    }
}