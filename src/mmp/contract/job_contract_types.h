// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_CONTRACT_JOB_CONTRACT_TYPES_H
#define GOTHAM_MMP_CONTRACT_JOB_CONTRACT_TYPES_H

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

// Forward declarations
class Middleman;

/** Job contract status codes */
enum class JobState : uint8_t {
    CREATED,    // Job is created but not yet posted
    OPEN,       // Job is posted and open for applications
    ASSIGNED,   // Employer has selected a specific worker
    IN_PROGRESS, // Work is in progress
    COMPLETED,  // Worker has marked the job as completed
    DISPUTED,   // Job is in dispute resolution
    RESOLVED,   // Job has been resolved (payment released)
    CANCELLED,  // Job was cancelled
    EXPIRED     // Job has expired
};

/** Result of job actions */
enum class JobActionResult : uint8_t {
    SUCCESS,
    INVALID_CONTRACT_STATE,
    KEY_ROTATION_NOT_DUE,
    KEY_GENERATION_FAILED,
    AGGREGATION_FAILED,
    SCRIPT_UPDATE_FAILED
};

/** Result of state transitions */
enum class StateTransitionResult : uint8_t {
    SUCCESS,
    INVALID_TRANSITION,
    TRANSITION_LOCKED
};

/** Resolution path for job contract */
enum class ResolutionPath : uint8_t {
    NONE,
    EMPLOYER_APPROVED,
    WORKER_TIMEOUT,
    MIDDLEMAN_RESOLUTION,
    REFUND
};

/** Worker application status */
enum class ApplicationStatus : uint8_t {
    PENDING,
    ACCEPTED,
    REJECTED
};

/** MMP error codes */
enum class MMPError : uint8_t {
    NONE,
    INVALID_STATE,
    INVALID_KEYS,
    INVALID_SCRIPT,
    INVALID_METADATA,
    INVALID_FUNDING
};

/** Key rotation event */
struct KeyRotationEvent {
    int64_t timestamp;
    bool is_emergency;
    std::string reason;

    SERIALIZE_METHODS(KeyRotationEvent, obj) {
        READWRITE(obj.timestamp);
        READWRITE(obj.is_emergency);
        READWRITE(obj.reason);
    }
};

/** Key aggregation context */
struct KeyAggregationContext {
    CPubKey employer_key;
    CPubKey worker_key;
    CPubKey middleman_key;
    CPubKey aggregated_key;
    bool keys_rotated{false};
    int64_t last_rotation_time{0};
    uint32_t rotation_count{0};
    std::vector<KeyRotationEvent> rotation_history;
    
    // Constants
    static constexpr int64_t ROTATION_INTERVAL = 2592000; // 30 days in seconds
    static constexpr int64_t MAX_KEY_AGE = 7776000;      // 90 days in seconds

    bool RecomputeAggregation();  // Declaration only, implementation in .cpp file

    SERIALIZE_METHODS(KeyAggregationContext, obj) {
        READWRITE(obj.employer_key);
        READWRITE(obj.worker_key);
        READWRITE(obj.middleman_key);
        READWRITE(obj.aggregated_key);
        READWRITE(obj.keys_rotated);
        READWRITE(obj.last_rotation_time);
        READWRITE(obj.rotation_count);
        READWRITE(obj.rotation_history);
    }
};

/** Taproot script paths */
struct TaprootPaths {
    CScript employer_approval_script;
    CScript worker_timeout_script;
    CScript middleman_resolution_script;
    CScript refund_script;

    bool UpdateWithNewKeys(const KeyAggregationContext& keys);  // Declaration only, implementation in .cpp file

    SERIALIZE_METHODS(TaprootPaths, obj) {
        READWRITE(obj.employer_approval_script);
        READWRITE(obj.worker_timeout_script);
        READWRITE(obj.middleman_resolution_script);
        READWRITE(obj.refund_script);
    }
};

/** Job metadata */
struct JobMetadata {
    std::string title;
    std::string description;
    CAmount amount;
    uint32_t created_height;
    int64_t created_timestamp;
    uint32_t timeout_blocks;
    std::string requirements;
    std::string deliverables;

    SERIALIZE_METHODS(JobMetadata, obj) {
        READWRITE(obj.title);
        READWRITE(obj.description);
        READWRITE(obj.amount);
        READWRITE(obj.created_height);
        READWRITE(obj.created_timestamp);
        READWRITE(obj.timeout_blocks);
        READWRITE(obj.requirements);
        READWRITE(obj.deliverables);
    }
};

/** Worker application */
struct WorkerApplication {
    CPubKey worker_pubkey;
    std::string proposal;
    int64_t timestamp;
    ApplicationStatus status;

    SERIALIZE_METHODS(WorkerApplication, obj) {
        READWRITE(obj.worker_pubkey);
        READWRITE(obj.proposal);
        READWRITE(obj.timestamp);
        READWRITE(static_cast<uint8_t>(obj.status));
    }
};

/** Middleman information */
struct MiddlemanInfo {
    uint256 id;
    std::string name;
    CPubKey pubkey;
    int64_t selection_timestamp;

    SERIALIZE_METHODS(MiddlemanInfo, obj) {
        READWRITE(obj.id);
        READWRITE(obj.name);
        READWRITE(obj.pubkey);
        READWRITE(obj.selection_timestamp);
    }
};

/** Dispute resolution configuration */
struct DisputeResolutionConfig {
    bool auto_select_middleman;
    uint32_t resolution_timeout_blocks;
    
    SERIALIZE_METHODS(DisputeResolutionConfig, obj) {
        READWRITE(obj.auto_select_middleman);
        READWRITE(obj.resolution_timeout_blocks);
    }
};

/** Contract event */
struct ContractEvent {
    int64_t timestamp;
    JobState state;
    JobState previous_state;
    uint256 txid;
    std::string memo;
    
    std::string ToString() const {
        // Simple implementation
        return memo;
    }

    SERIALIZE_METHODS(ContractEvent, obj) {
        READWRITE(obj.timestamp);
        READWRITE(static_cast<uint8_t>(obj.state));
        READWRITE(static_cast<uint8_t>(obj.previous_state));
        READWRITE(obj.txid);
        READWRITE(obj.memo);
    }
};

// Constants
static constexpr int MMP_DISPUTE_TIMEOUT_BLOCKS = 144; // ~1 day

// Forward declarations
CPubKey ComputeAggregatedKey(const std::vector<CPubKey>& keys);

// Helper functions
std::string JobStateToString(JobState state);
void LogStateTransition(const uint256& job_id, JobState old_state, JobState new_state, const std::string& memo);
void GetStrongRandBytes(unsigned char* buf, int num);

// Thread safety macros (placeholders for now)
#define MMP_THREAD_SAFE
#define MMP_REQUIRES_LOCK
#define MMP_REENTRANCY_GUARD(x)

} // namespace mmp

#endif // GOTHAM_MMP_CONTRACT_JOB_CONTRACT_TYPES_H