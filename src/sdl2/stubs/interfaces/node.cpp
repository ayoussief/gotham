// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "node.h"
#include <iostream>

namespace interfaces {

class NodeImpl : public Node
{
public:
    NodeImpl(NodeContext& context) : m_context(context) {}

    NodeContext* context() override { return &m_context; }

    bool start() override {
        std::cout << "🦇 Starting Gotham node (stub)..." << std::endl;
        m_running = true;
        return true;
    }

    void stop() override {
        std::cout << "🦇 Stopping Gotham node (stub)..." << std::endl;
        m_running = false;
    }

    bool isRunning() override { return m_running; }

    std::string getNetwork() override { return "main"; }

    int getConnectionCount() override { return 8; }

    int getBlockCount() override { return 1234567; }

    bool isInitialBlockDownload() override { return false; }

    double getSyncProgress() override { return 1.0; }

private:
    NodeContext& m_context;
    bool m_running = false;
};

std::unique_ptr<Node> MakeNode(NodeContext& context)
{
    return std::make_unique<NodeImpl>(context);
}

} // namespace interfaces