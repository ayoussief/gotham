// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_H
#define GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_H

#include <pubkey.h>
#include <serialize.h>
#include <uint256.h>
#include <chainparams.h>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <mmp/middleman/middleman_types.h>

namespace mmp {

// Constants for validation
constexpr uint64_t MMP_MIN_MIDDLEMAN_BOND_SATS = 1000000;  // 0.01 BTC
constexpr uint64_t MMP_MAX_MIDDLEMAN_BOND_SATS = 1000000000;  // 10 BTC
constexpr uint32_t MMP_MAX_REPUTATION_SCORE = 100;
constexpr size_t MMP_MAX_MIDDLEMAN_NAME_LENGTH = 50;
constexpr size_t MMP_MAX_MIDDLEMAN_DESCRIPTION_LENGTH = 500;
constexpr size_t MMP_MAX_SPECIALTIES_PER_USER = 10;

// Thread safety annotation
#define MMP_THREAD_SAFE

/**
 * Represents a registered middleman who can resolve disputes between employer and worker.
 */
class Middleman {
public:
    uint256 id;                             // Unique identifier
    CPubKey pubkey;                         // Middleman's public key
    std::string name;                       // Human-readable name (renamed from middleman_name)
    std::string contact_info;               // Contact information
    std::string description;                // Description of services
    uint64_t fee_sats{0};                   // Middleman fee in satoshis
    uint32_t trust_score{0};                // 0-100 (where 100 is most trusted) (renamed from reputation_score)
    uint64_t bond_amount_sats{0};           // Security deposit (slashed for bad decisions)
    std::string identity_hash;              // Obfuscated identity (e.g., SHA256("Arbitrator LLC"))
    uint32_t total_disputes{0};             // Total disputes handled
    uint32_t successful_resolutions{0};     // Successfully resolved disputes
    uint32_t bond_slashes{0};               // Number of times bond was slashed (reputation impact)
    std::vector<std::string> specialties;   // e.g., ["SOFTWARE", "DESIGN", "WRITING"] (deprecated)
    std::vector<Specialty> detailed_specialties; // Enhanced specialty tracking
    int64_t last_active_timestamp{0};       // Last activity timestamp
    bool is_active{false};                  // Whether middleman is currently active
    bool accepts_new_jobs{false};           // Whether accepting new arbitration jobs
    uint32_t max_job_amount_sats{0};        // Maximum job amount they'll handle
    uint32_t response_time_blocks{144};     // Typical response time in blocks (default 24h)
    bool is_dao_approved{false};            // Whether approved by DAO/governance
    std::vector<SlashRecord> slash_history; // History of bond slashes for transparency
    bool is_kyc_verified{false};            // Whether KYC verified (for fallback arbitrators)
    std::vector<uint8_t> zk_attestation;    // Zero-knowledge attestation of identity/credentials
    std::vector<uint8_t> zk_kyc_proof;      // Zero-knowledge proof of KYC verification
    uint256 credential_merkle_root;         // Merkle root of credential set
    
    // Activity tracking
    uint32_t active_disputes_count{0};      // Number of currently active disputes
    int64_t last_activity_timestamp{0};     // Last activity timestamp
    uint32_t max_concurrent_jobs{5};        // Maximum number of concurrent jobs
    
    // Performance metrics (precomputed)
    double credibility_score{0.0};          // Precomputed credibility score
    
    // Enhanced reputation decay parameters (configurable per middleman)
    double reputation_decay_half_life_days{180.0}; // 6 months default half-life
    double min_reputation_retention{0.5};   // Minimum 50% reputation retained
    bool use_custom_decay{false};           // Whether to use custom decay parameters
    double custom_decay_rate{0.0038};       // Default ~0.38% daily decay rate
    
    // Performance metrics
    MiddlemanPerformance performance_metrics; // Detailed performance tracking
    
    // Bond insurance (optional for high-value jobs)
    BondInsurance bond_insurance;           // Insurance coverage for bond

    // Default constructor with safe defaults
    Middleman() : trust_score(0) {}

    // Constructor with required fields
    Middleman(const std::string& name, const CPubKey& pubkey)
        : name(name), pubkey(pubkey), trust_score(50) {
        id = GenerateMiddlemanId(name, pubkey);
        is_active = true;
        accepts_new_jobs = true;
    }

    // Constructor with additional fields
    Middleman(const std::string& name, const CPubKey& pubkey, uint64_t fee, uint64_t bond)
        : name(name), pubkey(pubkey), fee_sats(fee), bond_amount_sats(bond), trust_score(50) {
        id = GenerateMiddlemanId(name, pubkey);
        is_active = true;
        accepts_new_jobs = true;
    }

    // Serialization
    SERIALIZE_METHODS(Middleman, obj) {
        READWRITE(obj.id);
        READWRITE(obj.name);
        READWRITE(obj.pubkey);
        READWRITE(obj.trust_score);
        READWRITE(obj.contact_info);
        READWRITE(obj.description);
        READWRITE(obj.fee_sats);
        READWRITE(obj.bond_amount_sats);
        READWRITE(obj.identity_hash);
        READWRITE(obj.total_disputes);
        READWRITE(obj.successful_resolutions);
        READWRITE(obj.bond_slashes);
        READWRITE(obj.specialties);
        READWRITE(obj.last_active_timestamp);
        READWRITE(obj.is_active);
        READWRITE(obj.accepts_new_jobs);
        READWRITE(obj.max_job_amount_sats);
        READWRITE(obj.response_time_blocks);
        READWRITE(obj.is_dao_approved);
        READWRITE(obj.is_kyc_verified);
        READWRITE(obj.active_disputes_count);
        READWRITE(obj.last_activity_timestamp);
        READWRITE(obj.max_concurrent_jobs);
        READWRITE(obj.credibility_score);
        READWRITE(obj.reputation_decay_half_life_days);
        READWRITE(obj.min_reputation_retention);
        READWRITE(obj.use_custom_decay);
        READWRITE(obj.custom_decay_rate);
        
        // Serialize complex structures
        READWRITE(obj.detailed_specialties);
        READWRITE(obj.slash_history);
        READWRITE(obj.zk_attestation);
        READWRITE(obj.zk_kyc_proof);
        READWRITE(obj.credential_merkle_root);
        
        // Serialize performance metrics
        READWRITE(obj.performance_metrics);
        
        // Serialize bond insurance
        READWRITE(obj.bond_insurance);
    }

    std::string GetAddress(const CChainParams& params) const;

    // Comprehensive validation with all security checks
    bool IsValid() const {
        // Basic validation
        if (bond_amount_sats < MMP_MIN_MIDDLEMAN_BOND_SATS || 
            bond_amount_sats > MMP_MAX_MIDDLEMAN_BOND_SATS ||
            trust_score > MMP_MAX_REPUTATION_SCORE ||
            name.empty() ||
            name.length() > MMP_MAX_MIDDLEMAN_NAME_LENGTH ||
            description.length() > MMP_MAX_MIDDLEMAN_DESCRIPTION_LENGTH ||
            specialties.size() > MMP_MAX_SPECIALTIES_PER_USER ||
            fee_sats == 0 ||
            fee_sats > bond_amount_sats) {  // Fee shouldn't exceed bond (economic security)
            return false;
        }
        
        return true;
    }

    /**
     * @brief Calculate effective reputation considering bond slashes
     * @return Effective reputation score after penalties
     */
    uint32_t GetEffectiveReputation() const {
        if (total_disputes == 0) return trust_score;
        
        // Safe division with bounds checking
        uint32_t penalty = total_disputes > 0 ? 
            (bond_slashes * 100) / total_disputes : 0; // Penalty per slash
            
        // Cap penalty at reputation score
        penalty = std::min(penalty, trust_score);
        
        return trust_score - penalty;
    }
    
    // Optimized time decay factor calculation (simplified exponential decay)
    double CalculateTimeDecayFactor(int64_t time_since_active) const {
        // Convert to days for more intuitive decay rates
        const double days_since_active = static_cast<double>(time_since_active) / 86400.0;
        
        // Use custom decay rate if specified, otherwise use default
        const double decay_rate = use_custom_decay ? custom_decay_rate : 0.0038; // ~0.38% daily decay
        
        // Simple exponential decay with minimum retention floor
        return std::max(min_reputation_retention, std::exp(-decay_rate * days_since_active));
    }

private:
    static uint256 GenerateMiddlemanId(const std::string& name, const CPubKey& pubkey);
};

/**
 * Get the default middleman for testing purposes
 */
Middleman GetDefaultMiddleman();

} // namespace mmp

#endif // GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_H