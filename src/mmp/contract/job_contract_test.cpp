// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <mmp/contract/job_contract.h>
#include <mmp/taproot/taproot.h>
#include <key.h>
#include <pubkey.h>
#include <script/script.h>
#include <chainparams.h>
#include <util/strencodings.h>
#include <util/chaintype.h>
#include <util/translation.h>
#include <iostream>

// Define the translation function
#ifdef DEFINE_G_TRANSLATION_FUN_AS_NULLPTR
const TranslateFn G_TRANSLATION_FUN{nullptr};
#endif

// Initialize ECC context
static ECC_Context ecc_context;

int main(int argc, char* argv[])
{
    SelectParams(ChainType::MAIN);
    
    // Get the chain parameters for mainnet
    const CChainParams& params = Params();
    
    std::cout << "Advanced Job Contract System Test" << std::endl;
    std::cout << "===============================" << std::endl << std::endl;
    
    // Generate keys for testing
    std::pair<CKey, std::string> employer_pair = mmp::GenerateRandomTaprootAddress(params);
    std::pair<CKey, std::string> worker_pair = mmp::GenerateRandomTaprootAddress(params);
    
    CKey employer_key = employer_pair.first;
    CKey worker_key = worker_pair.first;
    
    CPubKey employer_pubkey = employer_key.GetPubKey();
    CPubKey worker_pubkey = worker_key.GetPubKey();
    
    // Get the default middleman
    mmp::Middleman middleman = mmp::GetDefaultMiddleman();
    
    // Create a job contract
    uint32_t timestamp = static_cast<uint32_t>(time(nullptr));
    uint32_t current_height = 800000; // Example current height
    std::string title = "Advanced Job Contract";
    std::string description = "This is a test of the advanced job contract system";
    uint32_t timeout_blocks = 144; // 1 day (approximately)
    CAmount amount = 100000000; // 1 GOTHAM
    
    uint256 job_id = mmp::GenerateJobId(employer_pubkey, title, timestamp);
    
    // Create the job contract with the new constructor
    mmp::JobContract job(job_id, title, description, current_height, timestamp, timeout_blocks, amount, employer_pubkey, middleman);
    
    // Print job details
    std::cout << "Job ID: " << job_id.GetHex() << std::endl;
    std::cout << "Title: " << job.metadata.title << std::endl;
    std::cout << "Description: " << job.metadata.description << std::endl;
    std::cout << "Created at height: " << job.metadata.created_height << std::endl;
    std::cout << "Timeout blocks: " << job.metadata.timeout_blocks << std::endl;
    std::cout << "Amount: " << job.metadata.amount << " satoshis" << std::endl;
    std::cout << "Employer pubkey: " << HexStr(job.keys.employer_key) << std::endl;
    std::cout << "Middleman ID: " << job.middleman_info.id.GetHex() << std::endl;
    std::cout << "Middleman Name: " << job.middleman_info.name << std::endl;
    std::cout << "Middleman Pubkey: " << HexStr(job.middleman_info.pubkey) << std::endl;
    std::cout << "State: " << mmp::JobStateToString(job.state) << std::endl;
    std::cout << std::endl;
    
    // Test state transitions
    std::cout << "Testing state transitions:" << std::endl;
    
    // Open the job
    auto result = job.TransitionState(mmp::JobState::OPEN, uint256(), "Job opened for applications");
    std::cout << "Transition to OPEN: " << (result == mmp::StateTransitionResult::SUCCESS ? "Success" : "Failed") << std::endl;
    std::cout << "Current state: " << mmp::JobStateToString(job.state) << std::endl;
    
    // Add a worker application
    mmp::WorkerApplication application;
    application.worker_pubkey = worker_pubkey;
    application.proposal = "I can complete this job efficiently";
    application.timestamp = timestamp;
    application.status = mmp::ApplicationStatus::PENDING;
    job.worker_applications.push_back(application);
    
    std::cout << "Worker application received from: " << HexStr(worker_pubkey) << std::endl;
    std::cout << "Number of applications: " << job.worker_applications.size() << std::endl;
    
    // Assign the worker
    job.assigned_worker = worker_pubkey;
    result = job.TransitionState(mmp::JobState::ASSIGNED, uint256(), "Worker assigned");
    std::cout << "Transition to ASSIGNED: " << (result == mmp::StateTransitionResult::SUCCESS ? "Success" : "Failed") << std::endl;
    std::cout << "Current state: " << mmp::JobStateToString(job.state) << std::endl;
    
    // Start work
    result = job.TransitionState(mmp::JobState::IN_PROGRESS, uint256(), "Work started");
    std::cout << "Transition to IN_PROGRESS: " << (result == mmp::StateTransitionResult::SUCCESS ? "Success" : "Failed") << std::endl;
    std::cout << "Current state: " << mmp::JobStateToString(job.state) << std::endl;
    
    // Test key rotation
    std::cout << std::endl << "Testing key rotation:" << std::endl;
    auto rotation_result = job.RotateKeys(timestamp + 3000000); // Simulate time passing
    std::cout << "Key rotation result: " << static_cast<int>(rotation_result) << std::endl;
    std::cout << "Keys rotated: " << job.keys.keys_rotated << std::endl;
    std::cout << "Rotation count: " << job.keys.rotation_count << std::endl;
    
    // Complete the job
    result = job.TransitionState(mmp::JobState::COMPLETED, uint256(), "Work completed");
    std::cout << "Transition to COMPLETED: " << (result == mmp::StateTransitionResult::SUCCESS ? "Success" : "Failed") << std::endl;
    std::cout << "Current state: " << mmp::JobStateToString(job.state) << std::endl;
    std::cout << "Completion timestamp: " << job.completion_timestamp << std::endl;
    
    // Test dispute
    std::cout << std::endl << "Testing dispute resolution:" << std::endl;
    job.dispute_initiator = employer_pubkey;
    job.dispute_reason = "Work quality does not meet requirements";
    result = job.TransitionState(mmp::JobState::DISPUTED, uint256(), "Dispute raised");
    std::cout << "Transition to DISPUTED: " << (result == mmp::StateTransitionResult::SUCCESS ? "Success" : "Failed") << std::endl;
    std::cout << "Current state: " << mmp::JobStateToString(job.state) << std::endl;
    std::cout << "Dispute raised: " << job.dispute_raised << std::endl;
    std::cout << "Dispute timestamp: " << job.dispute_timestamp << std::endl;
    std::cout << "Dispute reason: " << job.dispute_reason << std::endl;
    
    // Resolve the dispute
    job.resolution_path = mmp::ResolutionPath::MIDDLEMAN_RESOLUTION;
    result = job.TransitionState(mmp::JobState::RESOLVED, uint256(), "Dispute resolved by middleman");
    std::cout << "Transition to RESOLVED: " << (result == mmp::StateTransitionResult::SUCCESS ? "Success" : "Failed") << std::endl;
    std::cout << "Current state: " << mmp::JobStateToString(job.state) << std::endl;
    std::cout << "Resolution path: " << static_cast<int>(job.resolution_path) << std::endl;
    
    // Test event history
    std::cout << std::endl << "Event history:" << std::endl;
    for (const auto& event : job.event_history) {
        std::cout << "- " << event.timestamp << ": " 
                 << mmp::JobStateToString(event.previous_state) << " -> " 
                 << mmp::JobStateToString(event.state) << " - " 
                 << event.memo << std::endl;
    }
    
    // Test validation
    std::cout << std::endl << "Validation tests:" << std::endl;
    std::cout << "Is valid: " << job.IsValid() << std::endl;
    auto [valid, error] = job.ValidateEx();
    std::cout << "Extended validation: " << valid << " (Error code: " << static_cast<int>(error) << ")" << std::endl;
    
    // Test legacy helper methods
    std::cout << std::endl << "Legacy helper method tests:" << std::endl;
    std::cout << "IsAccepted: " << job.IsAccepted() << std::endl;
    std::cout << "IsSelected: " << job.IsSelected() << std::endl;
    std::cout << "IsCompleted: " << job.IsCompleted() << std::endl;
    std::cout << "IsConfirmed: " << job.IsConfirmed() << std::endl;
    std::cout << "IsRefunded: " << job.IsRefunded() << std::endl;
    std::cout << "IsActive: " << job.IsActive() << std::endl;
    
    // Test expiration
    std::cout << std::endl << "Expiration tests:" << std::endl;
    std::cout << "Is expired (current height): " << job.IsExpired(current_height) << std::endl;
    std::cout << "Is expired (future height): " << job.IsExpired(current_height + timeout_blocks + 1) << std::endl;
    std::cout << "Is near expiration: " << job.IsNearExpiration(current_height + timeout_blocks - 10) << std::endl;
    
    // Print job summary
    std::cout << std::endl << "Job summary:" << std::endl;
    std::cout << job.ToString() << std::endl;
    
    return 0;
}