# Enhanced MMP Workflow Examples

## Overview

This document provides comprehensive examples of the enhanced MMP system workflows, including the new security features, bond slashing system, and appeal process.

## 1. Basic Job Workflow with Enhanced Security

### Example: Software Development Job

```cpp
// 1. Employer creates job with enhanced dispute config
JobContract contract = CreateJobContract(employer_key, metadata);

// Enhanced dispute resolution configuration
DisputeResolutionConfig config;
config.selection_method = MiddlemanSelectionMethod::MUTUAL_AGREEMENT;
config.selection_penalty_sats = 10000;  // 0.0001 BTC penalty for unreasonable rejections
config.selection_timeout_blocks = 144;  // 24h timeout
config.min_middleman_bond_sats = 100000; // 0.001 BTC minimum bond
config.min_reputation_score = 70;       // Require good reputation
config.fallback_arbitrator = legal_service_key; // KYC'd legal service as fallback

SetDisputeResolutionConfig(contract, config);

// 2. Lock funds with enhanced validation
LockFundsResult lock_result = LockFunds(contract, funding_tx, vout);
assert(lock_result == LockFundsResult::SUCCESS);

// 3. Worker applies with qualification check
WorkerApplication application;
application.worker_key = worker_key;
application.message = "5 years experience in React/Node.js";
application.portfolio_url = "https://github.com/worker";

JobActionResult apply_result = ApplyForJob(contract, application);
assert(apply_result == JobActionResult::SUCCESS);

// 4. Employer assigns worker
JobActionResult assign_result = AssignWorker(contract, employer_key, worker_key);
assert(assign_result == JobActionResult::SUCCESS);

// 5. Worker accepts job
JobActionResult accept_result = AcceptJob(contract, worker_key);
assert(accept_result == JobActionResult::SUCCESS);

// 6. Worker submits work
JobActionResult submit_result = SubmitWork(contract, "https://github.com/worker/project");
assert(submit_result == JobActionResult::SUCCESS);

// 7a. Happy path - no dispute, worker claims after 24h
if (!contract.dispute_raised) {
    // Wait 24 hours (144 blocks)
    JobActionResult timeout_result = WorkerClaimTimeout(contract, worker_key);
    assert(timeout_result == JobActionResult::SUCCESS);
}

// 7b. Dispute path - employer raises dispute
else {
    // Employer finds qualified middleman
    std::vector<MiddlemanInfo> suggested = GetSuggestedMiddlemen(contract, 5);
    MiddlemanInfo selected_middleman;
    
    for (const auto& mm : suggested) {
        if (contract.dispute_config.IsMiddlemanQualified(mm) && 
            mm.IsQualifiedForJob(contract.job_amount_sats, 70)) {
            selected_middleman = mm;
            break;
        }
    }
    
    // Raise dispute with proposed middleman
    JobActionResult dispute_result = RaiseDispute(
        contract, 
        employer_key, 
        "Work delivered late and incomplete", 
        selected_middleman.pubkey
    );
    assert(dispute_result == JobActionResult::SUCCESS);
    
    // Worker responds to middleman proposal
    JobActionResult response_result = RespondToMiddlemanProposal(
        contract, 
        worker_key, 
        true  // Accept the proposed middleman
    );
    assert(response_result == JobActionResult::SUCCESS);
    
    // Middleman resolves dispute with evidence
    std::vector<uint8_t> evidence_hash = SHA256("Evidence: Screenshots, git commits, communication logs");
    JobActionResult resolution_result = MiddlemanResolveDispute(
        contract,
        selected_middleman.pubkey,
        ResolutionPath::MIDDLEMAN_SPLIT,  // 70% to worker, 30% to employer
        "Work was delivered with minor delays but meets requirements. Split payment fair.",
        evidence_hash
    );
    assert(resolution_result == JobActionResult::SUCCESS);
}
```

## 2. Bond Slashing Workflow with Appeal Process

### Example: Middleman Misconduct

```cpp
// 1. Detect middleman misconduct (e.g., biased decision, collusion)
CPubKey bad_middleman_key = /* ... */;
CPubKey reporter_key = /* ... */;

// 2. Create slash proposal with evidence
BondSlashProposal slash_proposal;
slash_proposal.middleman_key = bad_middleman_key;
slash_proposal.slash_amount_sats = 50000;  // Slash 50% of bond
slash_proposal.reason = "Middleman showed clear bias, ignored evidence, possible collusion";
slash_proposal.evidence_hash = SHA256("Chat logs, transaction analysis, witness statements");
slash_proposal.proposer_key = reporter_key;
slash_proposal.proposal_timestamp = GetCurrentTimestamp();
slash_proposal.challenge_period_blocks = 1008;  // 1 week for appeals

// 3. Submit slash proposal to DAO
JobActionResult proposal_result = ProposeBondSlash(slash_proposal);
assert(proposal_result == JobActionResult::SUCCESS);

// 4. DAO reviews and approves (requires 3+ approvers)
std::vector<CPubKey> dao_approvers = {dao_member1, dao_member2, dao_member3, dao_member4};
// ... DAO voting process ...

// 5. Middleman appeals the decision
BondSlashAppeal appeal;
appeal.proposal_hash = Hash(slash_proposal);
appeal.appellant_key = bad_middleman_key;
appeal.appeal_reason = "Decision was correct based on available evidence. Reporter misunderstood the situation.";
appeal.counter_evidence_hash = SHA256("Additional evidence showing decision was justified");
appeal.appeal_timestamp = GetCurrentTimestamp();

JobActionResult appeal_result = AppealBondSlash(appeal);
assert(appeal_result == JobActionResult::SUCCESS);

// 6. DAO reviews appeal
std::vector<CPubKey> appeal_reviewers = {dao_member1, dao_member5, dao_member6};
// ... DAO appeal review process ...

// 7a. Appeal denied - execute slash
JobActionResult slash_execution = ExecuteBondSlash(
    Hash(slash_proposal), 
    dao_approvers
);
assert(slash_execution == JobActionResult::SUCCESS);

// 7b. Appeal granted - cancel slash
JobActionResult appeal_resolution = ResolveBondSlashAppeal(
    Hash(appeal),
    true,  // Grant appeal
    appeal_reviewers
);
assert(appeal_resolution == JobActionResult::SUCCESS);
```

## 3. Advanced Middleman Selection Scenarios

### Scenario A: High-Value Job with Reputation-Based Selection

```cpp
// High-value job requiring top-tier middleman
JobContract high_value_contract = CreateJobContract(employer_key, metadata);
high_value_contract.job_amount_sats = 10000000;  // 0.1 BTC job

DisputeResolutionConfig high_security_config;
high_security_config.selection_method = MiddlemanSelectionMethod::REPUTATION_BASED;
high_security_config.min_reputation_score = 90;  // Only top-tier middlemen
high_security_config.min_middleman_bond_sats = 1000000;  // 0.01 BTC minimum bond
high_security_config.selection_penalty_sats = 50000;  // Higher penalty for rejections

SetDisputeResolutionConfig(high_value_contract, high_security_config);

// When dispute occurs, system auto-selects highest reputation middleman
if (dispute_raised) {
    JobActionResult auto_select = AutoSelectMiddleman(high_value_contract);
    assert(auto_select == JobActionResult::SUCCESS);
    
    // Verify selected middleman meets requirements
    assert(high_value_contract.middleman_info.GetEffectiveReputation() >= 90);
    assert(high_value_contract.middleman_info.bond_amount_sats >= 1000000);
    assert(high_value_contract.middleman_info.is_dao_approved);
}
```

### Scenario B: Community Pool with Fallback

```cpp
// Community-driven selection with legal fallback
DisputeResolutionConfig community_config;
community_config.selection_method = MiddlemanSelectionMethod::RANDOM_FROM_POOL;
community_config.approved_pool = GetDAOApprovedMiddlemen();  // DAO-curated pool
community_config.fallback_arbitrator = kyc_legal_service;   // KYC'd legal service
community_config.selection_timeout_blocks = 72;  // 12h timeout for quick resolution

// Filter pool to only qualified middlemen
std::vector<CPubKey> qualified_pool;
for (const auto& mm_key : community_config.approved_pool) {
    MiddlemanInfo* mm = GetMiddlemanInfo(mm_key);
    if (mm && community_config.IsMiddlemanQualified(*mm)) {
        qualified_pool.push_back(mm_key);
    }
}
community_config.approved_pool = qualified_pool;

SetDisputeResolutionConfig(contract, community_config);
```

## 4. Economic Security Validation Examples

### Example: Middleman Registration with Enhanced Validation

```cpp
// Create middleman profile with economic security checks
MiddlemanInfo new_middleman;
new_middleman.pubkey = middleman_key;
new_middleman.middleman_name = "Expert Arbitrator LLC";
new_middleman.fee_sats = 25000;           // 0.00025 BTC fee
new_middleman.bond_amount_sats = 100000;  // 0.001 BTC bond
new_middleman.max_job_amount_sats = 5000000; // Handle up to 0.05 BTC jobs
new_middleman.reputation_score = 85;
new_middleman.is_dao_approved = true;
new_middleman.specialties = {"SOFTWARE", "DESIGN", "LEGAL"};

// Validation checks (all must pass)
assert(new_middleman.IsValid());  // Comprehensive validation
assert(new_middleman.fee_sats <= new_middleman.bond_amount_sats);  // Fee ≤ bond
assert(new_middleman.bond_amount_sats >= new_middleman.max_job_amount_sats / 20);  // Bond ≥ 5% of max job

// Economic ratio checks
assert(new_middleman.fee_sats <= new_middleman.bond_amount_sats * MMP_MAX_FEE_TO_BOND_RATIO);
assert(new_middleman.bond_amount_sats >= new_middleman.max_job_amount_sats * MMP_MIN_BOND_TO_JOB_RATIO);

// Job qualification checks
uint64_t test_job_amount = 1000000;  // 0.01 BTC job
assert(new_middleman.IsQualifiedForJob(test_job_amount, 70));
assert(new_middleman.CanHandleJob(test_job_amount));
```

## 5. Time-Weighted Reputation Example

### Example: Reputation Decay Over Time

```cpp
// Middleman with historical activity
MiddlemanInfo experienced_middleman;
experienced_middleman.reputation_score = 95;
experienced_middleman.total_disputes = 100;
experienced_middleman.successful_resolutions = 92;
experienced_middleman.bond_slashes = 2;
experienced_middleman.last_active_timestamp = GetCurrentTimestamp() - (6 * 30 * 24 * 3600); // 6 months ago

// Current reputation calculations
uint32_t current_reputation = experienced_middleman.GetEffectiveReputation();
// = 95 - (2 * 100 / 100) = 93

double time_weighted_reputation = experienced_middleman.GetTimeWeightedReputation(GetCurrentTimestamp());
// Applies time decay for 6 months of inactivity
// ≈ 93 * 0.85 = ~79

// Success rate
double success_rate = experienced_middleman.GetSuccessRate();
// = 92/100 = 0.92 (92%)

// Qualification for different scenarios
assert(experienced_middleman.IsHighReputation());  // 93 >= 50
assert(current_reputation >= 80);  // Good for reputation-based selection
assert(time_weighted_reputation >= 70);  // Still qualified but with time penalty
```

## 6. Security Considerations in Practice

### Example: Comprehensive Security Validation

```cpp
// Before allowing middleman to resolve dispute
JobActionResult ValidateMiddlemanResolution(
    const JobContract& contract,
    const CPubKey& middleman_key,
    ResolutionPath resolution,
    const std::string& reasoning,
    const std::vector<uint8_t>& evidence_hash
) {
    // 1. Verify middleman is authorized for this contract
    if (!contract.middleman_info.IsValid() || 
        contract.middleman_info.pubkey != middleman_key) {
        return JobActionResult::MIDDLEMAN_NOT_AUTHORIZED;
    }
    
    // 2. Check middleman still meets qualification requirements
    if (!contract.dispute_config.IsMiddlemanQualified(contract.middleman_info)) {
        return JobActionResult::REPUTATION_TOO_LOW;
    }
    
    // 3. Validate resolution reasoning
    if (reasoning.empty() || reasoning.length() > MMP_MAX_DISPUTE_REASON_LENGTH) {
        return JobActionResult::RESOLUTION_REASONING_INVALID;
    }
    
    // 4. Require evidence hash for transparency
    if (evidence_hash.empty()) {
        return JobActionResult::EVIDENCE_HASH_REQUIRED;
    }
    
    // 5. Verify resolution path is valid for current state
    if (contract.state != JobState::DISPUTED) {
        return JobActionResult::INVALID_STATE_TRANSITION;
    }
    
    // 6. Check if middleman bond is still intact
    if (contract.middleman_info.bond_slashes > contract.middleman_info.total_disputes / 4) {
        return JobActionResult::MIDDLEMAN_BOND_SLASHED;  // Too many slashes
    }
    
    return JobActionResult::SUCCESS;
}
```

## Summary

The enhanced MMP system provides:

1. **Economic Security**: Fee/bond ratio validation, risk-based bonding
2. **Reputation Integrity**: Time-weighted reputation, slash penalties
3. **Appeal Process**: Fair bond slashing with challenge periods
4. **Comprehensive Validation**: Multi-layer security checks
5. **Flexible Selection**: Multiple middleman selection methods
6. **Transparency**: Evidence hashing, on-chain dispute records

This creates a robust, production-ready system that balances trust-minimization with practical dispute resolution needs.