// Copyright (c) 2023 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_NODE_KERNEL_NOTIFICATIONS_H
#define GOTHAM_NODE_KERNEL_NOTIFICATIONS_H

#include <kernel/notifications_interface.h>

#include <sync.h>
#include <threadsafety.h>
#include <uint256.h>

#include <atomic>
#include <cstdint>
#include <functional>

class ArgsManager;
class CBlockIndex;
enum class SynchronizationState;
struct bilingual_str;

namespace kernel {
enum class Warning;
} // namespace kernel

namespace node {

class Warnings;
static constexpr int DEFAULT_STOPATHEIGHT{0};

class KernelNotifications : public kernel::Notifications
{
public:
    KernelNotifications(const std::function<bool()>& shutdown_request, std::atomic<int>& exit_status, node::Warnings& warnings)
        : m_shutdown_request(shutdown_request), m_exit_status{exit_status}, m_warnings{warnings} {}

    [[nodiscard]] kernel::InterruptResult blockTip(SynchronizationState state, CBlockIndex& index, double verification_progress) override EXCLUSIVE_LOCKS_REQUIRED(!m_tip_block_mutex);

    void headerTip(SynchronizationState state, int64_t height, int64_t timestamp, bool presync) override;

    void progress(const bilingual_str& title, int progress_percent, bool resume_possible) override;

    void warningSet(kernel::Warning id, const bilingual_str& message) override;

    void warningUnset(kernel::Warning id) override;

    void flushError(const bilingual_str& message) override;

    void fatalError(const bilingual_str& message) override;

    //! Block height after which blockTip notification will return Interrupted{}, if >0.
    int m_stop_at_height{DEFAULT_STOPATHEIGHT};
    //! Useful for tests, can be set to false to avoid shutdown on fatal error.
    bool m_shutdown_on_fatal_error{true};

    Mutex m_tip_block_mutex;
    std::condition_variable m_tip_block_cv GUARDED_BY(m_tip_block_mutex);
    //! The block for which the last blockTip notification was received.
    //! It's first set when the tip is connected during node initialization.
    //! Might be unset during an early shutdown.
    std::optional<uint256> TipBlock() EXCLUSIVE_LOCKS_REQUIRED(m_tip_block_mutex);

private:
    const std::function<bool()>& m_shutdown_request;
    std::atomic<int>& m_exit_status;
    node::Warnings& m_warnings;

    std::optional<uint256> m_tip_block GUARDED_BY(m_tip_block_mutex);
};

void ReadNotificationArgs(const ArgsManager& args, KernelNotifications& notifications);

} // namespace node

#endif // GOTHAM_NODE_KERNEL_NOTIFICATIONS_H
