// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <gotham-build-config.h> // IWYU pragma: keep

#include <core_io.h>
#include <key_io.h>
#include <rpc/server.h>
#include <rpc/util.h>
#include <univalue.h>
#include <util/translation.h>
#include <wallet/context.h>
#include <wallet/receive.h>
#include <wallet/rpc/util.h>
#include <wallet/wallet.h>
#include <wallet/walletutil.h>
#include <mmp/job.h>
#include <uint256.h>
#include <util/strencodings.h>

namespace wallet {

// RPC command: createjobpost (wallet function)
RPCHelpMan createjobpost() {
    return RPCHelpMan{
        "createjobpost",
        "Create a new job posting on the MMP network\n",
        {
            {"title", RPCArg::Type::STR, RPCArg::Optional::NO, "Job title (max 100 characters)"},
            {"description", RPCArg::Type::STR, RPCArg::Optional::NO, "Job description (max 1000 characters)"},
            {"amount", RPCArg::Type::AMOUNT, RPCArg::Optional::NO, "Job payment amount in BTC"},
            {"timeout_blocks", RPCArg::Type::STR, RPCArg::Optional::NO, "Job timeout in blocks (144-52560)"},
            {"requirements", RPCArg::Type::STR, RPCArg::Default{""}, "Job requirements"},
            {"deliverables", RPCArg::Type::STR, RPCArg::Default{""}, "Expected deliverables"},
        },
        RPCResult{
            RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::STR_HEX, "job_id", "The unique job identifier"},
                {RPCResult::Type::STR_HEX, "txid", "The transaction ID of the job posting"},
                {RPCResult::Type::STR, "status", "Status of the job creation"},
            }
        },
        RPCExamples{
            HelpExampleCli("createjobpost", "\"Web Development\" \"Build a simple website\" 0.1 1008 \"HTML/CSS/JS skills\" \"Responsive website\"")
            + HelpExampleRpc("createjobpost", "\"Web Development\", \"Build a simple website\", 0.1, 1008, \"HTML/CSS/JS skills\", \"Responsive website\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue {
            // Get wallet context
            const std::shared_ptr<CWallet> pwallet = GetWalletForJSONRPCRequest(request);
            if (!pwallet) return UniValue::VNULL;

            // Extract parameters
            std::string title = request.params[0].get_str();
            std::string description = request.params[1].get_str();
            CAmount amount = AmountFromValue(request.params[2]);
            
            uint32_t timeout_blocks;
            try {
                timeout_blocks = std::stoul(request.params[3].get_str());
            } catch (const std::exception& e) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, "timeout_blocks must be a number between 144-52560");
            }
            
            // Validate range
            if (timeout_blocks < 144 || timeout_blocks > 52560) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, "timeout_blocks must be between 144-52560");
            }
            
            std::string requirements = request.params.size() > 4 ? request.params[4].get_str() : "";
            std::string deliverables = request.params.size() > 5 ? request.params[5].get_str() : "";
            
            // Validate parameters
            std::string error;
            if (!mmp::JobRPC::ValidateJobPost(title, description, amount, timeout_blocks, error)) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, error);
            }
            
            // Create job posting transaction using wallet
            return mmp::JobRPC::CreateJobPostTransaction(title, description, amount, timeout_blocks, requirements, deliverables, request);
        },
    };
}

// RPC command: applytojob (wallet function)
RPCHelpMan applytojob() {
    return RPCHelpMan{
        "applytojob",
        "Apply to an existing job posting on the MMP network\n",
        {
            {"job_id", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The job ID to apply to"},
            {"proposal", RPCArg::Type::STR, RPCArg::Optional::NO, "Your proposal/application (max 1000 characters)"},
            {"estimated_completion", RPCArg::Type::STR, RPCArg::Optional::NO, "Estimated completion time in blocks"},
        },
        RPCResult{
            RPCResult::Type::OBJ, "", "",
            {
                {RPCResult::Type::STR_HEX, "application_id", "The unique application identifier"},
                {RPCResult::Type::STR_HEX, "txid", "The transaction ID of the job application"},
                {RPCResult::Type::STR, "status", "Status of the job application"},
            }
        },
        RPCExamples{
            HelpExampleCli("applytojob", "\"1234567890abcdef\" \"I can build this website with modern responsive design\" 720")
            + HelpExampleRpc("applytojob", "\"1234567890abcdef\", \"I can build this website with modern responsive design\", 720")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue {
            // Get wallet context
            const std::shared_ptr<CWallet> pwallet = GetWalletForJSONRPCRequest(request);
            if (!pwallet) return UniValue::VNULL;

            // Extract parameters
            uint256 job_id = ParseHashV(request.params[0], "job_id");
            std::string proposal = request.params[1].get_str();
            
            uint32_t estimated_completion;
            try {
                estimated_completion = std::stoul(request.params[2].get_str());
            } catch (const std::exception& e) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, "estimated_completion must be a number");
            }
            
            // Validate parameters
            std::string error;
            if (!mmp::JobRPC::ValidateJobApplication(job_id, proposal, error)) {
                throw JSONRPCError(RPC_INVALID_PARAMETER, error);
            }
            
            // Create job application transaction using wallet
            return mmp::JobRPC::CreateJobApplicationTransaction(job_id, proposal, request);
        },
    };
}

} // namespace wallet