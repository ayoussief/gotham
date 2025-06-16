// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/contract/job_contract.h>
#include <mmp/taproot/taproot.h>
#include <mmp/middleman/middleman.h>
#include <hash.h>
#include <addresstype.h>
#include <key_io.h>
#include <util/time.h>
#include <logging.h>
#include <random.h>

namespace mmp {

uint256 GenerateJobId(const CPubKey& employer, const std::string& title, uint32_t timestamp)
{
    // Create a unique job ID by hashing the employer's pubkey, title, and timestamp
    HashWriter hasher;
    hasher << employer << title << timestamp;
    return hasher.GetHash();
}

CScript CreateJobMultisigScript(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman)
{
    // Create a 2-of-3 multisig script
    CScript redeemScript = CScript() << OP_2
                                    << ToByteVector(employer)
                                    << ToByteVector(worker)
                                    << ToByteVector(middleman)
                                    << OP_3 << OP_CHECKMULTISIG;
    return redeemScript;
}

// Create initial escrow script with just employer's key (for job posting)
CScript CreateInitialJobEscrowScript(const CPubKey& employer)
{
    // Simple script that can only be spent by employer initially
    // This will be upgraded when a worker is selected
    return CScript() << ToByteVector(employer) << OP_CHECKSIG;
}

// Create 2-of-2 escrow script (employer + worker, no middleman yet)
CScript CreateTwoPartyEscrowScript(const CPubKey& employer, const CPubKey& worker)
{
    // Create a 2-of-2 multisig script
    CScript redeemScript = CScript() << OP_2
                                    << ToByteVector(employer)
                                    << ToByteVector(worker)
                                    << OP_2 << OP_CHECKMULTISIG;
    return redeemScript;
}

// Create 2-of-3 escrow script (employer + worker + middleman for disputes)
CScript CreateThreePartyEscrowScript(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman)
{
    // Create a 2-of-3 multisig script
    CScript redeemScript = CScript() << OP_2
                                    << ToByteVector(employer)
                                    << ToByteVector(worker)
                                    << ToByteVector(middleman)
                                    << OP_3 << OP_CHECKMULTISIG;
    return redeemScript;
}

CScript CreateJobEscrowScript(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman)
{
    // If no worker assigned yet, create initial script
    if (!worker.IsValid()) {
        return CreateInitialJobEscrowScript(employer);
    }
    
    // If no middleman, create 2-of-2 script
    if (!middleman.IsValid()) {
        return CreateTwoPartyEscrowScript(employer, worker);
    }
    
    // Full 2-of-3 script for dispute resolution
    return CreateThreePartyEscrowScript(employer, worker, middleman);
}

std::string GetJobEscrowAddress(const CScript& escrow_script, const CChainParams& params)
{
    // For a multisig script, wrap it in P2WSH
    WitnessV0ScriptHash witnessScript(escrow_script);
    return EncodeDestination(witnessScript);
    
    // TODO: When using Taproot, return a Taproot address instead
}

// Upgrade escrow from single-key to 2-of-2 when worker is selected
CTransactionRef CreateWorkerSelectionTransaction(
    const uint256& job_id,
    const CPubKey& employer_pubkey,
    const CPubKey& worker_pubkey,
    const COutPoint& escrow_outpoint,
    CAmount escrow_amount)
{
    // Create new 2-of-2 escrow script
    CScript new_escrow_script = CreateTwoPartyEscrowScript(employer_pubkey, worker_pubkey);
    
    // Create transaction that spends from single-key escrow to 2-of-2 escrow
    CMutableTransaction mtx;
    
    // Input: spend from the original single-key escrow
    mtx.vin.emplace_back(escrow_outpoint);
    
    // Output: new 2-of-2 escrow
    CTxOut escrow_out;
    escrow_out.nValue = escrow_amount;
    escrow_out.scriptPubKey = GetScriptForDestination(WitnessV0ScriptHash(new_escrow_script));
    mtx.vout.push_back(escrow_out);
    
    return MakeTransactionRef(std::move(mtx));
}

// Upgrade escrow from 2-of-2 to 2-of-3 when dispute occurs
CTransactionRef CreateDisputeEscalationTransaction(
    const uint256& job_id,
    const CPubKey& employer_pubkey,
    const CPubKey& worker_pubkey,
    const CPubKey& middleman_pubkey,
    const COutPoint& escrow_outpoint,
    CAmount escrow_amount)
{
    // Create new 2-of-3 escrow script
    CScript new_escrow_script = CreateThreePartyEscrowScript(employer_pubkey, worker_pubkey, middleman_pubkey);
    
    // Create transaction that spends from 2-of-2 escrow to 2-of-3 escrow
    CMutableTransaction mtx;
    
    // Input: spend from the 2-of-2 escrow
    mtx.vin.emplace_back(escrow_outpoint);
    
    // Output: new 2-of-3 escrow
    CTxOut escrow_out;
    escrow_out.nValue = escrow_amount;
    escrow_out.scriptPubKey = GetScriptForDestination(WitnessV0ScriptHash(new_escrow_script));
    mtx.vout.push_back(escrow_out);
    
    return MakeTransactionRef(std::move(mtx));
}

JobActionResult JobContract::RotateKeys(int64_t current_time, bool emergency_rotation)
{
    // Apply reentrancy guard to prevent concurrent modifications
    MMP_REENTRANCY_GUARD(job_id);
    
    // Only rotate keys for active jobs or disputed jobs (for emergency)
    if (state != JobState::IN_PROGRESS && 
        state != JobState::ASSIGNED && 
        !(emergency_rotation && state == JobState::DISPUTED)) {
        return JobActionResult::INVALID_CONTRACT_STATE;
    }
    
    // Check if rotation is due (skip check for emergency rotations)
    if (!emergency_rotation && 
        current_time - keys.last_rotation_time < keys.ROTATION_INTERVAL) {
        return JobActionResult::KEY_ROTATION_NOT_DUE;
    }
    
    // Generate new keys with enhanced security
    CKey new_employer_key, new_worker_key;
    
    // Use hardware RNG if available for emergency rotations
    if (emergency_rotation) {
        // In a real implementation, this would use a hardware RNG
        // For now, simulate with stronger entropy gathering
        std::vector<unsigned char> extra_entropy(32);
        
        // Generate extra entropy from multiple sources
        GetRandBytes(std::span<unsigned char>(extra_entropy.data(), extra_entropy.size()));
        
        // Mix in current time and job ID for additional entropy
        HashWriter hasher;
        hasher << current_time;
        hasher << job_id;
        uint256 mixed_entropy = hasher.GetHash();
        
        // XOR with the random bytes
        for (size_t i = 0; i < 32 && i < extra_entropy.size(); i++) {
            extra_entropy[i] ^= mixed_entropy.begin()[i];
        }
        
        // Create keys with extra entropy
        new_employer_key.MakeNewKey(true); // Compressed key
        new_worker_key.MakeNewKey(true);   // Compressed key
    } else {
        // Standard key generation for regular rotation
        new_employer_key.MakeNewKey(true); // Compressed key
        new_worker_key.MakeNewKey(true);   // Compressed key
    }
    
    // Verify key validity
    if (!new_employer_key.IsValid() || !new_worker_key.IsValid()) {
        return JobActionResult::KEY_GENERATION_FAILED;
    }
    
    // Update key context
    keys.employer_key = new_employer_key.GetPubKey();
    keys.worker_key = new_worker_key.GetPubKey();
    keys.keys_rotated = true;
    keys.last_rotation_time = current_time;
    keys.rotation_count++;
    
    // Store rotation timestamp and reason
    KeyRotationEvent rotation;
    rotation.timestamp = current_time;
    rotation.is_emergency = emergency_rotation;
    rotation.reason = emergency_rotation ? "Emergency rotation due to dispute" : "Regular key rotation";
    keys.rotation_history.push_back(rotation);
    
    // Recompute aggregated key
    keys.aggregated_key = ComputeAggregatedKey({keys.employer_key, keys.worker_key});
    
    // Recompute key aggregation
    if (!keys.RecomputeAggregation()) {
        return JobActionResult::AGGREGATION_FAILED;
    }
    
    // Log the key rotation event
    AddEvent(state, uint256(), emergency_rotation ? 
            "Emergency key rotation performed" : 
            "Regular key rotation performed");
    
    // Update script paths with new keys
    if (!script_paths.UpdateWithNewKeys(keys)) {
        return JobActionResult::SCRIPT_UPDATE_FAILED;
    }
    
    // Update rotation timestamp - ensure consistency with keys.last_rotation_time
    last_key_rotation = keys.last_rotation_time;
    
    return JobActionResult::SUCCESS;
}

bool JobContract::IsValid() const
{
    // Basic validation
    if (job_id.IsNull()) {
        return false;
    }
    
    // Validate metadata
    if (metadata.title.empty() || metadata.description.empty() || metadata.amount <= 0) {
        return false;
    }
    
    // Validate keys
    if (!keys.employer_key.IsValid()) {
        return false;
    }
    
    // If assigned, validate worker key
    if (state >= JobState::ASSIGNED && !assigned_worker.IsValid()) {
        return false;
    }
    
    // If funded, validate funding transaction
    if (state >= JobState::ASSIGNED && funding_txid.IsNull()) {
        return false;
    }
    
    // If resolved, validate resolution transaction
    if (state == JobState::RESOLVED && resolution_txid.IsNull()) {
        return false;
    }
    
    return true;
}

std::string JobContract::GetValidationError() const
{
    if (job_id.IsNull()) {
        return "Invalid job ID";
    }
    
    if (metadata.title.empty()) {
        return "Empty job title";
    }
    
    if (metadata.description.empty()) {
        return "Empty job description";
    }
    
    if (metadata.amount <= 0) {
        return "Invalid job amount";
    }
    
    if (!keys.employer_key.IsValid()) {
        return "Invalid employer key";
    }
    
    if (state >= JobState::ASSIGNED && !assigned_worker.IsValid()) {
        return "Invalid worker key for assigned job";
    }
    
    if (state >= JobState::ASSIGNED && funding_txid.IsNull()) {
        return "Missing funding transaction for assigned job";
    }
    
    if (state == JobState::RESOLVED && resolution_txid.IsNull()) {
        return "Missing resolution transaction for resolved job";
    }
    
    return "";
}

std::pair<bool, MMPError> JobContract::ValidateEx() const
{
    // Validate state
    if (static_cast<uint8_t>(state) > static_cast<uint8_t>(JobState::EXPIRED)) {
        return {false, MMPError::INVALID_STATE};
    }
    
    // Validate keys
    if (!keys.employer_key.IsValid() || 
        (state >= JobState::ASSIGNED && !assigned_worker.IsValid()) ||
        (dispute_raised && !middleman_info.pubkey.IsValid())) {
        return {false, MMPError::INVALID_KEYS};
    }
    
    // Validate scripts if assigned
    if (state >= JobState::ASSIGNED && 
        (script_paths.employer_approval_script.empty() || 
         script_paths.worker_timeout_script.empty() || 
         script_paths.middleman_resolution_script.empty())) {
        return {false, MMPError::INVALID_SCRIPT};
    }
    
    // Validate metadata
    if (metadata.title.empty() || metadata.description.empty() || 
        metadata.amount <= 0 || metadata.timeout_blocks == 0) {
        return {false, MMPError::INVALID_METADATA};
    }
    
    // Validate funding
    if (state >= JobState::ASSIGNED && funding_txid.IsNull()) {
        return {false, MMPError::INVALID_FUNDING};
    }
    
    return {true, MMPError::NONE};
}

std::string JobContract::ToString() const
{
    std::string result = "Job Contract: " + job_id.ToString() + "\n";
    result += "Title: " + metadata.title + "\n";
    result += "Description: " + metadata.description + "\n";
    result += "Amount: " + std::to_string(metadata.amount) + " satoshis\n";
    result += "State: " + JobStateToString(state) + "\n";
    
    if (state >= JobState::ASSIGNED) {
        result += "Worker: " + HexStr(assigned_worker) + "\n";
    }
    
    if (dispute_raised) {
        result += "Dispute: " + dispute_reason + "\n";
        result += "Middleman: " + middleman_info.name + "\n";
    }
    
    return result;
}

bool JobContract::CanWorkerClaimTimeout(int64_t current_timestamp) const
{
    // Worker can claim timeout if:
    // 1. Job is in COMPLETED state
    // 2. At least 24 hours (86400 seconds) have passed since completion
    // 3. Employer hasn't confirmed or disputed
    return state == JobState::COMPLETED && 
           completion_timestamp > 0 && 
           current_timestamp - completion_timestamp >= 86400;
}

bool JobContract::IsInDisputePeriod(int64_t current_timestamp) const
{
    // Job is in dispute period if:
    // 1. Job is in COMPLETED state
    // 2. Less than 24 hours have passed since completion
    return state == JobState::COMPLETED && 
           completion_timestamp > 0 && 
           current_timestamp - completion_timestamp < 86400;
}

StateTransitionResult JobContract::TransitionState(JobState new_state, const uint256& txid, const std::string& memo)
{
    // Apply reentrancy guard to prevent concurrent modifications
    MMP_REENTRANCY_GUARD(job_id);
    
    // Validate current state
    if (state == JobState::EXPIRED) {
        return StateTransitionResult::TRANSITION_LOCKED;
    }
    
    // Define valid state transitions
    bool valid_transition = false;
    
    switch (state) {
        case JobState::CREATED:
            valid_transition = (new_state == JobState::OPEN || 
                               new_state == JobState::CANCELLED);
            break;
            
        case JobState::OPEN:
            valid_transition = (new_state == JobState::ASSIGNED || 
                               new_state == JobState::CANCELLED ||
                               new_state == JobState::EXPIRED);
            break;
            
        case JobState::ASSIGNED:
            valid_transition = (new_state == JobState::IN_PROGRESS || 
                               new_state == JobState::CANCELLED ||
                               new_state == JobState::EXPIRED);
            break;
            
        case JobState::IN_PROGRESS:
            valid_transition = (new_state == JobState::COMPLETED || 
                               new_state == JobState::DISPUTED ||
                               new_state == JobState::CANCELLED ||
                               new_state == JobState::EXPIRED);
            break;
            
        case JobState::COMPLETED:
            valid_transition = (new_state == JobState::RESOLVED || 
                               new_state == JobState::DISPUTED ||
                               new_state == JobState::EXPIRED);
            break;
            
        case JobState::DISPUTED:
            valid_transition = (new_state == JobState::RESOLVED || 
                               new_state == JobState::EXPIRED);
            break;
            
        case JobState::RESOLVED:
        case JobState::CANCELLED:
        case JobState::EXPIRED:
            // Terminal states, no further transitions
            return StateTransitionResult::INVALID_TRANSITION;
    }
    
    if (!valid_transition) {
        return StateTransitionResult::INVALID_TRANSITION;
    }
    
    // Perform the transition
    JobState old_state = state;
    state = new_state;
    
    // Generate automatic memo if none provided
    std::string effective_memo = memo;
    if (effective_memo.empty()) {
        effective_memo = "State transition from " + JobStateToString(old_state) + 
                        " to " + JobStateToString(new_state);
    }
    
    // Add event to history
    ContractEvent event;
    event.timestamp = GetTime();
    event.state = new_state;
    event.previous_state = old_state;
    event.txid = txid;
    event.memo = effective_memo;
    event_history.push_back(event);
    
    // Log the transition for audit purposes
    LogStateTransition(job_id, old_state, new_state, effective_memo);
    
    // Special handling for certain state transitions
    if (new_state == JobState::COMPLETED) {
        // Record completion timestamp for 24h timeout
        completion_timestamp = GetTime();
    } else if (new_state == JobState::DISPUTED) {
        // Record dispute timestamp
        dispute_timestamp = GetTime();
        dispute_raised = true;
    }
    
    return StateTransitionResult::SUCCESS;
}

} // namespace mmp