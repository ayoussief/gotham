# Two-Phase Escrow System with Dynamic Middleman Selection

## Overview

The updated MMP system implements a **two-phase escrow** approach that eliminates the need for pre-selecting middlemen while maintaining strong dispute resolution capabilities.

## How It Works

### Phase 1: Normal Operation (No Middleman Needed)
1. **Job Creation**: Employer posts job with `CreateJobContract(employer, metadata)` - **NO middleman required**
2. **Escrow Setup**: Funds locked in 2-of-2 (Employer + Worker) Taproot address
3. **Work Completion**: Worker submits work, 24-hour dispute window begins
4. **Happy Path**: If no dispute in 24h → Worker can claim funds alone via timeout path

### Phase 2: Dispute Resolution (Middleman Selected Dynamically)
Only when a dispute actually occurs:

1. **Dispute Raised**: Either party calls `RaiseDispute()` with proposed middleman
2. **Middleman Selection**: Multiple methods available (see below)
3. **Arbitration**: Selected middleman resolves dispute
4. **Resolution**: Funds distributed according to middleman's decision

## Middleman Selection Methods

### 1. Mutual Agreement (Default)
```cpp
// Both parties must agree on the same middleman
JobActionResult ProposeMiddleman(contract, proposer_key, middleman_key);
JobActionResult AcceptMiddleman(contract, accepter_key, middleman_key);
```

**Process:**
- Disputer proposes a middleman
- Other party can accept or propose alternative
- Continue until both agree

### 2. Random from Pre-Approved Pool
```cpp
DisputeResolutionConfig config;
config.selection_method = MiddlemanSelectionMethod::RANDOM_FROM_POOL;
config.approved_middlemen_pool = {middleman1_key, middleman2_key, middleman3_key};
SetDisputeResolutionConfig(contract, config);
```

**Process:**
- Both parties agree on a pool of acceptable middlemen during job creation
- If dispute occurs, system randomly selects from the pool
- No further negotiation needed

### 3. Reputation-Based Auto-Selection
```cpp
config.selection_method = MiddlemanSelectionMethod::REPUTATION_BASED;
AutoSelectMiddleman(contract, MiddlemanSelectionMethod::REPUTATION_BASED);
```

**Process:**
- System automatically selects highest-rated available middleman
- Based on reputation score, successful resolutions, etc.
- Fast resolution, no negotiation delays

### 4. Employer/Worker Choice with Penalties
```cpp
config.selection_method = MiddlemanSelectionMethod::EMPLOYER_CHOICE;
config.selection_penalty_sats = 10000; // Penalty for unreasonable rejection
```

**Process:**
- One party gets to choose middleman
- Other party can reject but pays penalty if rejection is deemed unreasonable
- Incentivizes reasonable middleman selection

## Technical Implementation

### Contract Structure
```cpp
struct JobContract {
    // ... other fields ...
    
    // Dispute resolution (only populated when needed)
    DisputeResolutionConfig dispute_config;  // How to select middleman
    MiddlemanInfo middleman_info;            // Empty until dispute
    bool dispute_raised;
    CPubKey dispute_initiator;
    std::vector<CPubKey> proposed_middlemen;
    bool middleman_agreed;
};
```

### Taproot Script Paths
```cpp
struct TaprootPaths {
    CScript cooperative_path;     // Employer + Worker (2-of-2) - MAIN PATH
    CScript worker_timeout_path;  // Worker-only after 24h - TIMEOUT PATH
    CScript employer_dispute;     // Employer + Middleman - DISPUTE PATH
    CScript worker_dispute;       // Worker + Middleman - DISPUTE PATH
    CScript middleman_split;      // Middleman decides split - DISPUTE PATH
    CScript emergency_path;       // Middleman emergency - DISPUTE PATH
};
```

## Workflow Examples

### Example 1: Happy Path (No Dispute)
```
1. Employer: CreateJobContract(employer_key, metadata)
2. Employer: LockFunds() → 2-of-2 escrow created
3. Worker: ApplyForJob()
4. Employer: AssignWorker()
5. Worker: AcceptJob()
6. Worker: SubmitWork()
7. [24 hours pass with no dispute]
8. Worker: WorkerClaimTimeout() → Worker gets paid automatically
```

### Example 2: Dispute with Mutual Agreement
```
1-6. [Same as above]
7. Employer: RaiseDispute(employer_key, "Work incomplete", proposed_middleman_key)
8. Worker: ProposeMiddleman(worker_key, alternative_middleman_key)
9. Employer: AcceptMiddleman(employer_key, alternative_middleman_key)
10. Middleman: MiddlemanResolveDispute(resolution_path, reasoning)
11. Funds distributed according to resolution
```

### Example 3: Pre-Approved Pool
```
1. Employer: CreateJobContract(employer_key, metadata)
2. Employer: SetDisputeResolutionConfig(RANDOM_FROM_POOL, [mid1, mid2, mid3])
3. Worker: (sees and agrees to dispute config when applying)
4-6. [Normal workflow]
7. Worker: RaiseDispute(worker_key, "Not paid fairly", null) 
8. System: AutoSelectMiddleman(RANDOM_FROM_POOL) → Selects mid2
9. Middleman: MiddlemanResolveDispute()
```

## Advantages of This System

### 1. **No Upfront Middleman Costs**
- Most jobs complete without disputes (90%+)
- No need to pay middleman fees for successful jobs
- Middleman only involved when actually needed

### 2. **Flexible Middleman Selection**
- Parties can choose method that works for their situation
- Can adapt to different trust levels and job types
- Multiple fallback mechanisms

### 3. **Incentive Alignment**
- 24-hour timeout incentivizes quick dispute resolution
- Penalties discourage frivolous disputes or unreasonable middleman rejections
- Reputation system ensures quality middlemen

### 4. **Scalability**
- System can handle thousands of jobs without requiring thousands of middlemen
- Middlemen only activated when needed
- Efficient use of arbitration resources

### 5. **Trust Minimization**
- No need to trust middleman until dispute actually occurs
- Multiple selection methods reduce single points of failure
- Cryptographic enforcement of agreements

## Security Considerations

### 1. **Middleman Availability**
- System maintains pool of active, rated middlemen
- Automatic fallbacks if selected middleman unavailable
- Timeout mechanisms prevent indefinite delays

### 2. **Collusion Prevention**
- Random selection from pools prevents gaming
- Reputation system tracks middleman behavior
- Multiple dispute paths prevent single point of control

### 3. **Economic Attacks**
- Selection penalties discourage bad faith rejections
- Middleman fees create economic incentive for fair resolution
- Timeout mechanisms prevent holding funds hostage

## Configuration Examples

### For High-Trust Relationships
```cpp
config.selection_method = MUTUAL_AGREEMENT;
config.selection_timeout_hours = 72; // More time to negotiate
```

### For Quick Resolution
```cpp
config.selection_method = REPUTATION_BASED;
config.selection_timeout_hours = 12; // Fast auto-selection
```

### For Large Jobs
```cpp
config.selection_method = RANDOM_FROM_POOL;
config.approved_middlemen_pool = {expert1, expert2, expert3}; // Pre-vetted experts
config.selection_penalty_sats = 50000; // Higher penalty for large jobs
```

This system provides the flexibility and trust-minimization you were looking for while maintaining strong dispute resolution capabilities!