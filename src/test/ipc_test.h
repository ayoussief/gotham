// Copyright (c) 2023 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_TEST_IPC_TEST_H
#define GOTHAM_TEST_IPC_TEST_H

#include <primitives/transaction.h>
#include <script/script.h>
#include <univalue.h>
#include <util/fs.h>
#include <validation.h>

class FooImplementation
{
public:
    int add(int a, int b) { return a + b; }
    COutPoint passOutPoint(COutPoint o) { return o; }
    UniValue passUniValue(UniValue v) { return v; }
    CTransactionRef passTransaction(CTransactionRef t) { return t; }
    std::vector<char> passVectorChar(std::vector<char> v) { return v; }
    BlockValidationState passBlockState(BlockValidationState s) { return s; }
    CScript passScript(CScript s) { return s; }
};

void IpcPipeTest();
void IpcSocketPairTest();
void IpcSocketTest(const fs::path& datadir);

#endif // GOTHAM_TEST_IPC_TEST_H
