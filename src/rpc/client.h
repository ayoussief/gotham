// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_RPC_CLIENT_H
#define GOTHAM_RPC_CLIENT_H

#include <string>
#include <string_view>

#include <univalue.h>

/** Convert positional arguments to command-specific RPC representation */
UniValue RPCConvertValues(const std::string& strMethod, const std::vector<std::string>& strParams);

/** Convert named arguments to command-specific RPC representation */
UniValue RPCConvertNamedValues(const std::string& strMethod, const std::vector<std::string>& strParams);

#endif // GOTHAM_RPC_CLIENT_H
