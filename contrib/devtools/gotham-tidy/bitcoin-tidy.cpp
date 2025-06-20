// Copyright (c) 2023 Gotham Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "nontrivial-threadlocal.h"

#include <clang-tidy/ClangTidyModule.h>
#include <clang-tidy/ClangTidyModuleRegistry.h>

class GothamModule final : public clang::tidy::ClangTidyModule
{
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<gotham::NonTrivialThreadLocal>("gotham-nontrivial-threadlocal");
    }
};

static clang::tidy::ClangTidyModuleRegistry::Add<GothamModule>
    X("gotham-module", "Adds gotham checks.");

volatile int GothamModuleAnchorSource = 0;
