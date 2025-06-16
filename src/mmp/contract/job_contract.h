// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_CONTRACT_JOB_CONTRACT_H
#define GOTHAM_MMP_CONTRACT_JOB_CONTRACT_H

#include <mmp/contract/job_contract_types.h>
#include <mmp/middleman/middleman.h>
#include <script/script.h>
#include <key.h>
#include <pubkey.h>
#include <serialize.h>
#include <consensus/amount.h>
#include <uint256.h>
#include <chainparams.h>
#include <string>
#include <vector>
#include <cstdint>
#include <utility>

namespace mmp {

/** Represents a full job contract between employer and worker */
class JobContract {
public:
    uint256 job_id;
    KeyAggregationContext keys;
    TaprootPaths script_paths;
    JobMetadata metadata;
    JobState state;
    uint256 funding_txid;         // Transaction that funds the escrow
    uint32_t funding_vout;        // Output index in funding transaction
    uint256 resolution_txid;      // Final resolution transaction (if any)
    ResolutionPath resolution_path;
    
    // Key rotation security
    int64_t last_key_rotation{0}; // Timestamp of last key rotation
    static constexpr int64_t KEY_ROTATION_INTERVAL = 2592000; // 30 days in seconds
    
    // Worker application management
    std::vector<WorkerApplication> worker_applications;  // List of worker applications
    CPubKey assigned_worker;      // Currently assigned worker (empty if none)
    
    // Dispute resolution management
    DisputeResolutionConfig dispute_config; // How middleman will be selected if dispute occurs
    MiddlemanInfo middleman_info; // Middleman details (only set when dispute occurs)
    bool dispute_raised{false};   // Whether a dispute has been raised
    int64_t dispute_timestamp{0}; // When dispute was raised (0 if none)
    int64_t completion_timestamp{0}; // When work was marked complete (for 24h timeout)
    CPubKey dispute_initiator;    // Who raised the dispute (employer or worker)
    std::string dispute_reason;   // Reason for the dispute
    std::vector<CPubKey> proposed_middlemen; // Middlemen proposed by each party
    bool middleman_agreed{false}; // Whether both parties agreed on middleman
    
    // Event history
    std::vector<ContractEvent> event_history;
    
    // Constructors
    JobContract() : state(JobState::CREATED), funding_vout(0), resolution_path(ResolutionPath::NONE) {}
    
    JobContract(const uint256& job_id, const std::string& title, const std::string& description,
                uint32_t created_height, int64_t created_timestamp, uint32_t timeout_blocks, 
                CAmount amount, const CPubKey& employer_pubkey)
        : job_id(job_id), state(JobState::CREATED), funding_vout(0), resolution_path(ResolutionPath::NONE) {
        
        // Initialize metadata
        metadata.title = title;
        metadata.description = description;
        metadata.amount = amount;
        metadata.created_height = created_height;
        metadata.created_timestamp = created_timestamp;
        metadata.timeout_blocks = timeout_blocks;
        
        // Initialize keys
        keys.employer_key = employer_pubkey;
        
        // Add creation event
        AddEvent(JobState::CREATED, uint256(), "Contract created");
    }
    
    JobContract(const uint256& job_id, const std::string& title, const std::string& description,
                uint32_t created_height, int64_t created_timestamp, uint32_t timeout_blocks,
                CAmount amount, const CPubKey& employer_pubkey, const Middleman& middleman)
        : job_id(job_id), state(JobState::CREATED), funding_vout(0), resolution_path(ResolutionPath::NONE) {
        
        // Initialize metadata
        metadata.title = title;
        metadata.description = description;
        metadata.amount = amount;
        metadata.created_height = created_height;
        metadata.created_timestamp = created_timestamp;
        metadata.timeout_blocks = timeout_blocks;
        
        // Initialize keys
        keys.employer_key = employer_pubkey;
        
        // Initialize middleman info
        middleman_info.id = middleman.id;
        middleman_info.name = middleman.name;
        middleman_info.pubkey = middleman.pubkey;
        middleman_info.selection_timestamp = created_timestamp;
        
        // Add creation event
        AddEvent(JobState::CREATED, uint256(), "Contract created with middleman");
    }
    
    // Serialization
    SERIALIZE_METHODS(JobContract, obj) {
        READWRITE(obj.job_id);
        READWRITE(obj.keys);
        READWRITE(obj.script_paths);
        READWRITE(obj.metadata);
        READWRITE(static_cast<uint8_t>(obj.state));
        READWRITE(obj.funding_txid);
        READWRITE(obj.funding_vout);
        READWRITE(obj.resolution_txid);
        READWRITE(static_cast<uint8_t>(obj.resolution_path));
        READWRITE(obj.last_key_rotation);
        READWRITE(obj.worker_applications);
        READWRITE(obj.assigned_worker);
        READWRITE(obj.dispute_config);
        READWRITE(obj.middleman_info);
        READWRITE(obj.dispute_raised);
        READWRITE(obj.dispute_timestamp);
        READWRITE(obj.completion_timestamp);
        READWRITE(obj.dispute_initiator);
        READWRITE(obj.dispute_reason);
        READWRITE(obj.proposed_middlemen);
        READWRITE(obj.middleman_agreed);
        READWRITE(obj.event_history);
    }
    
    // Enhanced dispute state check
    bool IsDisputeActive() const {
        return dispute_raised && 
               state == JobState::DISPUTED &&
               GetTime() - dispute_timestamp < MMP_DISPUTE_TIMEOUT_BLOCKS * 600; // Convert blocks to seconds (10 min/block)
    }
    
    // Validate keys for critical actions (security check)
    bool ValidateKeysForAction(int64_t current_time) const {
        // Reject if keys are too old but not rotated
        if (keys.last_rotation_time > 0 && // Only check if rotation has happened before
            current_time - keys.last_rotation_time > keys.MAX_KEY_AGE) {
            return false; // Force rotation first
        }
        
        // For long-running jobs, ensure keys have been rotated at least once
        if (current_time - metadata.created_timestamp > keys.ROTATION_INTERVAL * 2 && 
            !keys.keys_rotated) {
            return false; // Long-running job needs key rotation
        }
        
        return true;
    }
    
    // Check if key rotation is due
    bool IsKeyRotationDue(int64_t current_time) const {
        // First check if the job has been running long enough to need rotation
        if (current_time - metadata.created_timestamp <= keys.ROTATION_INTERVAL) {
            return false; // Job hasn't been running long enough to need rotation
        }
        
        // Now check if we've never rotated keys or if it's time for another rotation
        if (keys.last_rotation_time == 0 || 
            current_time - keys.last_rotation_time > keys.ROTATION_INTERVAL) {
            return true;
        }
        
        return false;
    }
    
    // Enhanced timeout handling with early warnings
    bool IsNearExpiration(uint32_t current_height) const {
        static constexpr uint32_t EXPIRATION_WARNING_THRESHOLD = 24; // Warning at ~4 hours remaining
        
        // Calculate blocks remaining until timeout
        uint32_t blocks_remaining = metadata.timeout_blocks - 
                                  (current_height - metadata.created_height);
        
        // Return true if we're within the warning threshold
        return blocks_remaining < EXPIRATION_WARNING_THRESHOLD && 
               blocks_remaining > 0;
    }
    
    // Enhanced key rotation mechanism for long-running jobs
    JobActionResult RotateKeys(int64_t current_time, bool emergency_rotation = false);
    
    // Validation methods
    bool IsValid() const;
    std::string GetValidationError() const;
    std::pair<bool, MMPError> ValidateEx() const;
    
    // Utility methods
    std::string ToString() const;
    bool IsResolved() const { return state == JobState::RESOLVED; }
    
    // Check if the contract is expired based on current block height
    bool IsExpired(uint32_t current_height) const MMP_THREAD_SAFE {
        // Use atomic loads to prevent TOCTOU issues
        uint32_t created = metadata.created_height;
        uint32_t timeout = metadata.timeout_blocks;
        
        // Calculate expiration with overflow protection
        if (current_height < created) {
            return false; // Handle chain reorg or overflow
        }
        
        uint32_t blocks_elapsed = current_height - created;
        return blocks_elapsed >= timeout;
    }
    
    bool CanWorkerClaimTimeout(int64_t current_timestamp) const;
    bool IsInDisputePeriod(int64_t current_timestamp) const;
    
    void AddEvent(JobState new_state, const uint256& txid = uint256(), const std::string& memo = "") {
        ContractEvent event;
        event.timestamp = GetTime();
        event.state = new_state;
        event.previous_state = state;
        event.txid = txid;
        event.memo = memo;
        event_history.push_back(event);
        
        // Update contract state if provided
        if (new_state != JobState::CREATED) { // Don't overwrite with default value
            state = new_state;
        }
    }
    
    // Centralized state transition function with validation
    StateTransitionResult TransitionState(JobState new_state, const uint256& txid = uint256(), 
                                         const std::string& memo = "");
    
    // Legacy helper methods for backward compatibility
    bool IsAccepted() const { return state >= JobState::ASSIGNED; }
    bool IsSelected() const { return state >= JobState::ASSIGNED; }
    bool IsCompleted() const { return state >= JobState::COMPLETED; }
    bool IsConfirmed() const { return state >= JobState::RESOLVED; }
    bool IsRefunded() const { return resolution_path == ResolutionPath::REFUND; }
    bool IsActive() const { return state != JobState::RESOLVED && state != JobState::CANCELLED && state != JobState::EXPIRED; }
};

/**
 * Generate a unique job ID based on employer's pubkey and other parameters
 */
uint256 GenerateJobId(const CPubKey& employer, const std::string& title, uint32_t timestamp);

/**
 * Create initial escrow script with just employer's key (for job posting)
 */
CScript CreateInitialJobEscrowScript(const CPubKey& employer);

/**
 * Create 2-of-2 escrow script (employer + worker, no middleman yet)
 */
CScript CreateTwoPartyEscrowScript(const CPubKey& employer, const CPubKey& worker);

/**
 * Create 2-of-3 escrow script (employer + worker + middleman for disputes)
 */
CScript CreateThreePartyEscrowScript(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman);

/**
 * Create appropriate escrow script based on job state
 */
CScript CreateJobEscrowScript(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman);

/**
 * Create a 2-of-3 multisig script for the job (legacy approach)
 */
CScript CreateJobMultisigScript(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman);

/**
 * Get the address for a job contract's escrow script
 */
std::string GetJobEscrowAddress(const CScript& escrow_script, const CChainParams& params);

/**
 * Create transaction to upgrade escrow from single-key to 2-of-2 when worker is selected
 */
CTransactionRef CreateWorkerSelectionTransaction(
    const uint256& job_id,
    const CPubKey& employer_pubkey,
    const CPubKey& worker_pubkey,
    const COutPoint& escrow_outpoint,
    CAmount escrow_amount);

/**
 * Create transaction to upgrade escrow from 2-of-2 to 2-of-3 when dispute occurs
 */
CTransactionRef CreateDisputeEscalationTransaction(
    const uint256& job_id,
    const CPubKey& employer_pubkey,
    const CPubKey& worker_pubkey,
    const CPubKey& middleman_pubkey,
    const COutPoint& escrow_outpoint,
    CAmount escrow_amount);

} // namespace mmp

#endif // GOTHAM_MMP_CONTRACT_JOB_CONTRACT_H