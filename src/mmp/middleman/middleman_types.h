// Copyright (c) 2023-present The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_TYPES_H
#define GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_TYPES_H

#include <string>
#include <vector>
#include <cstdint>
#include <serialize.h>
#include <uint256.h>

namespace mmp {

/**
 * Represents a specialty area for a middleman
 */
struct Specialty {
    std::string category;       // Main category (e.g., "SOFTWARE")
    std::string subcategory;    // Subcategory (e.g., "BLOCKCHAIN")
    uint32_t experience_level;  // 1-5 (where 5 is expert)
    uint32_t jobs_completed;    // Number of jobs completed in this specialty
    
    SERIALIZE_METHODS(Specialty, obj) {
        READWRITE(obj.category);
        READWRITE(obj.subcategory);
        READWRITE(obj.experience_level);
        READWRITE(obj.jobs_completed);
    }
};

/**
 * Record of a bond slash event
 */
struct SlashRecord {
    uint256 dispute_id;         // ID of the dispute that led to the slash
    int64_t timestamp;          // When the slash occurred
    uint64_t amount_slashed;    // Amount slashed from bond
    std::string reason;         // Reason for the slash
    
    SERIALIZE_METHODS(SlashRecord, obj) {
        READWRITE(obj.dispute_id);
        READWRITE(obj.timestamp);
        READWRITE(obj.amount_slashed);
        READWRITE(obj.reason);
    }
};

/**
 * Detailed performance metrics for a middleman
 */
struct MiddlemanPerformance {
    uint32_t avg_response_time_blocks{0};   // Average response time in blocks
    uint32_t avg_resolution_time_blocks{0}; // Average resolution time in blocks
    double employer_satisfaction{0.0};      // 0.0-1.0 employer satisfaction score
    double worker_satisfaction{0.0};        // 0.0-1.0 worker satisfaction score
    uint32_t disputes_resolved{0};          // Total disputes resolved
    uint32_t disputes_appealed{0};          // Disputes that were appealed
    int64_t last_updated_timestamp{0};      // When metrics were last updated
    
    SERIALIZE_METHODS(MiddlemanPerformance, obj) {
        READWRITE(obj.avg_response_time_blocks);
        READWRITE(obj.avg_resolution_time_blocks);
        READWRITE(obj.employer_satisfaction);
        READWRITE(obj.worker_satisfaction);
        READWRITE(obj.disputes_resolved);
        READWRITE(obj.disputes_appealed);
        READWRITE(obj.last_updated_timestamp);
    }
    
    /**
     * Check if performance data is fresh enough
     */
    bool IsPerformanceDataFresh(int64_t current_timestamp, int64_t max_age_seconds) const {
        return (current_timestamp - last_updated_timestamp) <= max_age_seconds;
    }
};

/**
 * Bond insurance for high-value jobs
 */
struct BondInsurance {
    bool has_insurance{false};          // Whether insurance is active
    uint64_t coverage_amount_sats{0};   // Coverage amount
    std::string provider;               // Insurance provider
    int64_t expiry_timestamp{0};        // When insurance expires
    
    SERIALIZE_METHODS(BondInsurance, obj) {
        READWRITE(obj.has_insurance);
        READWRITE(obj.coverage_amount_sats);
        READWRITE(obj.provider);
        READWRITE(obj.expiry_timestamp);
    }
    
    /**
     * Check if insurance is valid
     */
    bool IsValid() const {
        if (!has_insurance) return true; // No insurance is valid
        
        // If has insurance, check that it has coverage and hasn't expired
        return coverage_amount_sats > 0 && !provider.empty() && expiry_timestamp > 0;
    }
};

} // namespace mmp

#endif // GOTHAM_MMP_MIDDLEMAN_MIDDLEMAN_TYPES_H