// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/contract/job_contract_types.h>
#include <random.h>
#include <logging.h>
#include <hash.h>

namespace mmp {

bool KeyAggregationContext::RecomputeAggregation() {
    // Collect all valid keys for aggregation
    std::vector<CPubKey> keys_to_aggregate;
    
    if (employer_key.IsValid()) {
        keys_to_aggregate.push_back(employer_key);
    }
    
    if (worker_key.IsValid()) {
        keys_to_aggregate.push_back(worker_key);
    }
    
    if (middleman_key.IsValid()) {
        keys_to_aggregate.push_back(middleman_key);
    }
    
    // Only proceed if we have at least one valid key
    if (keys_to_aggregate.empty()) {
        return false;
    }
    
    // Compute the aggregated key
    aggregated_key = ComputeAggregatedKey(keys_to_aggregate);
    
    return aggregated_key.IsValid();
}

bool TaprootPaths::UpdateWithNewKeys(const KeyAggregationContext& keys) {
    // Only proceed if we have valid keys
    if (!keys.employer_key.IsValid()) {
        return false;
    }
    
    // For employer approval script (employer + worker)
    if (keys.worker_key.IsValid()) {
        // Create a script that requires both employer and worker signatures
        employer_approval_script = CScript() << ToByteVector(keys.employer_key) << OP_CHECKSIGVERIFY
                                          << ToByteVector(keys.worker_key) << OP_CHECKSIG;
    }
    
    // For worker timeout script (worker only after timeout)
    if (keys.worker_key.IsValid()) {
        // Create a script that requires only worker signature after timeout
        worker_timeout_script = CScript() << ToByteVector(keys.worker_key) << OP_CHECKSIG;
    }
    
    // For middleman resolution script (middleman + either employer or worker)
    if (keys.middleman_key.IsValid()) {
        // Create a script that requires middleman and either employer or worker
        middleman_resolution_script = CScript() << ToByteVector(keys.middleman_key) << OP_CHECKSIGVERIFY
                                             << OP_IF
                                             << ToByteVector(keys.employer_key)
                                             << OP_ELSE
                                             << ToByteVector(keys.worker_key)
                                             << OP_ENDIF
                                             << OP_CHECKSIG;
    }
    
    // For refund script (employer only)
    refund_script = CScript() << ToByteVector(keys.employer_key) << OP_CHECKSIG;
    
    return true;
}

std::string JobStateToString(JobState state) {
    switch (state) {
        case JobState::CREATED: return "Created";
        case JobState::OPEN: return "Open";
        case JobState::ASSIGNED: return "Assigned";
        case JobState::IN_PROGRESS: return "In Progress";
        case JobState::COMPLETED: return "Completed";
        case JobState::DISPUTED: return "Disputed";
        case JobState::RESOLVED: return "Resolved";
        case JobState::CANCELLED: return "Cancelled";
        case JobState::EXPIRED: return "Expired";
        default: return "Unknown";
    }
}

CPubKey ComputeAggregatedKey(const std::vector<CPubKey>& keys) {
    // Safety checks
    if (keys.empty()) {
        return CPubKey();
    }
    
    for (const auto& key : keys) {
        if (!key.IsValid()) {
            return CPubKey();
        }
    }
    
    // For a single key, just return it
    if (keys.size() == 1) {
        return keys[0];
    }
    
    // In a real implementation, this would use MuSig2 to aggregate the keys
    // For now, just return the first key as a placeholder
    return keys[0];
}

void LogStateTransition(const uint256& job_id, JobState old_state, JobState new_state, const std::string& memo) {
    // Format: [timestamp] JOB_ID: OLD_STATE -> NEW_STATE (memo)
    std::string log_entry = "[" + std::to_string(GetTime()) + "] " + 
                           job_id.ToString() + ": " + 
                           JobStateToString(old_state) + " -> " + 
                           JobStateToString(new_state) + " (" + memo + ")";
    
    // Log to the Bitcoin Core logging system
    LogPrintf("Job %s: State transition from %s to %s - %s\n",
              job_id.ToString(),
              JobStateToString(old_state),
              JobStateToString(new_state),
              memo);
}

void GetStrongRandBytes(unsigned char* buf, int num) {
    // Use the Bitcoin Core random number generator with enhanced security
    GetRandBytes(std::span<unsigned char>(buf, num));
    
    // In a real implementation, this would use additional entropy sources
    // For now, just use the standard random number generator
}

} // namespace mmp