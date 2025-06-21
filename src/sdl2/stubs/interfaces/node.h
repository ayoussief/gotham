// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_INTERFACES_NODE_H
#define GOTHAM_INTERFACES_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

/**
 * Node interface for Gotham Core
 * This is a stub implementation for the SDL2 GUI build
 */

namespace interfaces {

struct NodeContext {
    // Stub implementation
};

struct WalletContext {
    // Stub implementation
};

class Node
{
public:
    virtual ~Node() = default;

    /**
     * Get node context
     */
    virtual NodeContext* context() = 0;

    /**
     * Start node
     */
    virtual bool start() = 0;

    /**
     * Stop node
     */
    virtual void stop() = 0;

    /**
     * Check if node is running
     */
    virtual bool isRunning() = 0;

    /**
     * Get network name
     */
    virtual std::string getNetwork() = 0;

    /**
     * Get connection count
     */
    virtual int getConnectionCount() = 0;

    /**
     * Get block count
     */
    virtual int getBlockCount() = 0;

    /**
     * Check if initial block download is in progress
     */
    virtual bool isInitialBlockDownload() = 0;

    /**
     * Get synchronization progress
     */
    virtual double getSyncProgress() = 0;
};

/**
 * Create a node instance
 */
std::unique_ptr<Node> MakeNode(NodeContext& context);

} // namespace interfaces

#endif // GOTHAM_INTERFACES_NODE_H