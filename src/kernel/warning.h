// Copyright (c) 2024-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_KERNEL_WARNING_H
#define GOTHAM_KERNEL_WARNING_H

namespace kernel {
enum class Warning {
    UNKNOWN_NEW_RULES_ACTIVATED,
    LARGE_WORK_INVALID_CHAIN,
};
} // namespace kernel
#endif // GOTHAM_KERNEL_WARNING_H
