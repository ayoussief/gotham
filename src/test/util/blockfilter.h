// Copyright (c) 2019 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_TEST_UTIL_BLOCKFILTER_H
#define GOTHAM_TEST_UTIL_BLOCKFILTER_H

#include <blockfilter.h>

class CBlockIndex;
namespace node {
class BlockManager;
}

bool ComputeFilter(BlockFilterType filter_type, const CBlockIndex& block_index, BlockFilter& filter, const node::BlockManager& blockman);

#endif // GOTHAM_TEST_UTIL_BLOCKFILTER_H
