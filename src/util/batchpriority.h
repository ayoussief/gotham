// Copyright (c) 2023 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_UTIL_BATCHPRIORITY_H
#define GOTHAM_UTIL_BATCHPRIORITY_H

/**
 * On platforms that support it, tell the kernel the calling thread is
 * CPU-intensive and non-interactive. See SCHED_BATCH in sched(7) for details.
 *
 */
void ScheduleBatchPriority();

#endif // GOTHAM_UTIL_BATCHPRIORITY_H
