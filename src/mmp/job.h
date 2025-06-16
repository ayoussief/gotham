// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_JOB_H
#define GOTHAM_MMP_JOB_H

#include <mmp/contract/job_contract.h>
#include <mmp/contract/job_contract_types.h>
#include <mmp/middleman/middleman.h>
#include <rpc/server.h>
#include <rpc/util.h>
#include <univalue.h>
#include <node/context.h>
#include <txmempool.h>
#include <chain.h>
#include <primitives/transaction.h>
#include <uint256.h>
#include <consensus/amount.h>
#include <sync.h>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <memory>

namespace mmp {

/** Job contract storage and management */
class JobContractManager {
private:
    mutable RecursiveMutex cs_contracts;
    std::map<uint256, std::shared_ptr<JobContract>> contracts GUARDED_BY(cs_contracts);
    
public:
    // Store a job contract
    void StoreContract(const std::shared_ptr<JobContract>& contract);
    
    // Retrieve a job contract
    std::shared_ptr<JobContract> GetContract(const uint256& job_id) const;
    
    // Get all contracts
    std::vector<std::shared_ptr<JobContract>> GetAllContracts() const;
    
    // Get contracts by state
    std::vector<std::shared_ptr<JobContract>> GetContractsByState(JobState state) const;
    
    // Update contract state
    bool UpdateContractState(const uint256& job_id, JobState new_state, const uint256& txid = uint256(), const std::string& memo = "");
    
    // Add application to contract
    bool AddApplicationToContract(const uint256& job_id, const WorkerApplication& application);
    
    // Remove contract
    bool RemoveContract(const uint256& job_id);
    
    // Get singleton instance
    static JobContractManager& GetInstance();
};

/** Job search result for RPC responses */
struct JobSearchResult {
    uint256 job_id;
    uint256 txid;
    JobMetadata metadata;
    JobState state;
    int64_t timestamp;
    uint32_t block_height;
    bool in_mempool;
    std::vector<WorkerApplication> applications;
    
    JobSearchResult() = default;
    
    // Convert to JSON for RPC response
    UniValue ToJSON() const;
};

/** Job management functions for RPC commands */
class JobRPC {
public:
    // Search for job transactions in mempool
    static std::vector<JobSearchResult> SearchMempoolForJobs(const CTxMemPool& mempool);
    
    // Search for job transactions in blockchain
    static std::vector<JobSearchResult> SearchBlockchainForJobs(
        const ChainstateManager& chainman, 
        uint32_t start_height = 0, 
        uint32_t end_height = 0
    );
    
    // Check if a transaction contains job data
    static bool IsJobTransaction(const CTransactionRef& tx);
    
    // Parse job data from transaction
    static std::optional<JobSearchResult> ParseJobTransaction(
        const CTransactionRef& tx, 
        bool in_mempool = false, 
        uint32_t block_height = 0
    );
    
    // Create and broadcast job posting transaction
    static UniValue CreateJobPostTransaction(
        const std::string& title,
        const std::string& description,
        CAmount amount,
        uint32_t timeout_blocks,
        const std::string& requirements,
        const std::string& deliverables,
        const JSONRPCRequest& request
    );
    
    // Create and broadcast job application transaction
    static UniValue CreateJobApplicationTransaction(
        const uint256& job_id,
        const std::string& proposal,
        const JSONRPCRequest& request
    );
    
    // Create OP_RETURN data for job posting
    static std::vector<unsigned char> CreateJobPostingData(
        const uint256& job_id,
        const std::string& title,
        const std::string& description,
        CAmount amount,
        uint32_t timeout_blocks,
        const std::string& requirements,
        const std::string& deliverables
    );
    
    // Create OP_RETURN data for job application
    static std::vector<unsigned char> CreateJobApplicationData(
        const uint256& job_id,
        const std::string& proposal,
        const CPubKey& worker_pubkey
    );
    
    // Validate job post parameters
    static bool ValidateJobPost(
        const std::string& title,
        const std::string& description,
        CAmount amount,
        uint32_t timeout_blocks,
        std::string& error
    );
    
    // Validate job application parameters
    static bool ValidateJobApplication(
        const uint256& job_id,
        const std::string& proposal,
        std::string& error
    );
};

// RPC command implementations
RPCHelpMan createjobpost();
RPCHelpMan listopenjobs();
RPCHelpMan applytojob();

} // namespace mmp

// Register MMP RPC commands (defined in job.cpp)
void RegisterMMPRPCCommands(class CRPCTable& table);

#endif // GOTHAM_MMP_JOB_H