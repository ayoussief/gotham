// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/job.h>
#include <mmp/contract/job_contract.h>
#include <mmp/contract/job_contract_types.h>
#include <mmp/middleman/middleman.h>

#include <rpc/server.h>
#include <rpc/server_util.h>
#include <rpc/util.h>
#include <node/context.h>
#include <txmempool.h>
#include <chain.h>
#include <chainparams.h>
#include <primitives/transaction.h>
#include <primitives/block.h>
#include <validation.h>
#include <node/blockstorage.h>
#include <util/strencodings.h>
#include <util/time.h>
#include <util/moneystr.h>
#include <random.h>
#include <core_io.h>
#include <rpc/util.h>
#include <rpc/server.h>
#include <key_io.h>
#include <script/script.h>
#include <consensus/amount.h>
#include <wallet/rpc/util.h>
#include <wallet/spend.h>
#include <wallet/wallet.h>
#include <wallet/coincontrol.h>
#include <policy/fees.h>
#include <serialize.h>
#include <streams.h>

#include <algorithm>
#include <sstream>

using node::NodeContext;

namespace mmp {

// JobContractManager implementation
void JobContractManager::StoreContract(const std::shared_ptr<JobContract>& contract) {
    LOCK(cs_contracts);
    contracts[contract->job_id] = contract;
}

std::shared_ptr<JobContract> JobContractManager::GetContract(const uint256& job_id) const {
    LOCK(cs_contracts);
    auto it = contracts.find(job_id);
    return (it != contracts.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<JobContract>> JobContractManager::GetAllContracts() const {
    LOCK(cs_contracts);
    std::vector<std::shared_ptr<JobContract>> result;
    for (const auto& pair : contracts) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<JobContract>> JobContractManager::GetContractsByState(JobState state) const {
    LOCK(cs_contracts);
    std::vector<std::shared_ptr<JobContract>> result;
    for (const auto& pair : contracts) {
        if (pair.second->state == state) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool JobContractManager::UpdateContractState(const uint256& job_id, JobState new_state, const uint256& txid, const std::string& memo) {
    LOCK(cs_contracts);
    auto it = contracts.find(job_id);
    if (it != contracts.end()) {
        it->second->AddEvent(new_state, txid, memo);
        return true;
    }
    return false;
}

bool JobContractManager::AddApplicationToContract(const uint256& job_id, const WorkerApplication& application) {
    LOCK(cs_contracts);
    auto it = contracts.find(job_id);
    if (it != contracts.end()) {
        it->second->worker_applications.push_back(application);
        return true;
    }
    return false;
}

bool JobContractManager::RemoveContract(const uint256& job_id) {
    LOCK(cs_contracts);
    return contracts.erase(job_id) > 0;
}

JobContractManager& JobContractManager::GetInstance() {
    static JobContractManager instance;
    return instance;
}

// Convert JobSearchResult to JSON
UniValue JobSearchResult::ToJSON() const {
    UniValue result(UniValue::VOBJ);
    
    result.pushKV("job_id", job_id.GetHex());
    result.pushKV("txid", txid.GetHex());
    result.pushKV("state", JobStateToString(state));
    result.pushKV("timestamp", timestamp);
    result.pushKV("block_height", (int64_t)block_height);
    result.pushKV("in_mempool", in_mempool);
    
    // Add metadata
    UniValue metadata_obj(UniValue::VOBJ);
    metadata_obj.pushKV("title", metadata.title);
    metadata_obj.pushKV("description", metadata.description);
    metadata_obj.pushKV("amount", ValueFromAmount(metadata.amount));
    metadata_obj.pushKV("created_height", (int64_t)metadata.created_height);
    metadata_obj.pushKV("created_timestamp", metadata.created_timestamp);
    metadata_obj.pushKV("timeout_blocks", (int64_t)metadata.timeout_blocks);
    metadata_obj.pushKV("requirements", metadata.requirements);
    metadata_obj.pushKV("deliverables", metadata.deliverables);
    result.pushKV("metadata", metadata_obj);
    
    // Add applications if any
    if (!applications.empty()) {
        UniValue apps_array(UniValue::VARR);
        for (const auto& app : applications) {
            UniValue app_obj(UniValue::VOBJ);
            app_obj.pushKV("worker_pubkey", HexStr(app.worker_pubkey));
            app_obj.pushKV("proposal", app.proposal);
            app_obj.pushKV("timestamp", app.timestamp);
            app_obj.pushKV("status", static_cast<int>(app.status));
            apps_array.push_back(app_obj);
        }
        result.pushKV("applications", apps_array);
    }
    
    return result;
}

// Search mempool for job transactions
std::vector<JobSearchResult> JobRPC::SearchMempoolForJobs(const CTxMemPool& mempool) {
    std::vector<JobSearchResult> results;
    
    LOCK(mempool.cs);
    
    // Iterate through all transactions in mempool
    for (const auto& entry : mempool.mapTx) {
        const CTransactionRef& tx = entry.GetSharedTx();
        
        if (IsJobTransaction(tx)) {
            auto job_result = ParseJobTransaction(tx, true, 0);
            if (job_result) {
                results.push_back(*job_result);
            }
        }
    }
    
    return results;
}

// Search blockchain for job transactions
std::vector<JobSearchResult> JobRPC::SearchBlockchainForJobs(
    const ChainstateManager& chainman, 
    uint32_t start_height, 
    uint32_t end_height) {
    
    std::vector<JobSearchResult> results;
    
    LOCK(cs_main);
    
    const CChain& active_chain = chainman.ActiveChain();
    
    // Set default end height to current tip if not specified
    if (end_height == 0) {
        end_height = active_chain.Height();
    }
    
    // Validate height range
    if (start_height > end_height || end_height > (uint32_t)active_chain.Height()) {
        return results; // Return empty results for invalid range
    }
    
    // Search through blocks
    for (uint32_t height = start_height; height <= end_height; ++height) {
        const CBlockIndex* pindex = active_chain[height];
        if (!pindex) continue;
        
        CBlock block;
        if (!chainman.m_blockman.ReadBlock(block, *pindex)) {
            continue; // Skip if can't read block
        }
        
        // Check each transaction in the block
        for (const auto& tx : block.vtx) {
            if (IsJobTransaction(tx)) {
                auto job_result = ParseJobTransaction(tx, false, height);
                if (job_result) {
                    results.push_back(*job_result);
                }
            }
        }
    }
    
    return results;
}

// Check if transaction contains job data
bool JobRPC::IsJobTransaction(const CTransactionRef& tx) {
    // Look for OP_RETURN outputs that might contain job data
    for (const auto& output : tx->vout) {
        if (output.scriptPubKey.size() > 0 && output.scriptPubKey[0] == OP_RETURN) {
            // Check if the OP_RETURN data starts with MMP job marker
            std::vector<unsigned char> data;
            if (output.scriptPubKey.size() > 1) {
                data.assign(output.scriptPubKey.begin() + 1, output.scriptPubKey.end());
                
                // Look for MMP job marker (simple check for now)
                if (data.size() >= 4) {
                    std::string marker(data.begin(), data.begin() + 4);
                    if (marker == "MMPJ") { // MMP Job marker
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

// Parse job data from transaction
std::optional<JobSearchResult> JobRPC::ParseJobTransaction(
    const CTransactionRef& tx, 
    bool in_mempool, 
    uint32_t block_height) {
    
    // Find the OP_RETURN output with job data
    for (size_t i = 0; i < tx->vout.size(); ++i) {
        const auto& output = tx->vout[i];
        if (output.scriptPubKey.size() > 0 && output.scriptPubKey[0] == OP_RETURN) {
            std::vector<unsigned char> data;
            if (output.scriptPubKey.size() > 1) {
                data.assign(output.scriptPubKey.begin() + 1, output.scriptPubKey.end());
                
                if (data.size() >= 6) { // marker(4) + version(1) + type(1)
                    std::string marker(data.begin(), data.begin() + 4);
                    if (marker == "MMPJ") {
                        uint8_t version = data[4];
                        uint8_t job_type = data[5];
                        
                        if (version == 0x01 && job_type == 0x01) { // Job posting
                            try {
                                // Parse serialized job data
                                DataStream stream{std::span<const unsigned char>(data.begin() + 6, data.end())};
                                
                                JobSearchResult result;
                                result.txid = tx->GetHash();
                                result.in_mempool = in_mempool;
                                result.block_height = block_height;
                                result.timestamp = GetTime();
                                
                                // Deserialize job data
                                stream >> result.job_id;
                                stream >> result.metadata.title;
                                stream >> result.metadata.description;
                                stream >> result.metadata.amount;
                                stream >> result.metadata.timeout_blocks;
                                stream >> result.metadata.requirements;
                                stream >> result.metadata.deliverables;
                                
                                result.metadata.created_height = block_height;
                                result.metadata.created_timestamp = GetTime();
                                result.state = JobState::OPEN;
                                
                                // Check if there's an escrow output (should be the next output)
                                if (i + 1 < tx->vout.size()) {
                                    result.metadata.amount = tx->vout[i + 1].nValue;
                                }
                                
                                return result;
                            } catch (const std::exception&) {
                                // Failed to parse, continue searching
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return std::nullopt;
}

// Validate job post parameters
bool JobRPC::ValidateJobPost(
    const std::string& title,
    const std::string& description,
    CAmount amount,
    uint32_t timeout_blocks,
    std::string& error) {
    
    if (title.empty() || title.length() > 100) {
        error = "Title must be between 1 and 100 characters";
        return false;
    }
    
    if (description.empty() || description.length() > 1000) {
        error = "Description must be between 1 and 1000 characters";
        return false;
    }
    
    if (amount <= 0) {
        error = "Amount must be positive";
        return false;
    }
    
    if (timeout_blocks < 144 || timeout_blocks > 52560) { // 1 day to 1 year
        error = "Timeout must be between 144 blocks (1 day) and 52560 blocks (1 year)";
        return false;
    }
    
    return true;
}

// Validate job application parameters
bool JobRPC::ValidateJobApplication(
    const uint256& job_id,
    const std::string& proposal,
    std::string& error) {
    
    if (job_id.IsNull()) {
        error = "Invalid job ID";
        return false;
    }
    
    if (proposal.empty() || proposal.length() > 500) {
        error = "Proposal must be between 1 and 500 characters";
        return false;
    }
    
    return true;
}

// Create OP_RETURN data for job posting
std::vector<unsigned char> JobRPC::CreateJobPostingData(
    const uint256& job_id,
    const std::string& title,
    const std::string& description,
    CAmount amount,
    uint32_t timeout_blocks,
    const std::string& requirements,
    const std::string& deliverables) {
    
    std::vector<unsigned char> data;
    
    // Add MMP job marker
    std::string marker = "MMPJ";
    data.insert(data.end(), marker.begin(), marker.end());
    
    // Add version byte
    data.push_back(0x01);
    
    // Add job type (0x01 = job posting)
    data.push_back(0x01);
    
    // Serialize job data
    DataStream stream;
    stream << job_id << title << description << amount << timeout_blocks << requirements << deliverables;
    
    // Convert stream to vector
    std::vector<unsigned char> stream_data;
    stream_data.reserve(stream.size());
    for (const auto& byte : stream) {
        stream_data.push_back(static_cast<unsigned char>(byte));
    }
    data.insert(data.end(), stream_data.begin(), stream_data.end());
    
    return data;
}

// Create OP_RETURN data for job application
std::vector<unsigned char> JobRPC::CreateJobApplicationData(
    const uint256& job_id,
    const std::string& proposal,
    const CPubKey& worker_pubkey) {
    
    std::vector<unsigned char> data;
    
    // Add MMP job marker
    std::string marker = "MMPJ";
    data.insert(data.end(), marker.begin(), marker.end());
    
    // Add version byte
    data.push_back(0x01);
    
    // Add job type (0x02 = job application)
    data.push_back(0x02);
    
    // Serialize application data
    DataStream stream;
    stream << job_id << proposal << worker_pubkey;
    
    // Convert stream to vector
    std::vector<unsigned char> stream_data;
    stream_data.reserve(stream.size());
    for (const auto& byte : stream) {
        stream_data.push_back(static_cast<unsigned char>(byte));
    }
    data.insert(data.end(), stream_data.begin(), stream_data.end());
    
    return data;
}

// Create and broadcast job posting transaction
UniValue JobRPC::CreateJobPostTransaction(
    const std::string& title,
    const std::string& description,
    CAmount amount,
    uint32_t timeout_blocks,
    const std::string& requirements,
    const std::string& deliverables,
    const JSONRPCRequest& request) {
    
    // Get wallet
    std::shared_ptr<wallet::CWallet> pwallet = wallet::GetWalletForJSONRPCRequest(request);
    if (!pwallet) {
        throw JSONRPCError(RPC_WALLET_ERROR, "No wallet available");
    }
    
    wallet::EnsureWalletIsUnlocked(*pwallet);
    
    // Get current block height and timestamp
    uint32_t current_height = pwallet->GetLastBlockHeight();
    int64_t current_timestamp = GetTime();
    
    // Get employer's public key from a new destination
    CPubKey employer_pubkey;
    {
        LOCK(pwallet->cs_wallet);
        auto dest_result = pwallet->GetNewDestination(OutputType::LEGACY, "");
        if (!dest_result) {
            throw JSONRPCError(RPC_WALLET_KEYPOOL_RAN_OUT, util::ErrorString(dest_result).original);
        }
        
        // Extract public key from PKHash destination
        if (auto* pkhash = std::get_if<PKHash>(&*dest_result)) {
            CKeyID keyID = ToKeyID(*pkhash);
            CScript script = GetScriptForDestination(*dest_result);
            auto provider = pwallet->GetSolvingProvider(script);
            if (!provider || !provider->GetPubKey(keyID, employer_pubkey)) {
                throw JSONRPCError(RPC_WALLET_ERROR, "Unable to get public key from wallet");
            }
        } else {
            throw JSONRPCError(RPC_WALLET_ERROR, "Unable to get PKHash destination from wallet");
        }
    }
    
    // Generate job ID
    uint256 job_id = GenerateJobId(employer_pubkey, title, current_timestamp);
    
    // Create job contract
    auto contract = std::make_shared<JobContract>(
        job_id, title, description, current_height, current_timestamp, 
        timeout_blocks, amount, employer_pubkey
    );
    
    // Get default middleman for now
    Middleman default_middleman = GetDefaultMiddleman();
    contract->middleman_info.id = default_middleman.id;
    contract->middleman_info.name = default_middleman.name;
    contract->middleman_info.pubkey = default_middleman.pubkey;
    contract->middleman_info.selection_timestamp = current_timestamp;
    
    // Create OP_RETURN data
    std::vector<unsigned char> op_return_data = CreateJobPostingData(
        job_id, title, description, amount, timeout_blocks, requirements, deliverables
    );
    
    // Create transaction
    wallet::CCoinControl coin_control;
    std::vector<wallet::CRecipient> recipients;
    
    // Add OP_RETURN output
    wallet::CRecipient op_return_recipient;
    op_return_recipient.dest = CNoDestination{CScript() << OP_RETURN << op_return_data}; // Proper OP_RETURN destination
    op_return_recipient.nAmount = 0;
    op_return_recipient.fSubtractFeeFromAmount = false;
    recipients.push_back(op_return_recipient);
    
    // Create initial Taproot escrow with just employer's key
    // This will be upgraded to 2-of-2 when worker is selected, and 2-of-3 if dispute occurs
    CScript initial_escrow_script = CreateInitialJobEscrowScript(employer_pubkey);
    wallet::CRecipient escrow_recipient;
    
    // Use Taproot address for the escrow (single key initially)
    XOnlyPubKey employer_xonly(employer_pubkey);
    escrow_recipient.dest = WitnessV1Taproot(employer_xonly);
    escrow_recipient.nAmount = amount;
    escrow_recipient.fSubtractFeeFromAmount = false;
    recipients.push_back(escrow_recipient);
    
    // Create and sign transaction
    auto res = wallet::CreateTransaction(*pwallet, recipients, std::nullopt, coin_control, true);
    if (!res) {
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, util::ErrorString(res).original);
    }
    
    const CTransactionRef& tx = res->tx;
    
    // Update contract with funding transaction info
    contract->funding_txid = tx->GetHash();
    contract->funding_vout = 1; // Escrow output is second output
    contract->TransitionState(JobState::OPEN, tx->GetHash(), "Job posted and funded");
    
    // Store contract
    JobContractManager::GetInstance().StoreContract(contract);
    
    // Broadcast transaction
    pwallet->CommitTransaction(tx, {}, {});
    
    // Return result
    UniValue result(UniValue::VOBJ);
    result.pushKV("job_id", job_id.GetHex());
    result.pushKV("txid", tx->GetHash().GetHex());
    result.pushKV("status", "posted");
    result.pushKV("escrow_amount", ValueFromAmount(amount));
    result.pushKV("escrow_address", EncodeDestination(WitnessV1Taproot(employer_xonly)));
    result.pushKV("escrow_type", "taproot_single_key");
    result.pushKV("note", "Escrow will upgrade to 2-of-2 when worker selected, 2-of-3 if dispute occurs");
    
    return result;
}

// Create and broadcast job application transaction
UniValue JobRPC::CreateJobApplicationTransaction(
    const uint256& job_id,
    const std::string& proposal,
    const JSONRPCRequest& request) {
    
    // Get wallet
    std::shared_ptr<wallet::CWallet> pwallet = wallet::GetWalletForJSONRPCRequest(request);
    if (!pwallet) {
        throw JSONRPCError(RPC_WALLET_ERROR, "No wallet available");
    }
    
    wallet::EnsureWalletIsUnlocked(*pwallet);
    
    // Check if job exists
    auto contract = JobContractManager::GetInstance().GetContract(job_id);
    if (!contract) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Job not found");
    }
    
    if (contract->state != JobState::OPEN) {
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Job is not open for applications");
    }
    
    // Get worker's public key from a new destination
    CPubKey worker_pubkey;
    {
        LOCK(pwallet->cs_wallet);
        auto dest_result = pwallet->GetNewDestination(OutputType::LEGACY, "");
        if (!dest_result) {
            throw JSONRPCError(RPC_WALLET_KEYPOOL_RAN_OUT, util::ErrorString(dest_result).original);
        }
        
        // Extract public key from PKHash destination
        if (auto* pkhash = std::get_if<PKHash>(&*dest_result)) {
            CKeyID keyID = ToKeyID(*pkhash);
            CScript script = GetScriptForDestination(*dest_result);
            auto provider = pwallet->GetSolvingProvider(script);
            if (!provider || !provider->GetPubKey(keyID, worker_pubkey)) {
                throw JSONRPCError(RPC_WALLET_ERROR, "Unable to get public key from wallet");
            }
        } else {
            throw JSONRPCError(RPC_WALLET_ERROR, "Unable to get PKHash destination from wallet");
        }
    }
    
    // Create OP_RETURN data
    std::vector<unsigned char> op_return_data = CreateJobApplicationData(
        job_id, proposal, worker_pubkey
    );
    
    // Create transaction
    wallet::CCoinControl coin_control;
    std::vector<wallet::CRecipient> recipients;
    
    // Add OP_RETURN output
    wallet::CRecipient op_return_recipient;
    op_return_recipient.dest = CNoDestination{CScript() << OP_RETURN << op_return_data}; // Proper OP_RETURN destination
    op_return_recipient.nAmount = 0;
    op_return_recipient.fSubtractFeeFromAmount = false;
    recipients.push_back(op_return_recipient);
    
    // Create and sign transaction
    auto res = wallet::CreateTransaction(*pwallet, recipients, std::nullopt, coin_control, true);
    if (!res) {
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, util::ErrorString(res).original);
    }
    
    const CTransactionRef& tx = res->tx;
    
    // Create worker application
    WorkerApplication application;
    application.worker_pubkey = worker_pubkey;
    application.proposal = proposal;
    application.timestamp = GetTime();
    application.status = ApplicationStatus::PENDING;
    
    // Add application to contract
    JobContractManager::GetInstance().AddApplicationToContract(job_id, application);
    
    // Broadcast transaction
    pwallet->CommitTransaction(tx, {}, {});
    
    // Return result
    UniValue result(UniValue::VOBJ);
    result.pushKV("application_id", tx->GetHash().GetHex());
    result.pushKV("txid", tx->GetHash().GetHex());
    result.pushKV("status", "submitted");
    result.pushKV("job_id", job_id.GetHex());
    
    return result;
}


// RPC command: listopenjobs
RPCHelpMan listopenjobs() {
    return RPCHelpMan{
        "listopenjobs",
        "List open job postings from mempool and blockchain\n",
        {
            {"limit", RPCArg::Type::NUM, RPCArg::Default{50}, "Maximum number of jobs to return"},
            {"start_height", RPCArg::Type::NUM, RPCArg::Default{0}, "Starting block height to search from"},
            {"end_height", RPCArg::Type::NUM, RPCArg::Default{0}, "Ending block height to search to (0 = current tip)"},
        },
        RPCResult{
            RPCResult::Type::ARR, "", "",
            {
                {RPCResult::Type::OBJ, "", "",
                {
                    {RPCResult::Type::STR_HEX, "job_id", "The unique job identifier"},
                    {RPCResult::Type::STR_HEX, "txid", "The transaction ID"},
                    {RPCResult::Type::STR, "state", "Current job state"},
                    {RPCResult::Type::NUM_TIME, "timestamp", "Job creation timestamp"},
                    {RPCResult::Type::NUM, "block_height", "Block height (0 if in mempool)"},
                    {RPCResult::Type::BOOL, "in_mempool", "Whether the job is in mempool"},
                    {RPCResult::Type::OBJ, "metadata", "Job metadata",
                    {
                        {RPCResult::Type::STR, "title", "Job title"},
                        {RPCResult::Type::STR, "description", "Job description"},
                        {RPCResult::Type::STR_AMOUNT, "amount", "Job payment amount"},
                        {RPCResult::Type::NUM, "timeout_blocks", "Job timeout in blocks"},
                        {RPCResult::Type::STR, "requirements", "Job requirements"},
                        {RPCResult::Type::STR, "deliverables", "Expected deliverables"},
                    }},
                }},
            }
        },
        RPCExamples{
            HelpExampleCli("listopenjobs", "")
            + HelpExampleCli("listopenjobs", "10")
            + HelpExampleRpc("listopenjobs", "10, 100, 200")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue {
            uint32_t limit = request.params.size() > 0 ? request.params[0].getInt<uint32_t>() : 50;
            uint32_t start_height = request.params.size() > 1 ? request.params[1].getInt<uint32_t>() : 0;
            uint32_t end_height = request.params.size() > 2 ? request.params[2].getInt<uint32_t>() : 0;
            
            // Limit the maximum number of results
            if (limit > 1000) {
                limit = 1000;
            }
            
            std::vector<JobSearchResult> all_jobs;
            
            // Get jobs from contract manager
            auto contracts = JobContractManager::GetInstance().GetContractsByState(JobState::OPEN);
            
            for (const auto& contract : contracts) {
                JobSearchResult result;
                result.job_id = contract->job_id;
                result.txid = contract->funding_txid;
                result.metadata = contract->metadata;
                result.state = contract->state;
                result.timestamp = contract->metadata.created_timestamp;
                result.block_height = contract->metadata.created_height;
                result.in_mempool = false; // Contracts are stored after confirmation
                result.applications = contract->worker_applications;
                
                all_jobs.push_back(result);
            }
            
            // Also search mempool and blockchain for additional jobs
            const CTxMemPool& mempool = EnsureAnyMemPool(request.context);
            auto mempool_jobs = JobRPC::SearchMempoolForJobs(mempool);
            all_jobs.insert(all_jobs.end(), mempool_jobs.begin(), mempool_jobs.end());
            
            ChainstateManager& chainman = EnsureAnyChainman(request.context);
            auto blockchain_jobs = JobRPC::SearchBlockchainForJobs(chainman, start_height, end_height);
            all_jobs.insert(all_jobs.end(), blockchain_jobs.begin(), blockchain_jobs.end());
            
            // Remove duplicates based on job_id
            std::sort(all_jobs.begin(), all_jobs.end(), 
                [](const JobSearchResult& a, const JobSearchResult& b) {
                    return a.job_id < b.job_id;
                });
            all_jobs.erase(std::unique(all_jobs.begin(), all_jobs.end(),
                [](const JobSearchResult& a, const JobSearchResult& b) {
                    return a.job_id == b.job_id;
                }), all_jobs.end());
            
            // Sort by timestamp (newest first)
            std::sort(all_jobs.begin(), all_jobs.end(), 
                [](const JobSearchResult& a, const JobSearchResult& b) {
                    return a.timestamp > b.timestamp;
                });
            
            // Apply limit
            if (all_jobs.size() > limit) {
                all_jobs.resize(limit);
            }
            
            // Convert to JSON
            UniValue result(UniValue::VARR);
            for (const auto& job : all_jobs) {
                result.push_back(job.ToJSON());
            }
            
            return result;
        }
    };
}



} // namespace mmp

// Register MMP RPC commands (node functions only)
void RegisterMMPRPCCommands(CRPCTable& table) {
    static const CRPCCommand commands[]{
        {"mmp", &mmp::listopenjobs},
    };
    
    for (const auto& c : commands) {
        table.appendCommand(c.name, &c);
    }
}