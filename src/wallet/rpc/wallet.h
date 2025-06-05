// Copyright (c) 2016-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_WALLET_RPC_WALLET_H
#define GOTHAM_WALLET_RPC_WALLET_H

#include <span.h>

class CRPCCommand;

namespace wallet {
std::span<const CRPCCommand> GetWalletRPCCommands();
} // namespace wallet

#endif // GOTHAM_WALLET_RPC_WALLET_H
