// Copyright (c) 2021 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_IPC_CAPNP_CONTEXT_H
#define GOTHAM_IPC_CAPNP_CONTEXT_H

#include <ipc/context.h>

namespace ipc {
namespace capnp {
//! Cap'n Proto context struct. Generally the parent ipc::Context struct should
//! be used instead of this struct to give all IPC protocols access to
//! application state, so there aren't unnecessary differences between IPC
//! protocols. But this specialized struct can be used to pass capnp-specific
//! function and object types to capnp hooks.
struct Context : ipc::Context
{
};
} // namespace capnp
} // namespace ipc

#endif // GOTHAM_IPC_CAPNP_CONTEXT_H
