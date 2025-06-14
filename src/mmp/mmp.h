
#ifndef GOTHAM_MMP_H
#define GOTHAM_MMP_H

#include <uint256.h>
#include <script/script.h>
#include <primitives/transaction.h>
#include <pubkey.h>
#include <key.h>
#include <span.h>
#include <addresstype.h>
#include <optional>

// Thread safety annotations
#ifdef __GNUC__
#define MMP_THREAD_SAFE __attribute__((const))
#define MMP_REQUIRES_LOCK __attribute__((lock_required))
#else
#define MMP_THREAD_SAFE
#define MMP_REQUIRES_LOCK
#endif

/**
 * @namespace mmp
 * @brief Middleman Protocol - Trustless Bitcoin escrow system
 * 
 * @security Important Security Considerations:
 * - All cryptographic operations use libsecp256k1
 * - Replay protection enforced via nonces+timestamps
 * - Strict input validation on all parameters
 * - Thread-safe design with clear locking requirements
 * 
 * @performance Critical Path Optimizations:
 * - Schnorr aggregation for cooperative paths
 * - Taproot for single-key-spend appearance
 * - MAST for efficient dispute resolution
 */
namespace mmp {

// Protocol constants (all with MMP_ prefix for consistency)
static constexpr char MMP_PROTOCOL_TAG[] = "MMP";
static constexpr uint8_t MMP_PROTOCOL_VERSION = 1;
static constexpr unsigned int MMP_OP_RETURN_LIMIT = 80;
static constexpr unsigned int MMP_DEFAULT_TIMEOUT_BLOCKS = 144; // ~24 hours
static constexpr unsigned int MMP_DEFAULT_DISPUTE_TIMEOUT_BLOCKS = 1008; // ~1 week
static const unsigned int MMP_MIN_CONFIRMATION_BLOCKS = 6;

// Validation constraints (constexpr for compile-time evaluation)
static constexpr size_t MMP_MAX_METADATA_URL_LENGTH = 2048;     // Max URL length
static constexpr size_t MMP_MAX_METADATA_HASH_LENGTH = 64;      // SHA256 hex string
static constexpr uint64_t MMP_MIN_JOB_AMOUNT_SATS = 1000;       // Minimum job amount (dust limit)
static constexpr uint64_t MMP_MAX_JOB_AMOUNT_SATS = 2100000000000000ULL; // 21M BTC in sats
static constexpr uint32_t MMP_MIN_TIMEOUT_BLOCKS = 6;           // Minimum timeout
static constexpr uint32_t MMP_MAX_TIMEOUT_BLOCKS = 52560;       // ~1 year maximum

// Application and messaging constraints
static constexpr size_t MMP_MAX_APPLICATION_MESSAGE_LENGTH = 1024;  // Max application message length
static constexpr size_t MMP_MAX_CONTACT_INFO_LENGTH = 256;          // Max contact info length
static constexpr size_t MMP_MAX_JOB_MESSAGE_LENGTH = 2048;          // Max job message length
static constexpr size_t MMP_MAX_CANCEL_REASON_LENGTH = 512;         // Max cancellation reason length
static constexpr uint32_t MMP_MAX_APPLICATIONS_PER_JOB = 100;       // Max applications per job
static constexpr uint32_t MMP_MAX_NOTIFICATIONS_PER_USER = 1000;    // Max notifications per user
static constexpr uint32_t MMP_MAX_TIMEOUT_EXTENSION_BLOCKS = 8640;  // Max extension ~60 days

// Rating system constraints
static constexpr uint8_t MMP_MIN_RATING = 1;                       // Minimum rating (1 star)
static constexpr uint8_t MMP_MAX_RATING = 5;                       // Maximum rating (5 stars)
static constexpr size_t MMP_MAX_REVIEW_TEXT_LENGTH = 1024;         // Max review text length
static constexpr uint32_t MMP_MAX_RATINGS_PER_USER = 10000;        // Max ratings per user

// Middleman and two-phase escrow constraints
static constexpr size_t MMP_MAX_MIDDLEMAN_NAME_LENGTH = 128;       // Max middleman name length
static constexpr size_t MMP_MAX_MIDDLEMAN_DESCRIPTION_LENGTH = 512; // Max middleman description length
static constexpr size_t MMP_MAX_SPECIALTY_LENGTH = 64;             // Max specialty string length
static constexpr uint32_t MMP_MAX_SPECIALTIES_PER_USER = 10;       // Max specialties per user
static constexpr uint32_t MMP_DISPUTE_TIMEOUT_HOURS = 24;          // 24 hour dispute window
static constexpr uint32_t MMP_DISPUTE_TIMEOUT_BLOCKS = 144;        // ~24 hours in blocks (10min avg)
static constexpr size_t MMP_MAX_DISPUTE_REASON_LENGTH = 1024;      // Max dispute reason length

// Security and anti-spam constraints
static constexpr uint64_t MMP_MIN_SELECTION_PENALTY_SATS = 1000;   // Minimum penalty to prevent spam
static constexpr uint64_t MMP_MAX_SELECTION_PENALTY_SATS = 1000000; // Maximum reasonable penalty
static constexpr uint64_t MMP_MIN_MIDDLEMAN_BOND_SATS = 50000;     // Minimum bond for middlemen
static constexpr uint64_t MMP_MAX_MIDDLEMAN_BOND_SATS = 100000000; // Maximum bond (1 BTC)
static constexpr uint32_t MMP_MIN_SELECTION_TIMEOUT_BLOCKS = 6;    // Minimum 1 hour
static constexpr uint32_t MMP_MAX_SELECTION_TIMEOUT_BLOCKS = 1008; // Maximum 1 week
static constexpr uint32_t MMP_MAX_REPUTATION_SCORE = 100;          // Maximum reputation score
static constexpr uint32_t MMP_MIN_REPUTATION_FOR_AUTO = 50;        // Minimum reputation for auto-selection
static constexpr uint32_t MMP_MAX_APPROVED_MIDDLEMEN = 50;         // Maximum in approved pool
static constexpr size_t MMP_MAX_IDENTITY_HASH_LENGTH = 64;         // Max identity hash length

// Enhanced constants for production use
static constexpr double MMP_MAX_FEE_TO_BOND_RATIO = 0.5;           // Fee can be max 50% of bond
static constexpr double MMP_MIN_BOND_TO_JOB_RATIO = 0.05;          // Bond must be min 5% of job amount
static constexpr uint32_t MMP_MIN_DAO_APPROVERS = 3;               // Minimum DAO approvers for slashing
static constexpr uint32_t MMP_MAX_DAO_APPROVERS = 10;              // Maximum DAO approvers
static constexpr uint32_t MMP_MIN_INSURANCE_APPROVERS = 2;         // Minimum insurance claim approvers
static constexpr uint32_t MMP_MAX_INSURANCE_APPROVERS = 5;         // Maximum insurance claim approvers

// Bond slashing and appeal constraints
static constexpr uint32_t MMP_MIN_CHALLENGE_PERIOD_BLOCKS = 144;   // Minimum 24h for appeals
static constexpr uint32_t MMP_MAX_CHALLENGE_PERIOD_BLOCKS = 4032;  // Maximum 4 weeks for appeals
static constexpr uint32_t MMP_MIN_DAO_APPROVERS = 3;               // Minimum DAO approvers for slashing
static constexpr uint32_t MMP_MAX_DAO_APPROVERS = 15;              // Maximum DAO approvers
static constexpr double MMP_MAX_FEE_TO_BOND_RATIO = 0.5;           // Fee can't exceed 50% of bond
static constexpr double MMP_MIN_BOND_TO_JOB_RATIO = 0.05;          // Bond must be at least 5% of max job
static constexpr double MMP_REPUTATION_TIME_DECAY_YEARS = 1.0;     // Reputation decay over 1 year

// Version compatibility constants
static constexpr uint8_t MMP_MIN_COMPATIBLE_VERSION = 1;    // Minimum supported version
static constexpr uint8_t MMP_MAX_COMPATIBLE_VERSION = 1;    // Maximum supported version

// Future-proofing constants
static constexpr uint8_t MMP_RESERVED_FLAGS = 0x0F;         // For future feature flags

// Exception safety and thread safety annotations
#define MMP_NOEXCEPT noexcept
#define MMP_STRONG_EXCEPTION_SAFETY [[nodiscard]]
#define MMP_THREAD_SAFE [[nodiscard]]
#define MMP_REQUIRES_LOCK [[requires: lock_held]]

// Bitcoin protocol limits
static constexpr size_t MMP_MAX_WITNESS_ELEMENT_SIZE = 520;  // Bitcoin witness element limit
static constexpr size_t MMP_MIN_CONTROL_BLOCK_SIZE = 33;     // 1 byte + 32 byte key
static constexpr size_t MMP_CONTROL_BLOCK_HASH_SIZE = 32;    // Each additional hash is 32 bytes
static constexpr size_t MMP_MAX_WITNESS_STACK_SIZE = 100;    // Reasonable limit for witness stack
static constexpr size_t MMP_MAX_BUFFER_SIZE = 1024 * 1024;   // 1MB max buffer size

// Debug and testing constants
#ifdef MMP_DEBUG
static constexpr bool MMP_STRICT_VALIDATION = true;
static constexpr size_t MMP_MAX_EVENT_HISTORY = 1000;       // Limit event history in debug mode
static constexpr size_t MMP_FUZZ_MAX_ITERATIONS = 10000;    // Max fuzz test iterations
#endif

// Static assertions for compile-time safety checks
static_assert(MMP_MAX_METADATA_URL_LENGTH <= 2048, "URL length exceeds safe limits");
static_assert(MMP_MAX_JOB_AMOUNT_SATS <= 2100000000000000ULL, "Amount exceeds Bitcoin supply");
static_assert(MMP_MAX_WITNESS_ELEMENT_SIZE == 520, "Must match Bitcoin consensus");
static_assert(MMP_MIN_CONTROL_BLOCK_SIZE >= 33, "Control block too small for Taproot");
static_assert(MMP_CONTROL_BLOCK_HASH_SIZE == 32, "Hash size must be 32 bytes");
static_assert(MMP_MIN_COMPATIBLE_VERSION <= MMP_MAX_COMPATIBLE_VERSION, "Invalid version range");
static_assert(MMP_MIN_TIMEOUT_BLOCKS >= 1, "Timeout must be at least 1 block");
static_assert(MMP_MAX_TIMEOUT_BLOCKS <= 52560, "Timeout exceeds reasonable limit");
static_assert(MMP_MAX_APPLICATION_MESSAGE_LENGTH <= 1024, "Application message too long");
static_assert(MMP_MAX_CONTACT_INFO_LENGTH <= 256, "Contact info too long");
static_assert(MMP_MAX_JOB_MESSAGE_LENGTH <= 2048, "Job message too long");
static_assert(MMP_MAX_APPLICATIONS_PER_JOB <= 1000, "Too many applications per job");
static_assert(MMP_MAX_TIMEOUT_EXTENSION_BLOCKS <= MMP_MAX_TIMEOUT_BLOCKS, "Extension exceeds max timeout");
static_assert(MMP_MIN_RATING >= 1 && MMP_MIN_RATING <= 5, "Invalid minimum rating");
static_assert(MMP_MAX_RATING >= 1 && MMP_MAX_RATING <= 5, "Invalid maximum rating");
static_assert(MMP_MIN_RATING <= MMP_MAX_RATING, "Invalid rating range");
static_assert(MMP_MAX_REVIEW_TEXT_LENGTH <= 2048, "Review text too long");
static_assert(MMP_MAX_MIDDLEMAN_NAME_LENGTH <= 256, "Middleman name too long");
static_assert(MMP_MAX_MIDDLEMAN_DESCRIPTION_LENGTH <= 1024, "Middleman description too long");
static_assert(MMP_MAX_SPECIALTY_LENGTH <= 128, "Specialty string too long");
static_assert(MMP_DISPUTE_TIMEOUT_HOURS == 24, "Dispute timeout must be 24 hours");
static_assert(MMP_DISPUTE_TIMEOUT_BLOCKS >= 144, "Dispute timeout blocks too small");
static_assert(MMP_MAX_DISPUTE_REASON_LENGTH <= 2048, "Dispute reason too long");
static_assert(MMP_MIN_SELECTION_PENALTY_SATS > 0, "Selection penalty must be positive");
static_assert(MMP_MIN_MIDDLEMAN_BOND_SATS >= 10000, "Minimum bond too small");
static_assert(MMP_MAX_REPUTATION_SCORE == 100, "Reputation score must be 0-100");
static_assert(MMP_MIN_REPUTATION_FOR_AUTO <= MMP_MAX_REPUTATION_SCORE, "Invalid reputation range");
static_assert(MMP_MIN_SELECTION_TIMEOUT_BLOCKS >= 6, "Selection timeout too small");
static_assert(MMP_MAX_SELECTION_TIMEOUT_BLOCKS <= 1008, "Selection timeout too large");

// Additional compile-time checks for enum sizes and ranges
static_assert(static_cast<uint8_t>(JobState::EXPIRED) < 16, "JobState values exceed 4-bit range");
static_assert(static_cast<uint8_t>(ResolutionPath::EMERGENCY) < 8, "ResolutionPath values exceed 3-bit range");
static_assert(static_cast<uint8_t>(MiddlemanSelectionMethod::FALLBACK_ARBITRATOR) < 8, "MiddlemanSelectionMethod values exceed 3-bit range");

// Struct size and alignment checks (for memory efficiency)
static_assert(sizeof(JobState) == 1, "JobState should be 1 byte");
static_assert(sizeof(ResolutionPath) == 1, "ResolutionPath should be 1 byte");
static_assert(sizeof(MiddlemanSelectionMethod) == 1, "MiddlemanSelectionMethod should be 1 byte");

// Ensure proper alignment for performance-critical structures
static_assert(alignof(uint64_t) <= 8, "uint64_t alignment assumption");
static_assert(alignof(CPubKey) <= 8, "CPubKey alignment assumption");

// Job states (explicit values for consistency)
enum class JobState : uint8_t {
    CREATED = 0,        // Job posted, awaiting worker applications
    OPEN = 1,           // Job open for worker applications, funds locked
    ASSIGNED = 2,       // Worker assigned by employer, awaiting acceptance
    IN_PROGRESS = 3,    // Worker accepted, job in progress
    COMPLETED = 4,      // Work submitted, awaiting confirmation
    DISPUTED = 5,       // Dispute raised, middleman intervention
    RESOLVED = 6,       // Successfully completed or resolved
    CANCELLED = 7,      // Cancelled before completion
    EXPIRED = 8         // Timeout expired
};

// Dispute resolution paths (explicit values for consistency)
enum class ResolutionPath : uint8_t {
    COOPERATIVE = 0,        // Employer + Worker agree (2-of-2 multisig)
    WORKER_TIMEOUT = 1,     // Worker claims after 24h timeout (worker-only)
    EMPLOYER_WIN = 2,       // Employer + Middleman (refund after dispute)
    WORKER_WIN = 3,         // Worker + Middleman (payment after dispute)
    MIDDLEMAN_SPLIT = 4,    // Middleman decides split payment
    EMERGENCY = 5           // Emergency resolution by middleman
};

// Error codes for advanced error handling
enum class MMPError {
    NONE = 0,
    INVALID_URL,
    INVALID_HASH,
    INVALID_KEYS,
    INVALID_AMOUNT,
    INVALID_TIMEOUT,
    TIMEOUT_EXPIRED,
    INVALID_CONTRACT,
    AGGREGATION_FAILED,
    TWEAK_INCONSISTENT,
    SERIALIZATION_FAILED,
    DESERIALIZATION_FAILED,
    INVALID_VERSION,
    INCOMPATIBLE_VERSION,
    INVALID_SIGNATURE,
    INSUFFICIENT_FUNDS,
    TRANSACTION_BUILD_FAILED,
    SCRIPT_VALIDATION_FAILED,
    CONTROL_BLOCK_INVALID,
    METADATA_TOO_LARGE,
    NETWORK_ERROR,
    UNKNOWN_ERROR
};

// Specific result enums for granular error handling
enum class LockFundsResult {
    SUCCESS,
    INVALID_CONTRACT_STATE,
    INVALID_VOUT,
    TX_MISMATCH,
    INSUFFICIENT_AMOUNT,
    ALREADY_LOCKED,
    INVALID_SCRIPT
};

enum class JobActionResult {
    SUCCESS,
    INVALID_CONTRACT_STATE,
    INVALID_KEY,
    ALREADY_ACCEPTED,
    WORK_ALREADY_SUBMITTED,
    INVALID_WORK_PROOF,
    DISPUTE_ALREADY_RAISED,
    TIMEOUT_EXPIRED,
    INSUFFICIENT_PERMISSIONS,
    INVALID_PARAMETERS,
    WORKER_ALREADY_APPLIED,
    NO_WORKER_ASSIGNED,
    WORKER_NOT_FOUND,
    JOB_NOT_OPEN_FOR_APPLICATIONS,
    INVALID_APPLICATION,
    CANNOT_CANCEL_JOB,
    APPLICATION_NOT_FOUND,
    TIMEOUT_EXTENSION_FAILED,
    METADATA_UPDATE_FAILED,
    MESSAGE_SEND_FAILED,
    NOTIFICATION_NOT_FOUND,
    MIDDLEMAN_NOT_FOUND,
    MIDDLEMAN_ALREADY_ASSIGNED,
    MIDDLEMAN_REGISTRATION_FAILED,
    MIDDLEMAN_INACTIVE,
    MIDDLEMAN_SELECTION_TIMEOUT,
    MIDDLEMAN_NOT_AGREED,
    INVALID_MIDDLEMAN_SELECTION_METHOD,
    NO_APPROVED_MIDDLEMEN,
    MIDDLEMAN_REJECTED,
    DISPUTE_RESOLUTION_CONFIG_INVALID,
    INSUFFICIENT_MIDDLEMAN_BOND,
    MIDDLEMAN_BOND_SLASHED,
    REPUTATION_TOO_LOW,
    NOT_DAO_APPROVED,
    NOT_KYC_VERIFIED,
    FALLBACK_ARBITRATOR_REQUIRED,
    SELECTION_PENALTY_TOO_LOW,
    IDENTITY_HASH_INVALID,
    FEE_EXCEEDS_BOND,
    BOND_TOO_LOW_FOR_JOB,
    ECONOMIC_RATIOS_INVALID,
    SLASH_PROPOSAL_INVALID,
    APPEAL_INVALID,
    CHALLENGE_PERIOD_ACTIVE,
    INSUFFICIENT_DAO_APPROVERS,
    MIDDLEMAN_NOT_AUTHORIZED,
    EVIDENCE_HASH_REQUIRED,
    RESOLUTION_REASONING_INVALID
};

enum class StateTransitionResult {
    SUCCESS,
    INVALID_CURRENT_STATE,
    INVALID_TARGET_STATE,
    TRANSITION_NOT_ALLOWED,
    MISSING_PREREQUISITES,
    TIMEOUT_EXPIRED
};

// Security audit logging structure
struct SecurityAuditLog {
    int64_t timestamp;
    MMPError error;
    std::string context;
    uint256 related_txid;
    
    std::string ToString() const;
};

/**
 * @struct JobMetadata
 * @brief Contains all metadata for a job posting
 * @var job_id Unique identifier computed from keys, hash, nonce, and timestamp
 * @var metadata_url Must be HTTPS/IPFS URL under 2048 chars
 * @var hash SHA256 of metadata content in hex format (64 chars)
 * @var amount_sats Must be between 1,000 and 21M BTC in satoshis
 * @var timeout_blocks Must be 6-52560 blocks (10min-1yr)
 * @var created_height Block height when job was created
 * @note All validation methods are thread-safe
 */
struct JobMetadata {
    uint256 job_id;
    std::string metadata_url;     // e.g., IPFS or HTTPS
    std::string hash;             // sha256 of the metadata contents
    uint64_t amount_sats;         // Job payment amount in satoshis
    uint32_t timeout_blocks;      // Timeout in blocks
    uint32_t created_height;      // Block height when created
    
    // Validation methods (thread-safe)
    bool IsValid() const MMP_THREAD_SAFE;
    bool IsValidURL() const MMP_THREAD_SAFE;
    bool IsValidHash() const MMP_THREAD_SAFE;
    bool IsValidAmount() const MMP_THREAD_SAFE;
    bool IsValidTimeout() const MMP_THREAD_SAFE;
    std::string GetValidationError() const MMP_THREAD_SAFE;
    std::pair<bool, MMPError> ValidateEx() const MMP_THREAD_SAFE;
    
    // Utility methods (thread-safe)
    std::string ToString() const MMP_THREAD_SAFE;
    size_t EstimateSize() const MMP_THREAD_SAFE;
};

// Taproot script paths for two-phase escrow system
struct TaprootPaths {
    CScript cooperative_path;     // Employer + Worker (2-of-2 multisig)
    CScript worker_timeout_path;  // Worker-only after 24h timeout
    CScript employer_dispute;     // Employer + Middleman (dispute resolution)
    CScript worker_dispute;       // Worker + Middleman (dispute resolution)
    CScript middleman_split;      // Middleman decides split (dispute resolution)
    CScript emergency_path;       // Middleman emergency resolution
};

/**
 * @struct SelectionCriteria
 * @brief Weighted criteria for middleman selection
 */
struct SelectionCriteria {
    double reputation_weight{0.4};         // Weight for reputation score (40%)
    double response_time_weight{0.3};      // Weight for response time (30%)
    double fee_weight{0.2};                // Weight for fee competitiveness (20%)
    double specialty_match_weight{0.1};    // Weight for specialty matching (10%)
    double performance_weight{0.0};        // Optional: weight for performance metrics
    
    // Validation
    bool IsValid() const noexcept {
        double total_weight = reputation_weight + response_time_weight + 
                             fee_weight + specialty_match_weight + performance_weight;
        return total_weight >= 0.99 && total_weight <= 1.01; // Allow small floating point errors
    }
    
    // Calculate weighted score for a middleman
    double CalculateScore(const MiddlemanInfo& mm, const std::vector<std::string>& required_specialties,
                         uint64_t max_acceptable_fee, uint32_t max_response_time_blocks) const noexcept {
        if (!mm.IsValid() || !IsValid()) return 0.0;
        
        // Reputation score (0-100 normalized to 0-1)
        double reputation_score = static_cast<double>(mm.GetEffectiveReputation()) / 100.0;
        
        // Response time score (faster = better, normalized to 0-1)
        double response_score = mm.response_time_blocks <= max_response_time_blocks ? 
            1.0 - (static_cast<double>(mm.response_time_blocks) / max_response_time_blocks) : 0.0;
        
        // Fee score (lower fee = better, normalized to 0-1)
        double fee_score = max_acceptable_fee > 0 ? 
            1.0 - (static_cast<double>(mm.fee_sats) / max_acceptable_fee) : 0.5;
        fee_score = std::max(0.0, std::min(1.0, fee_score));
        
        // Specialty match score
        double specialty_score = CalculateSpecialtyMatchScore(mm.specialties, required_specialties);
        
        // Performance score (if enabled)
        double perf_score = performance_weight > 0 ? 
            mm.performance_metrics.GetOverallPerformanceScore() / 100.0 : 0.0;
        
        // Weighted total
        return (reputation_score * reputation_weight) +
               (response_score * response_time_weight) +
               (fee_score * fee_weight) +
               (specialty_score * specialty_match_weight) +
               (perf_score * performance_weight);
    }

private:
    double CalculateSpecialtyMatchScore(const std::vector<std::string>& mm_specialties,
                                       const std::vector<std::string>& required_specialties) const noexcept {
        if (required_specialties.empty()) return 1.0; // No requirements = perfect match
        if (mm_specialties.empty()) return 0.0;       // No specialties = no match
        
        size_t matches = 0;
        for (const auto& required : required_specialties) {
            if (std::find(mm_specialties.begin(), mm_specialties.end(), required) != mm_specialties.end()) {
                matches++;
            }
        }
        
        return static_cast<double>(matches) / required_specialties.size();
    }
};

/**
 * @struct Specialty
 * @brief Detailed specialty tracking for middlemen
 */
struct Specialty {
    std::string category;                   // e.g., "SOFTWARE", "FINANCE", "LEGAL"
    std::string subcategory;                // e.g., "WEB_DEVELOPMENT", "BLOCKCHAIN", "CONTRACT_LAW"
    uint32_t jobs_completed{0};             // Number of jobs completed in this specialty
    double avg_rating{0.0};                 // Average rating for this specialty (0.0-5.0)
    uint32_t total_ratings{0};              // Total number of ratings received
    int64_t last_job_timestamp{0};          // Last job completed in this specialty
    bool is_certified{false};               // Whether middleman has certification in this area
    std::string certification_authority;    // Certifying body (if applicable)
    
    // Default constructor
    Specialty() = default;
    
    // Constructor
    Specialty(const std::string& cat, const std::string& subcat = "")
        : category(cat), subcategory(subcat) {}
    
    // Validation
    bool IsValid() const noexcept {
        return !category.empty() && 
               category.length() <= MMP_MAX_SPECIALTY_LENGTH &&
               subcategory.length() <= MMP_MAX_SPECIALTY_LENGTH &&
               avg_rating >= 0.0 && avg_rating <= 5.0 &&
               (total_ratings == 0 || avg_rating > 0.0); // If ratings exist, avg must be > 0
    }
    
    // Add a rating for this specialty
    void AddRating(double rating) {
        if (rating >= 0.0 && rating <= 5.0) {
            avg_rating = (avg_rating * total_ratings + rating) / (total_ratings + 1);
            total_ratings++;
        }
    }
    
    // Record job completion
    void RecordJobCompletion(double rating, int64_t completion_timestamp) {
        jobs_completed++;
        last_job_timestamp = completion_timestamp;
        AddRating(rating);
    }
    
    // Get specialty experience score (0-100)
    uint32_t GetExperienceScore() const noexcept {
        uint32_t job_score = std::min(50u, jobs_completed * 2); // Up to 50 points for jobs
        uint32_t rating_score = static_cast<uint32_t>(avg_rating * 10); // Up to 50 points for rating
        uint32_t cert_bonus = is_certified ? 10 : 0; // 10 point bonus for certification
        
        return std::min(100u, job_score + rating_score + cert_bonus);
    }
    
    // Check if specialty is active (recent work)
    bool IsActive(int64_t current_timestamp, int64_t max_age_seconds = 180 * 24 * 3600) const noexcept {
        return jobs_completed > 0 && 
               (current_timestamp - last_job_timestamp) <= max_age_seconds; // Default 6 months
    }
    
    // Get full specialty identifier
    std::string GetFullSpecialty() const {
        return subcategory.empty() ? category : category + "::" + subcategory;
    }
};

/**
 * @struct DisputeEvidence
 * @brief Comprehensive evidence system for disputes
 */
struct DisputeEvidence {
    std::vector<std::string> evidence_urls;        // IPFS/HTTPS links to evidence
    std::vector<uint256> content_hashes;           // SHA256 hashes of evidence content
    std::vector<CPubKey> notary_signatures;        // Optional third-party verification
    std::vector<std::string> evidence_descriptions; // Human-readable descriptions
    int64_t timestamp{0};                          // When evidence was submitted
    CPubKey submitter_key;                         // Who submitted the evidence
    std::string evidence_type;                     // "INITIAL", "REBUTTAL", "COUNTER", "EXPERT"
    bool is_sealed{false};                         // Whether evidence is sealed until dispute
    uint256 seal_hash;                             // Hash for sealed evidence
    
    // Default constructor
    DisputeEvidence() = default;
    
    // Constructor for evidence submission
    DisputeEvidence(const std::vector<std::string>& urls, 
                   const std::vector<uint256>& hashes,
                   const CPubKey& submitter,
                   const std::string& type = "INITIAL")
        : evidence_urls(urls), content_hashes(hashes), submitter_key(submitter),
          evidence_type(type), timestamp(GetCurrentTimestamp()) {}
    
    // Validation
    bool IsValid() const noexcept {
        if (evidence_urls.size() != content_hashes.size()) return false;
        if (evidence_urls.empty()) return false;
        if (!submitter_key.IsValid()) return false;
        if (evidence_type.empty()) return false;
        if (timestamp <= 0) return false;
        
        // Validate URLs (basic check)
        for (const auto& url : evidence_urls) {
            if (url.empty() || url.length() > MMP_MAX_METADATA_URL_LENGTH) {
                return false;
            }
        }
        
        // Validate hashes
        for (const auto& hash : content_hashes) {
            if (hash.IsNull()) return false;
        }
        
        return true;
    }
    
    // Add notary signature
    void AddNotarySignature(const CPubKey& notary_key) {
        if (notary_key.IsValid()) {
            notary_signatures.push_back(notary_key);
        }
    }
    
    // Check if evidence has notary verification
    bool HasNotaryVerification() const noexcept {
        return !notary_signatures.empty();
    }
    
    // Get evidence credibility score (0-100)
    uint32_t GetCredibilityScore() const noexcept {
        uint32_t base_score = 50; // Base credibility
        
        // Bonus for notary signatures (up to +30)
        base_score += std::min(30u, static_cast<uint32_t>(notary_signatures.size() * 10));
        
        // Bonus for multiple evidence pieces (up to +20)
        base_score += std::min(20u, static_cast<uint32_t>(evidence_urls.size() * 5));
        
        return std::min(100u, base_score);
    }
    
    // Seal evidence (for pre-dispute submission)
    void SealEvidence(const uint256& seal) {
        if (!seal.IsNull()) {
            is_sealed = true;
            seal_hash = seal;
        }
    }
    
    // Unseal evidence (during dispute)
    bool UnsealEvidence(const uint256& provided_seal) {
        if (is_sealed && seal_hash == provided_seal) {
            is_sealed = false;
            return true;
        }
        return false;
    }
};

/**
 * @struct FallbackArbitratorRotation
 * @brief Emergency rotation system for fallback arbitrators
 */
struct FallbackArbitratorRotation {
    std::vector<CPubKey> active_arbitrators;    // Pool of active fallback arbitrators
    uint32_t rotation_blocks{4032};             // Rotation every ~4 weeks (4032 blocks)
    uint32_t next_rotation_height{0};           // Next scheduled rotation block height
    uint32_t current_arbitrator_index{0};       // Current active arbitrator index
    std::vector<CPubKey> emergency_pool;        // Emergency backup pool
    bool emergency_rotation_active{false};      // Whether emergency rotation is active
    uint32_t min_arbitrators{3};                // Minimum number of arbitrators required
    uint32_t max_arbitrators{10};               // Maximum number of arbitrators allowed
    
    // Default constructor
    FallbackArbitratorRotation() = default;
    
    // Constructor with initial setup
    FallbackArbitratorRotation(const std::vector<CPubKey>& arbitrators, uint32_t rotation_period = 4032)
        : active_arbitrators(arbitrators), rotation_blocks(rotation_period) {
        if (!arbitrators.empty()) {
            next_rotation_height = GetCurrentBlockHeight() + rotation_blocks;
        }
    }
    
    // Validation
    bool IsValid() const noexcept {
        if (active_arbitrators.size() < min_arbitrators || 
            active_arbitrators.size() > max_arbitrators) {
            return false;
        }
        
        // Validate all arbitrator keys
        for (const auto& key : active_arbitrators) {
            if (!key.IsValid()) return false;
        }
        
        for (const auto& key : emergency_pool) {
            if (!key.IsValid()) return false;
        }
        
        return rotation_blocks >= 144 && // At least 24h rotation period
               current_arbitrator_index < active_arbitrators.size() &&
               next_rotation_height > 0;
    }
    
    // Get current active arbitrator
    CPubKey GetCurrentArbitrator() const noexcept {
        if (active_arbitrators.empty()) return CPubKey();
        return active_arbitrators[current_arbitrator_index];
    }
    
    // Check if rotation is due
    bool IsRotationDue(uint32_t current_block_height) const noexcept {
        return current_block_height >= next_rotation_height;
    }
    
    // Perform scheduled rotation
    bool PerformRotation(uint32_t current_block_height) noexcept {
        if (!IsRotationDue(current_block_height) || active_arbitrators.empty()) {
            return false;
        }
        
        // Move to next arbitrator
        current_arbitrator_index = (current_arbitrator_index + 1) % active_arbitrators.size();
        next_rotation_height = current_block_height + rotation_blocks;
        
        return true;
    }
    
    // Emergency rotation (when current arbitrator fails)
    bool PerformEmergencyRotation(const CPubKey& failed_arbitrator, uint32_t current_block_height) noexcept {
        // Remove failed arbitrator from active pool
        auto it = std::find(active_arbitrators.begin(), active_arbitrators.end(), failed_arbitrator);
        if (it != active_arbitrators.end()) {
            active_arbitrators.erase(it);
            
            // Adjust current index if needed
            if (current_arbitrator_index >= active_arbitrators.size() && !active_arbitrators.empty()) {
                current_arbitrator_index = 0;
            }
        }
        
        // Add from emergency pool if available and needed
        if (active_arbitrators.size() < min_arbitrators && !emergency_pool.empty()) {
            active_arbitrators.push_back(emergency_pool.back());
            emergency_pool.pop_back();
        }
        
        // Check if we still have enough arbitrators
        if (active_arbitrators.size() < min_arbitrators) {
            emergency_rotation_active = true;
            return false; // Critical: not enough arbitrators
        }
        
        // Schedule next rotation sooner for emergency
        next_rotation_height = current_block_height + (rotation_blocks / 4); // 25% of normal period
        emergency_rotation_active = true;
        
        return true;
    }
    
    // Add new arbitrator to pool (requires DAO approval)
    bool AddArbitrator(const CPubKey& new_arbitrator, bool dao_approved = false) noexcept {
        if (!dao_approved || !new_arbitrator.IsValid()) return false;
        
        // Check if already exists
        if (std::find(active_arbitrators.begin(), active_arbitrators.end(), new_arbitrator) != active_arbitrators.end()) {
            return false; // Already in pool
        }
        
        if (active_arbitrators.size() >= max_arbitrators) {
            // Add to emergency pool instead
            emergency_pool.push_back(new_arbitrator);
        } else {
            active_arbitrators.push_back(new_arbitrator);
        }
        
        return true;
    }
    
    // Remove arbitrator from pool (requires DAO approval)
    bool RemoveArbitrator(const CPubKey& arbitrator, bool dao_approved = false) noexcept {
        if (!dao_approved) return false;
        
        auto it = std::find(active_arbitrators.begin(), active_arbitrators.end(), arbitrator);
        if (it != active_arbitrators.end()) {
            active_arbitrators.erase(it);
            
            // Adjust current index if needed
            if (current_arbitrator_index >= active_arbitrators.size() && !active_arbitrators.empty()) {
                current_arbitrator_index = 0;
            }
            
            return true;
        }
        
        // Try emergency pool
        auto emergency_it = std::find(emergency_pool.begin(), emergency_pool.end(), arbitrator);
        if (emergency_it != emergency_pool.end()) {
            emergency_pool.erase(emergency_it);
            return true;
        }
        
        return false;
    }
    
    // Get rotation status
    std::string GetRotationStatus(uint32_t current_block_height) const {
        if (active_arbitrators.empty()) return "NO_ARBITRATORS";
        if (emergency_rotation_active) return "EMERGENCY_ACTIVE";
        if (IsRotationDue(current_block_height)) return "ROTATION_DUE";
        return "NORMAL";
    }
};

/**
 * @struct BondInsurance
 * @brief Insurance parameters for high-value jobs
 */
struct BondInsurance {
    bool is_insured{false};                 // Whether bond is insured
    uint64_t coverage_amount_sats{0};       // Insurance coverage amount
    double premium_rate{0.0};               // Premium rate (e.g., 0.02 = 2%)
    CPubKey insurer_key;                    // Insurance provider's key
    uint256 policy_hash;                    // Hash of insurance policy terms
    int64_t policy_expiry{0};               // When insurance expires
    uint64_t premium_paid_sats{0};          // Premium amount paid
    bool is_premium_paid{false};            // Whether premium was paid
    
    // Default constructor
    BondInsurance() = default;
    
    // Constructor for insured bond
    BondInsurance(uint64_t coverage, double rate, const CPubKey& insurer, 
                  const uint256& policy, int64_t expiry)
        : is_insured(true), coverage_amount_sats(coverage), premium_rate(rate),
          insurer_key(insurer), policy_hash(policy), policy_expiry(expiry) {
        premium_paid_sats = static_cast<uint64_t>(coverage * rate);
    }
    
    // Validation
    bool IsValid() const noexcept {
        if (!is_insured) return true; // Uninsured is valid
        
        return coverage_amount_sats > 0 &&
               premium_rate > 0.0 && premium_rate <= 0.1 && // Max 10% premium
               insurer_key.IsValid() &&
               !policy_hash.IsNull() &&
               policy_expiry > 0 &&
               premium_paid_sats > 0;
    }
    
    // Check if insurance is active
    bool IsActive(int64_t current_timestamp) const noexcept {
        return is_insured && 
               is_premium_paid && 
               current_timestamp < policy_expiry &&
               IsValid();
    }
    
    // Calculate required premium
    uint64_t CalculateRequiredPremium() const noexcept {
        return static_cast<uint64_t>(coverage_amount_sats * premium_rate);
    }
    
    // Get effective coverage (considering policy terms)
    uint64_t GetEffectiveCoverage(uint64_t claim_amount) const noexcept {
        if (!is_insured || !is_premium_paid) return 0;
        return std::min(claim_amount, coverage_amount_sats);
    }
};

/**
 * @struct InsuranceClaim
 * @brief Insurance claim processing for bond coverage
 */
struct InsuranceClaim {
    uint256 job_id;                             // Associated job contract ID
    uint256 claim_txid;                         // Transaction ID for claim payout
    uint32_t claim_vout{0};                     // Output index for claim payout
    uint64_t claim_amount_sats{0};              // Claimed amount
    uint64_t approved_amount_sats{0};           // Approved payout amount
    CPubKey claimant_key;                       // Who is making the claim
    CPubKey middleman_key;                      // Middleman whose bond is claimed against
    CPubKey insurer_key;                        // Insurance provider
    uint256 policy_hash;                        // Hash of insurance policy
    std::string claim_reason;                   // Reason for claim
    std::vector<uint8_t> evidence_hash;         // Hash of supporting evidence
    std::vector<CPubKey> approvers;             // Insurance approvers/adjusters
    int64_t claim_timestamp{0};                 // When claim was submitted
    int64_t approval_timestamp{0};              // When claim was approved
    int64_t payout_timestamp{0};                // When claim was paid out
    ClaimStatus status{ClaimStatus::PENDING};   // Current claim status
    std::string adjuster_notes;                 // Notes from insurance adjuster
    
    // Claim status enumeration
    enum class ClaimStatus {
        PENDING,        // Claim submitted, under review
        INVESTIGATING,  // Under investigation by adjusters
        APPROVED,       // Claim approved for payout
        REJECTED,       // Claim rejected
        PAID,          // Claim paid out
        DISPUTED,      // Claim is disputed
        EXPIRED        // Claim expired without resolution
    };
    
    // Default constructor
    InsuranceClaim() = default;
    
    // Constructor for new claim
    InsuranceClaim(const uint256& job, const CPubKey& claimant, const CPubKey& middleman,
                   const CPubKey& insurer, uint64_t amount, const std::string& reason)
        : job_id(job), claimant_key(claimant), middleman_key(middleman),
          insurer_key(insurer), claim_amount_sats(amount), claim_reason(reason),
          claim_timestamp(GetCurrentTimestamp()), status(ClaimStatus::PENDING) {}
    
    // Validation
    bool IsValid() const noexcept {
        return !job_id.IsNull() &&
               claimant_key.IsValid() &&
               middleman_key.IsValid() &&
               insurer_key.IsValid() &&
               claim_amount_sats > 0 &&
               !claim_reason.empty() &&
               claim_reason.length() <= MMP_MAX_DISPUTE_REASON_LENGTH &&
               claim_timestamp > 0 &&
               approvers.size() <= MMP_MAX_INSURANCE_APPROVERS;
    }
    
    // Add approver signature
    bool AddApprover(const CPubKey& approver_key) noexcept {
        if (!approver_key.IsValid()) return false;
        
        // Check if already approved by this key
        if (std::find(approvers.begin(), approvers.end(), approver_key) != approvers.end()) {
            return false; // Already approved
        }
        
        if (approvers.size() >= MMP_MAX_INSURANCE_APPROVERS) {
            return false; // Too many approvers
        }
        
        approvers.push_back(approver_key);
        return true;
    }
    
    // Check if claim has sufficient approvals
    bool HasSufficientApprovals() const noexcept {
        return approvers.size() >= MMP_MIN_INSURANCE_APPROVERS;
    }
    
    // Approve claim
    bool ApproveClaim(uint64_t approved_amount, const std::string& notes = "") {
        if (status != ClaimStatus::PENDING && status != ClaimStatus::INVESTIGATING) {
            return false; // Can only approve pending/investigating claims
        }
        
        approved_amount_sats = std::min(approved_amount, claim_amount_sats);
        adjuster_notes = notes;
        approval_timestamp = GetCurrentTimestamp();
        status = ClaimStatus::APPROVED;
        
        return true;
    }
    
    // Reject claim
    bool RejectClaim(const std::string& reason) {
        if (status != ClaimStatus::PENDING && status != ClaimStatus::INVESTIGATING) {
            return false; // Can only reject pending/investigating claims
        }
        
        adjuster_notes = reason;
        approval_timestamp = GetCurrentTimestamp();
        status = ClaimStatus::REJECTED;
        
        return true;
    }
    
    // Mark claim as paid
    bool MarkAsPaid(const uint256& payout_txid, uint32_t vout) {
        if (status != ClaimStatus::APPROVED) {
            return false; // Can only pay approved claims
        }
        
        claim_txid = payout_txid;
        claim_vout = vout;
        payout_timestamp = GetCurrentTimestamp();
        status = ClaimStatus::PAID;
        
        return true;
    }
    
    // Check if claim is expired
    bool IsExpired(int64_t current_timestamp, int64_t expiry_period = 90 * 24 * 3600) const noexcept {
        return (current_timestamp - claim_timestamp) > expiry_period; // Default 90 days
    }
    
    // Get claim age in days
    uint32_t GetClaimAgeDays(int64_t current_timestamp) const noexcept {
        return static_cast<uint32_t>((current_timestamp - claim_timestamp) / (24 * 3600));
    }
    
    // Get status string
    std::string GetStatusString() const {
        switch (status) {
            case ClaimStatus::PENDING: return "PENDING";
            case ClaimStatus::INVESTIGATING: return "INVESTIGATING";
            case ClaimStatus::APPROVED: return "APPROVED";
            case ClaimStatus::REJECTED: return "REJECTED";
            case ClaimStatus::PAID: return "PAID";
            case ClaimStatus::DISPUTED: return "DISPUTED";
            case ClaimStatus::EXPIRED: return "EXPIRED";
            default: return "UNKNOWN";
        }
    }
    
    // Calculate payout ratio
    double GetPayoutRatio() const noexcept {
        if (claim_amount_sats == 0) return 0.0;
        return static_cast<double>(approved_amount_sats) / claim_amount_sats;
    }
};

/**
 * @struct MiddlemanPerformance
 * @brief Detailed performance metrics for middlemen
 */
struct MiddlemanPerformance {
    double avg_resolution_time_hours{0.0};      // Average time to resolve disputes (hours)
    double employer_satisfaction{0.0};          // Average employer satisfaction (0.0-5.0)
    double worker_satisfaction{0.0};            // Average worker satisfaction (0.0-5.0)
    uint32_t repeat_clients{0};                 // Number of clients who used middleman multiple times
    uint32_t total_feedback_count{0};           // Total number of feedback entries
    double response_time_percentile_95{0.0};    // 95th percentile response time (hours)
    uint32_t complex_disputes_handled{0};       // Number of complex/high-value disputes
    double appeal_success_rate{0.0};            // Rate of successful appeals against this middleman
    int64_t last_performance_update{0};         // Last time metrics were updated
    
    // Default constructor
    MiddlemanPerformance() = default;
    
    // Calculate overall performance score (0-100)
    inline double GetOverallPerformanceScore() const noexcept {
        if (total_feedback_count == 0) return 50.0; // Neutral score for new middlemen
        
        // Weighted performance calculation
        double satisfaction_score = ((employer_satisfaction + worker_satisfaction) / 2.0) * 20.0; // 0-100
        double efficiency_score = CalculateEfficiencyScore();
        double reliability_score = CalculateReliabilityScore();
        
        // Weighted average: 40% satisfaction, 30% efficiency, 30% reliability
        return (satisfaction_score * 0.4) + (efficiency_score * 0.3) + (reliability_score * 0.3);
    }
    
    // Check if performance metrics are recent enough to be reliable
    inline bool IsPerformanceDataFresh(int64_t current_timestamp, int64_t max_age_seconds = 30 * 24 * 3600) const noexcept {
        return (current_timestamp - last_performance_update) <= max_age_seconds; // Default 30 days
    }

private:
    inline double CalculateEfficiencyScore() const noexcept {
        if (avg_resolution_time_hours <= 0) return 50.0;
        
        // Score based on resolution time (faster = better)
        // 24h = 100, 48h = 80, 72h = 60, 168h = 20
        const double ideal_time = 24.0;
        const double max_acceptable_time = 168.0; // 1 week
        
        if (avg_resolution_time_hours <= ideal_time) return 100.0;
        if (avg_resolution_time_hours >= max_acceptable_time) return 20.0;
        
        // Linear interpolation between ideal and max
        return 100.0 - ((avg_resolution_time_hours - ideal_time) / (max_acceptable_time - ideal_time)) * 80.0;
    }
    
    inline double CalculateReliabilityScore() const noexcept {
        // Score based on appeal success rate (lower appeals = higher reliability)
        double appeal_penalty = appeal_success_rate * 50.0; // Max 50 point penalty
        double repeat_client_bonus = std::min(static_cast<double>(repeat_clients) * 2.0, 20.0); // Max 20 point bonus
        
        return std::max(30.0, std::min(100.0, 80.0 - appeal_penalty + repeat_client_bonus));
    }
};

/**
 * @struct MiddlemanInfo
 * @brief Represents a potential middleman with security features
 */
struct MiddlemanInfo {
    CPubKey pubkey;                         // Middleman's public key
    std::string middleman_name;             // Human-readable name
    std::string contact_info;               // Contact information
    std::string description;                // Description of services
    uint64_t fee_sats{0};                  // Middleman fee in satoshis
    uint32_t reputation_score{0};           // 0-100 (where 100 is most trusted)
    uint64_t bond_amount_sats{0};          // Security deposit (slashed for bad decisions)
    std::string identity_hash;              // Obfuscated identity (e.g., SHA256("Arbitrator LLC"))
    uint32_t total_disputes{0};            // Total disputes handled
    uint32_t successful_resolutions{0};    // Successfully resolved disputes
    uint32_t bond_slashes{0};              // Number of times bond was slashed (reputation impact)
    std::vector<std::string> specialties;   // e.g., ["SOFTWARE", "DESIGN", "WRITING"] (deprecated)
    std::vector<Specialty> detailed_specialties; // Enhanced specialty tracking
    int64_t last_active_timestamp{0};      // Last activity timestamp
    bool is_active{false};                 // Whether middleman is currently active
    bool accepts_new_jobs{false};          // Whether accepting new arbitration jobs
    uint32_t max_job_amount_sats{0};       // Maximum job amount they'll handle
    uint32_t response_time_blocks{144};    // Typical response time in blocks (default 24h)
    bool is_dao_approved{false};           // Whether approved by DAO/governance
    bool is_kyc_verified{false};           // Whether KYC verified (for fallback arbitrators)
    
    // Enhanced reputation decay parameters (configurable per middleman)
    double reputation_decay_half_life_days{180.0}; // 6 months default half-life
    double min_reputation_retention{0.5};           // Minimum 50% reputation retained
    bool use_custom_decay{false};                   // Whether to use custom decay parameters
    
    // Performance metrics
    MiddlemanPerformance performance_metrics;       // Detailed performance tracking
    
    // Bond insurance (optional for high-value jobs)
    BondInsurance bond_insurance;                   // Insurance coverage for bond
    
    // Default constructor with safe defaults
    MiddlemanInfo() = default;
    
    // Constructor with required fields
    explicit MiddlemanInfo(const CPubKey& key, const std::string& name, uint64_t fee, uint64_t bond)
        : pubkey(key), middleman_name(name), fee_sats(fee), bond_amount_sats(bond), 
          is_active(true), accepts_new_jobs(true) {}
    
    // Copy/move semantics (explicitly defaulted for clarity)
    MiddlemanInfo(const MiddlemanInfo&) = default;
    MiddlemanInfo& operator=(const MiddlemanInfo&) = default;
    MiddlemanInfo(MiddlemanInfo&&) noexcept = default;
    MiddlemanInfo& operator=(MiddlemanInfo&&) noexcept = default;
    
    // Destructor
    ~MiddlemanInfo() = default;
    
    // Comprehensive validation with all security checks
    inline bool IsValid() const noexcept {
        return pubkey.IsValid() && 
               bond_amount_sats >= MMP_MIN_MIDDLEMAN_BOND_SATS && 
               bond_amount_sats <= MMP_MAX_MIDDLEMAN_BOND_SATS &&
               reputation_score <= MMP_MAX_REPUTATION_SCORE &&
               !middleman_name.empty() &&
               middleman_name.length() <= MMP_MAX_MIDDLEMAN_NAME_LENGTH &&
               description.length() <= MMP_MAX_MIDDLEMAN_DESCRIPTION_LENGTH &&
               specialties.size() <= MMP_MAX_SPECIALTIES_PER_USER &&
               fee_sats > 0 &&
               fee_sats <= bond_amount_sats &&  // Fee shouldn't exceed bond (economic security)
               ValidateSpecialties() &&        // Legacy specialties validation
               ValidateDetailedSpecialties() && // Enhanced specialties validation
               ValidateEconomicRatios() &&     // All economic security checks
               bond_insurance.IsValid() &&     // Insurance validation (if applicable)
               performance_metrics.IsPerformanceDataFresh(GetCurrentTimestamp(), 365 * 24 * 3600); // Performance data not too old
    }
    
    // Calculate effective reputation (considering bond slashes) - inline for performance
    inline uint32_t GetEffectiveReputation() const noexcept {
        if (total_disputes == 0) return reputation_score;
        uint32_t penalty = (bond_slashes * 100) / total_disputes; // Penalty per slash
        return reputation_score > penalty ? reputation_score - penalty : 0;
    }
    
    // Enhanced reputation calculation with time-based weighting
    inline double GetTimeWeightedReputation(int64_t current_timestamp = 0) const noexcept {
        if (total_disputes == 0) return static_cast<double>(reputation_score);
        
        // Apply time decay - older disputes matter less
        const int64_t time_since_last_active = current_timestamp > 0 ? 
            current_timestamp - last_active_timestamp : 0;
        const double time_decay_factor = CalculateTimeDecayFactor(time_since_last_active);
        
        double base_reputation = static_cast<double>(GetEffectiveReputation());
        return base_reputation * time_decay_factor;
    }
    
    // Inline getters for common operations
    inline double GetSuccessRate() const noexcept {
        return total_disputes > 0 ? static_cast<double>(successful_resolutions) / total_disputes : 0.0;
    }
    
    inline bool IsHighReputation() const noexcept {
        return GetEffectiveReputation() >= MMP_MIN_REPUTATION_FOR_AUTO;
    }
    
    inline bool CanHandleJob(uint64_t job_amount_sats) const noexcept {
        return is_active && accepts_new_jobs && 
               (max_job_amount_sats == 0 || job_amount_sats <= max_job_amount_sats);
    }
    
    // Enhanced qualification check for specific job requirements
    inline bool IsQualifiedForJob(uint64_t job_amount_sats, uint32_t min_reputation = MMP_MIN_REPUTATION_FOR_AUTO) const noexcept {
        return is_active && 
               accepts_new_jobs &&
               (max_job_amount_sats == 0 || job_amount_sats <= max_job_amount_sats) &&
               GetEffectiveReputation() >= min_reputation &&
               bond_amount_sats >= CalculateMinBondForJob(job_amount_sats);
    }
    
    // Calculate minimum bond required for a job (risk-based)
    inline uint64_t CalculateMinBondForJob(uint64_t job_amount_sats) const noexcept {
        // Bond should be at least 10% of job value, but not less than minimum
        uint64_t risk_based_bond = job_amount_sats / 10;
        return std::max(risk_based_bond, MMP_MIN_MIDDLEMAN_BOND_SATS);
    }

private:
    // Validate specialties vector
    inline bool ValidateSpecialties() const noexcept {
        for (const auto& specialty : specialties) {
            if (specialty.empty() || specialty.length() > MMP_MAX_SPECIALTY_LENGTH) {
                return false;
            }
        }
        return true;
    }
    
    // Enhanced economic validation with all security checks
    inline bool ValidateEconomicRatios() const noexcept {
        // Critical security check: Fee vs Bond ratio
        if (fee_sats > bond_amount_sats * MMP_MAX_FEE_TO_BOND_RATIO) {
            return false; // Fee exceeds maximum allowed ratio
        }
        
        // Critical security check: Bond vs Job ratio
        if (max_job_amount_sats > 0 && bond_amount_sats < max_job_amount_sats * MMP_MIN_BOND_TO_JOB_RATIO) {
            return false; // Bond too low for maximum job amount
        }
        
        // Insurance validation (if applicable)
        if (bond_insurance.is_insured && !bond_insurance.IsValid()) {
            return false; // Invalid insurance configuration
        }
        
        // Insurance coverage should not exceed bond amount
        if (bond_insurance.is_insured && bond_insurance.coverage_amount_sats > bond_amount_sats) {
            return false; // Over-insurance not allowed
        }
        
        return true;
    }
    
    // Validate detailed specialties
    inline bool ValidateDetailedSpecialties() const noexcept {
        if (detailed_specialties.size() > MMP_MAX_SPECIALTIES_PER_USER) {
            return false;
        }
        
        for (const auto& specialty : detailed_specialties) {
            if (!specialty.IsValid()) {
                return false;
            }
        }
        
        return true;
    }
    
    // Enhanced time decay factor with configurable parameters
    inline double CalculateTimeDecayFactor(int64_t time_since_last_active) const noexcept {
        if (time_since_last_active <= 0) return 1.0; // No decay if timestamp not provided
        
        // Use custom decay parameters if configured, otherwise use defaults
        const double half_life_seconds = use_custom_decay ? 
            reputation_decay_half_life_days * 24 * 3600 : 
            180.0 * 24 * 3600; // 6 months default
            
        const double min_retention = use_custom_decay ? 
            min_reputation_retention : 
            0.5; // 50% default minimum
        
        // Exponential decay with half-life: factor = min_retention + (1 - min_retention) * exp(-ln(2) * t / half_life)
        const double decay_factor = std::exp(-(static_cast<double>(time_since_last_active) / half_life_seconds) * std::log(2.0));
        
        // Ensure minimum retention
        return min_retention + (1.0 - min_retention) * decay_factor;
    }
    
    // Set custom decay parameters (requires DAO approval for fairness)
    inline void SetCustomDecayParameters(double half_life_days, double min_retention, bool dao_approved = false) noexcept {
        if (dao_approved && half_life_days > 30.0 && min_retention >= 0.3 && min_retention <= 0.8) {
            reputation_decay_half_life_days = half_life_days;
            min_reputation_retention = min_retention;
            use_custom_decay = true;
        }
    }
    
    std::string ToString() const;
};

// Key aggregation context for two-phase escrow
struct KeyAggregationContext {
    CPubKey employer_key;
    CPubKey worker_key;           // May be empty until worker is assigned
    CPubKey middleman_key;        // May be empty until dispute occurs
    CPubKey aggregated_key;       // Schnorr aggregated public key (employer + worker)
    uint256 tweak;                // Taproot tweak
    bool worker_assigned;         // Flag indicating if worker key is set
    bool middleman_assigned;      // Flag indicating if middleman key is set (only during dispute)
    uint32_t timeout_blocks;      // 24h timeout in blocks (~144 blocks)
    
    // Validation and consistency methods
    bool IsValid() const;
    bool AreKeysValid() const;
    bool IsAggregationConsistent() const;
    bool IsTweakConsistent() const;
    bool VerifyAggregatedKey() const;
    std::string GetValidationError() const;
    std::pair<bool, MMPError> ValidateEx() const;
    
    // Utility methods
    void Clear();
    bool IsEmpty() const;
    std::string ToString() const;
};

// Worker application structure
struct WorkerApplication {
    CPubKey worker_key;
    std::string application_message;    // Worker's proposal/message
    uint64_t proposed_fee_sats;        // Worker's proposed fee (optional)
    int64_t application_timestamp;
    std::string contact_info;          // Optional contact information
    
    // Validation methods
    bool IsValid() const;
    std::string ToString() const;
};

// Job contract structure
struct JobContract {
    uint256 job_id;
    KeyAggregationContext keys;
    TaprootPaths script_paths;
    JobMetadata metadata;
    JobState state;
    uint256 funding_txid;         // Transaction that funds the escrow
    uint32_t funding_vout;        // Output index in funding transaction
    uint256 resolution_txid;      // Final resolution transaction (if any)
    ResolutionPath resolution_path;
    
    // Worker application management
    std::vector<WorkerApplication> worker_applications;  // List of worker applications
    CPubKey assigned_worker;      // Currently assigned worker (empty if none)
    
    // Dispute resolution management
    DisputeResolutionConfig dispute_config; // How middleman will be selected if dispute occurs
    MiddlemanInfo middleman_info; // Middleman details (only set when dispute occurs)
    bool dispute_raised;          // Whether a dispute has been raised
    int64_t dispute_timestamp;    // When dispute was raised (0 if none)
    int64_t completion_timestamp; // When work was marked complete (for 24h timeout)
    CPubKey dispute_initiator;    // Who raised the dispute (employer or worker)
    std::vector<CPubKey> proposed_middlemen; // Middlemen proposed by each party
    bool middleman_agreed;        // Whether both parties agreed on middleman
    
    // Event tracking for auditing
    struct ContractEvent {
        int64_t timestamp;
        JobState state;
        uint256 txid;
        std::string memo;
        
        std::string ToString() const;
    };
    std::vector<ContractEvent> history;
    
    // Validation methods
    bool IsValid() const;
    std::string GetValidationError() const;
    std::pair<bool, MMPError> ValidateEx() const;
    
    // Utility methods
    std::string ToString() const;
    bool IsResolved() const;
    bool IsExpired(uint32_t current_height) const;
    bool CanWorkerClaimTimeout(int64_t current_timestamp) const; // Check if 24h timeout passed
    bool IsInDisputePeriod(int64_t current_timestamp) const;     // Check if still in 24h dispute window
    void AddEvent(JobState new_state, const uint256& txid = uint256(), const std::string& memo = "") MMP_REQUIRES_LOCK;
};

/**
 * @file mmp.h
 * @brief The Middleman Protocol (MMP) - Two-Phase Escrow System using Taproot
 * 
 * This header defines the complete MMP protocol for trustless job settlements using:
 * - Two-phase escrow: 2-of-2 (Employer + Worker) with 24h timeout for worker claims
 * - Taproot key aggregation for privacy (appears as single-key spend when parties agree)
 * - MAST (Merklized Abstract Syntax Trees) for dispute resolution paths
 * - Middleman arbitration only when disputes are raised
 * - OP_RETURN anchors for off-chain metadata storage
 * 
 * @section Two-Phase Escrow System
 * 1. Phase 1: Employer locks funds in 2-of-2 (Employer + Worker) address
 * 2. Phase 2: After work completion:
 *    - No dispute in 24h → Worker can claim funds alone (timeout path)
 *    - Dispute raised → Escalates to Middleman arbitration
 * 
 * @section State Machine
 * Job states follow this transition model:
 * CREATED → OPEN → ASSIGNED → IN_PROGRESS → COMPLETED → RESOLVED
 *                           ↘ DISPUTED → RESOLVED
 *                           ↘ CANCELLED/EXPIRED
 * 
 * Detailed workflow:
 * 1. CREATED: Job posted by employer, no funds locked yet
 * 2. OPEN: Funds locked in 2-of-2 escrow, job open for worker applications
 * 3. ASSIGNED: Employer selects worker, awaiting worker acceptance
 * 4. IN_PROGRESS: Worker accepted job, work in progress
 * 5. COMPLETED: Worker submitted work, 24h dispute window starts
 * 6. RESOLVED: Job completed (cooperative/timeout) or dispute resolved
 * 
 * @section Exception Safety Guarantees
 * - **Basic Guarantee**: All functions provide basic exception safety
 * - **Strong Guarantee**: State-modifying functions marked with MMP_STRONG_EXCEPTION_SAFETY
 * - **No-throw Guarantee**: Functions marked noexcept provide no-throw guarantee
 * - **Resource Safety**: All resources are RAII-managed, no manual cleanup required
 * 
 * @section Thread Safety
 * - **Read Operations**: All const methods are thread-safe
 * - **Write Operations**: Non-const methods require external synchronization
 * - **Atomic Operations**: Functions marked MMP_THREAD_SAFE are internally synchronized
 * 
 * @section Thread Safety
 * - All const methods are thread-safe
 * - Non-const methods require external synchronization
 * - Error state functions use thread-local storage
 * 
 * @section Security Considerations
 * - All cryptographic operations use secp256k1
 * - Replay protection via nonces and timestamps
 * - Input validation on all user-provided data
 * - Secure URL schemes enforced (HTTPS/IPFS only)
 */

// Function declarations

// ============================================================================
// SCRIPT CREATION FUNCTIONS
// ============================================================================

/**
 * @brief Script creation functions for Taproot-based escrow
 * @note All functions are thread-safe (const operations only)
 */
CScript CreateTaproot2of3Script(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman);
TaprootPaths CreateTaprootPaths(const KeyAggregationContext& ctx, uint32_t timeout_blocks);
CScript CreateCooperativePath(const CPubKey& employer, const CPubKey& worker);
CScript CreateDisputePath(const CPubKey& party1, const CPubKey& middleman);
CScript CreateTimeoutPath(const CPubKey& employer, uint32_t timeout_blocks);

// ============================================================================
// KEY AGGREGATION (SCHNORR)
// ============================================================================
KeyAggregationContext AggregateKeys(const CPubKey& employer, const CPubKey& worker, const CPubKey& middleman);
KeyAggregationContext CreateInitialKeyContext(const CPubKey& employer, const CPubKey& middleman);
bool UpdateKeyContextWithWorker(KeyAggregationContext& ctx, const CPubKey& worker);

// Temporary escrow address generation (before worker assignment)
CTxDestination CreateTemporaryEscrowAddress(const CPubKey& employer, const CPubKey& middleman, const uint256& job_id);
bool IsTemporaryEscrowAddress(const CTxDestination& dest);
CTxDestination UpdateEscrowAddressWithWorker(const CTxDestination& temp_address, const CPubKey& worker);
CPubKey ComputeAggregatedKey(const std::vector<CPubKey>& pubkeys);
uint256 ComputeTaprootTweak(const CPubKey& internal_key, const uint256& merkle_root);

// ============================================================================
// SERIALIZATION & OP_RETURN OPERATIONS
// ============================================================================
CScript CreateOPReturnAnchor(const JobMetadata& metadata);
std::optional<JobMetadata> ParseOPReturnMetadata(const CScript& scriptPubKey);
std::vector<unsigned char> SerializeJobMetadata(const JobMetadata& metadata);
std::optional<JobMetadata> DeserializeJobMetadata(const std::vector<unsigned char>& data);

// JobContract serialization with version-specific methods
std::vector<unsigned char> SerializeJobContract(const JobContract& contract);
std::optional<JobContract> DeserializeJobContract(const std::vector<unsigned char>& data);

// Version-specific serialization for future compatibility
std::vector<unsigned char> SerializeJobContractV1(const JobContract& contract);
std::optional<JobContract> DeserializeJobContractV1(const std::vector<unsigned char>& data);

// ============================================================================
// ADDRESS GENERATION
// ============================================================================
CTxDestination CreateTaprootDestination(const KeyAggregationContext& ctx, const TaprootPaths& paths);
bool ValidateTaprootDestination(const CTxDestination& dest);

// ============================================================================
// JOB LIFECYCLE MANAGEMENT
// ============================================================================
// Job creation for two-phase escrow system (no middleman needed initially)
JobContract CreateJobContract(const CPubKey& employer, const JobMetadata& metadata);
LockFundsResult LockFunds(JobContract& contract, const CTransaction& funding_tx, uint32_t vout);
JobActionResult AcceptJob(JobContract& contract, const CPubKey& worker_key);
JobActionResult SubmitWork(JobContract& contract, const std::string& work_proof_url);
JobActionResult ConfirmCompletion(JobContract& contract, bool employer_approves);
// Dispute resolution with middleman selection
JobActionResult RaiseDispute(JobContract& contract, const CPubKey& disputer_key, 
                            const std::string& dispute_reason, const CPubKey& proposed_middleman);
JobActionResult ProposeMiddleman(JobContract& contract, const CPubKey& proposer_key, const CPubKey& middleman_key);
JobActionResult AcceptMiddleman(JobContract& contract, const CPubKey& accepter_key, const CPubKey& middleman_key);
JobActionResult ResolveDispute(JobContract& contract, ResolutionPath resolution, const CPubKey& middleman_key);

// Two-phase escrow specific functions
JobActionResult WorkerClaimTimeout(JobContract& contract, const CPubKey& worker_key); // Worker claims after 24h
bool CanRaiseDispute(const JobContract& contract, int64_t current_timestamp);         // Check if dispute can be raised

// ============================================================================
// MIDDLEMAN SELECTION SYSTEM
// ============================================================================

/**
 * @enum MiddlemanSelectionMethod
 * @brief Defines how a middleman is selected during disputes
 * @note MUTUAL_AGREEMENT is the only truly trust-minimized option
 */
enum class MiddlemanSelectionMethod : uint8_t {
    MUTUAL_AGREEMENT = 0,    // Both parties must agree (most secure)
    RANDOM_FROM_POOL = 1,    // Random selection from pre-approved pool (needs trusted pool)
    REPUTATION_BASED = 2,    // Highest-rated middleman (requires reputation system)
    FIRST_TO_RESPOND = 3,    // First available middleman from pool (fast but risky)
    FALLBACK_ARBITRATOR = 4  // Uses hardcoded fallback (e.g., KYC'd legal service)
};

/**
 * @struct DisputeResolutionConfig
 * @brief Configures how disputes are handled
 * @warning selection_penalty_sats must be >0 to prevent spam
 */
struct DisputeResolutionConfig {
    MiddlemanSelectionMethod selection_method{MiddlemanSelectionMethod::MUTUAL_AGREEMENT};
    std::vector<CPubKey> approved_pool;      // Pre-vetted middlemen pubkeys
    uint64_t selection_penalty_sats{10000};  // Penalty for unreasonable rejection (anti-spam)
    uint32_t selection_timeout_blocks{144};  // ~24h timeout (in blocks, not hours)
    CPubKey fallback_arbitrator;             // Last-resort middleman (required for FALLBACK_ARBITRATOR)
    uint64_t min_middleman_bond_sats{100000}; // Minimum bond required for middlemen
    uint32_t min_reputation_score{50};       // Minimum reputation (0-100) for auto-selection
    FallbackArbitratorRotation rotation_system; // Emergency rotation system for fallback arbitrators
    
    // Validation
    bool IsValid() const {
        if (selection_method == MiddlemanSelectionMethod::FALLBACK_ARBITRATOR && 
            !fallback_arbitrator.IsValid()) {
            return false;
        }
        if (selection_penalty_sats == 0) {
            return false; // Must have penalty to prevent spam
        }
        if (selection_timeout_blocks < 6 || selection_timeout_blocks > 1008) { // 1h to 1 week
            return false;
        }
        if (min_middleman_bond_sats < MMP_MIN_MIDDLEMAN_BOND_SATS) {
            return false;
        }
        if (min_reputation_score > MMP_MAX_REPUTATION_SCORE) {
            return false;
        }
        return true;
    }
    
    // Check if a middleman meets the qualification requirements
    inline bool IsMiddlemanQualified(const MiddlemanInfo& mm) const noexcept {
        return mm.IsValid() && 
               mm.bond_amount_sats >= min_middleman_bond_sats &&
               mm.GetEffectiveReputation() >= min_reputation_score &&
               (selection_method != MiddlemanSelectionMethod::FALLBACK_ARBITRATOR || 
                mm.pubkey == fallback_arbitrator) &&
               mm.is_dao_approved && // Must be DAO approved for security
               ValidateMiddlemanForMethod(mm);
    }
    
    // Check if middleman meets method-specific requirements
    inline bool ValidateMiddlemanForMethod(const MiddlemanInfo& mm) const noexcept {
        switch (selection_method) {
            case MiddlemanSelectionMethod::FALLBACK_ARBITRATOR:
                return mm.is_kyc_verified; // Fallback must be KYC verified
            case MiddlemanSelectionMethod::REPUTATION_BASED:
                return mm.GetEffectiveReputation() >= 80; // Higher threshold for auto-selection
            case MiddlemanSelectionMethod::RANDOM_FROM_POOL:
                return std::find(approved_pool.begin(), approved_pool.end(), mm.pubkey) != approved_pool.end();
            default:
                return true; // MUTUAL_AGREEMENT and FIRST_TO_RESPOND have no special requirements
        }
    }
};

// Middleman functions
JobActionResult AcceptMiddlemanRole(JobContract& contract, const CPubKey& middleman_key);
JobActionResult MiddlemanResolveDispute(JobContract& contract, const CPubKey& middleman_key, 
                                       ResolutionPath resolution, const std::string& reasoning);

// ============================================================================
// MIDDLEMAN FUNCTIONS
// ============================================================================

/**
 * @brief Proposes a middleman for dispute resolution
 * @param contract The job contract
 * @param proposer_key Pubkey of party proposing the middleman
 * @param middleman The proposed middleman info
 * @return JobActionResult::SUCCESS if proposal is valid
 */
JobActionResult ProposeMiddleman(
    JobContract& contract,
    const CPubKey& proposer_key,
    const MiddlemanInfo& middleman
);

/**
 * @brief Accepts/rejects a proposed middleman
 * @param contract The job contract
 * @param acceptor_key Pubkey of party responding
 * @param accept True to accept, false to reject
 * @return JobActionResult::SUCCESS if operation is valid
 */
JobActionResult RespondToMiddlemanProposal(
    JobContract& contract,
    const CPubKey& acceptor_key,
    bool accept
);

/**
 * @brief Automatically selects a middleman based on contract rules
 * @param contract The job contract
 * @return JobActionResult::SUCCESS with selected middleman in contract
 */
JobActionResult AutoSelectMiddleman(JobContract& contract);

/**
 * @brief Middleman resolves a dispute and signs the resolution
 * @param contract The job contract
 * @param middleman_key The middleman's pubkey
 * @param resolution The decision (EMPLOYER_WINS/WORKER_WINS)
 * @param reasoning Signed proof of decision (e.g., "Delivered on time")
 * @param evidence_hash Hash of evidence supporting the decision
 * @return JobActionResult::SUCCESS if resolution is valid
 * 
 * @security This function performs comprehensive validation:
 * - Verifies middleman is authorized for this contract
 * - Checks middleman bond and reputation requirements
 * - Validates resolution reasoning and evidence
 * - Ensures atomic state transitions
 */
JobActionResult MiddlemanResolveDispute(
    JobContract& contract,
    const CPubKey& middleman_key,
    ResolutionPath resolution,
    const std::string& reasoning,
    const std::vector<uint8_t>& evidence_hash = {}
) MMP_STRONG_EXCEPTION_SAFETY;

// Configuration and utility functions
JobActionResult SetDisputeResolutionConfig(JobContract& contract, const DisputeResolutionConfig& config);
std::vector<MiddlemanInfo> GetSuggestedMiddlemen(const JobContract& contract, uint32_t limit = 5);

// Enhanced bond slashing system with appeal process
/**
 * @struct SlashEscrow
 * @brief Holds slashed funds during challenge period
 */
struct SlashEscrow {
    uint256 slash_txid;                 // Transaction ID of the slash escrow
    uint32_t slash_vout;                // Output index in the slash transaction
    uint64_t escrowed_amount_sats;      // Amount held in escrow
    int64_t unlock_timestamp;           // When funds can be released (if no appeal)
    uint32_t unlock_block_height;       // Block height when funds unlock
    bool is_challenged;                 // Whether an appeal was filed
    CPubKey middleman_key;              // Middleman whose funds are escrowed
    uint256 proposal_hash;              // Hash of the original slash proposal
    
    bool IsValid() const noexcept {
        return !slash_txid.IsNull() && 
               escrowed_amount_sats > 0 &&
               unlock_timestamp > 0 &&
               unlock_block_height > 0 &&
               middleman_key.IsValid();
    }
    
    bool IsUnlocked(int64_t current_timestamp, uint32_t current_block_height) const noexcept {
        return !is_challenged && 
               current_timestamp >= unlock_timestamp && 
               current_block_height >= unlock_block_height;
    }
};

struct BondSlashProposal {
    CPubKey middleman_key;              // Middleman to be slashed
    uint64_t slash_amount_sats;         // Amount to slash
    std::string reason;                 // Reason for slashing
    std::vector<uint8_t> evidence_hash; // Hash of evidence
    CPubKey proposer_key;               // Who proposed the slash
    int64_t proposal_timestamp;         // When proposed
    uint32_t challenge_period_blocks;   // Time for appeals (default 1008 blocks = 1 week)
    std::vector<CPubKey> dao_approvers; // DAO members who approved
    bool is_executed;                   // Whether slash was executed
    SlashEscrow escrow_info;            // Escrow details for slashed funds
    
    bool IsValid() const {
        return middleman_key.IsValid() && 
               proposer_key.IsValid() &&
               slash_amount_sats > 0 &&
               !reason.empty() &&
               reason.length() <= MMP_MAX_DISPUTE_REASON_LENGTH &&
               challenge_period_blocks >= 144 && // At least 24h for appeals
               (is_executed ? escrow_info.IsValid() : true); // Escrow must be valid if executed
    }
};

struct BondSlashAppeal {
    uint256 proposal_hash;              // Hash of the slash proposal
    CPubKey appellant_key;              // Who is appealing (usually the middleman)
    std::string appeal_reason;          // Reason for appeal
    std::vector<uint8_t> counter_evidence_hash; // Counter-evidence
    int64_t appeal_timestamp;           // When appeal was filed
    std::vector<CPubKey> dao_reviewers; // DAO members reviewing appeal
    bool is_resolved;                   // Whether appeal was resolved
    bool appeal_successful;             // Whether appeal was granted
    
    bool IsValid() const {
        return appellant_key.IsValid() &&
               !appeal_reason.empty() &&
               appeal_reason.length() <= MMP_MAX_DISPUTE_REASON_LENGTH;
    }
};

/**
 * @struct DAOVote
 * @brief Represents a DAO member's vote with stake-based weighting
 */
struct DAOVote {
    uint256 proposal_hash;              // Hash of the proposal being voted on
    CPubKey voter_key;                  // DAO member's public key
    bool approve;                       // Vote: true = approve, false = reject
    uint8_t vote_weight;                // Vote weight based on stake (1-10)
    int64_t vote_timestamp;             // When the vote was cast
    std::vector<uint8_t> vote_signature; // Signature proving vote authenticity
    std::string vote_reason;            // Optional reason for the vote
    
    bool IsValid() const noexcept {
        return voter_key.IsValid() && 
               vote_weight >= 1 && vote_weight <= 10 &&
               vote_timestamp > 0 &&
               !vote_signature.empty() &&
               vote_reason.length() <= MMP_MAX_DISPUTE_REASON_LENGTH;
    }
    
    // Calculate weighted vote value
    inline double GetWeightedVoteValue() const noexcept {
        return approve ? static_cast<double>(vote_weight) : -static_cast<double>(vote_weight);
    }
};

/**
 * @struct DAOProposal
 * @brief Generic DAO proposal with weighted voting
 */
struct DAOProposal {
    uint256 proposal_hash;              // Unique proposal identifier
    std::string proposal_type;          // Type: "SLASH", "APPEAL", "MIDDLEMAN_APPROVAL", etc.
    std::string proposal_description;   // Human-readable description
    std::vector<uint8_t> proposal_data; // Serialized proposal data
    CPubKey proposer_key;               // Who submitted the proposal
    int64_t proposal_timestamp;         // When proposed
    int64_t voting_deadline;            // Deadline for voting
    std::vector<DAOVote> votes;         // All votes cast
    bool is_executed;                   // Whether proposal was executed
    double approval_threshold;          // Required weighted approval ratio (default 0.6)
    
    bool IsValid() const noexcept {
        return proposer_key.IsValid() &&
               !proposal_type.empty() &&
               !proposal_description.empty() &&
               proposal_timestamp > 0 &&
               voting_deadline > proposal_timestamp &&
               approval_threshold > 0.0 && approval_threshold <= 1.0;
    }
    
    // Calculate current vote tally
    inline std::pair<double, double> GetVoteTally() const noexcept {
        double total_approve = 0.0;
        double total_reject = 0.0;
        
        for (const auto& vote : votes) {
            if (vote.approve) {
                total_approve += static_cast<double>(vote.vote_weight);
            } else {
                total_reject += static_cast<double>(vote.vote_weight);
            }
        }
        
        return {total_approve, total_reject};
    }
    
    // Check if proposal has enough votes to pass
    inline bool HasSufficientApproval() const noexcept {
        auto [approve_weight, reject_weight] = GetVoteTally();
        double total_weight = approve_weight + reject_weight;
        
        if (total_weight == 0.0) return false;
        
        return (approve_weight / total_weight) >= approval_threshold;
    }
    
    // Check if voting period has ended
    inline bool IsVotingClosed(int64_t current_timestamp) const noexcept {
        return current_timestamp >= voting_deadline;
    }
};

// Enhanced bond slashing functions with weighted DAO voting
JobActionResult ProposeBondSlash(const BondSlashProposal& proposal) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult AppealBondSlash(const BondSlashAppeal& appeal) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult ExecuteBondSlash(const uint256& proposal_hash, const std::vector<DAOVote>& dao_votes) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult ResolveBondSlashAppeal(const uint256& appeal_hash, bool grant_appeal, const std::vector<DAOVote>& dao_votes) MMP_STRONG_EXCEPTION_SAFETY;

// DAO governance functions
JobActionResult SubmitDAOProposal(const DAOProposal& proposal) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult CastDAOVote(const DAOVote& vote) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult ExecuteDAOProposal(const uint256& proposal_hash) MMP_STRONG_EXCEPTION_SAFETY;

// Enhanced utility functions
std::vector<MiddlemanInfo> GetDAOApprovedMiddlemen();
MiddlemanInfo* GetFallbackArbitrator();
std::vector<BondSlashProposal> GetPendingSlashProposals();
std::vector<BondSlashAppeal> GetPendingAppeals();
std::vector<SlashEscrow> GetActiveSlashEscrows();
std::vector<DAOProposal> GetActiveDAOProposals();
std::vector<MiddlemanInfo> GetTopPerformingMiddlemen(uint32_t limit = 10);

// Performance tracking functions
JobActionResult UpdateMiddlemanPerformance(const CPubKey& middleman_key, const MiddlemanPerformance& metrics) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult RecordDisputeResolution(const CPubKey& middleman_key, double resolution_time_hours, 
                                       double employer_rating, double worker_rating) MMP_STRONG_EXCEPTION_SAFETY;

// Reputation decay functions
JobActionResult ApplyReputationDecay(const CPubKey& middleman_key, int64_t current_timestamp) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult ApplyReputationDecayToAll(int64_t current_timestamp) MMP_STRONG_EXCEPTION_SAFETY;
uint32_t GetDecayedReputation(const MiddlemanInfo& middleman, int64_t current_timestamp) MMP_NOEXCEPT;

// Enhanced evidence and dispute functions
std::vector<uint8_t> HashEvidence(const std::vector<std::string>& evidence_files) MMP_NOEXCEPT;
JobActionResult SubmitDisputeEvidence(JobContract& contract, const DisputeEvidence& evidence) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult ValidateDisputeTimeout(const JobContract& contract, int64_t current_timestamp) MMP_NOEXCEPT;

// Enhanced middleman selection with criteria
std::vector<MiddlemanInfo> SelectMiddlemenByCriteria(const std::vector<MiddlemanInfo>& candidates,
                                                    const SelectionCriteria& criteria,
                                                    const std::vector<std::string>& required_specialties,
                                                    uint64_t max_fee, uint32_t max_response_time,
                                                    uint32_t limit = 5) MMP_NOEXCEPT;

// Emergency rotation functions
JobActionResult PerformScheduledRotation(FallbackArbitratorRotation& rotation, uint32_t current_block_height) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult PerformEmergencyRotation(FallbackArbitratorRotation& rotation, const CPubKey& failed_arbitrator, 
                                        uint32_t current_block_height) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult AddFallbackArbitrator(FallbackArbitratorRotation& rotation, const CPubKey& new_arbitrator, 
                                     bool dao_approved = false) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult RemoveFallbackArbitrator(FallbackArbitratorRotation& rotation, const CPubKey& arbitrator, 
                                        bool dao_approved = false) MMP_STRONG_EXCEPTION_SAFETY;

// Insurance claims functions
JobActionResult SubmitInsuranceClaim(const InsuranceClaim& claim) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult ApproveInsuranceClaim(const uint256& claim_id, uint64_t approved_amount, 
                                     const CPubKey& approver_key, const std::string& notes = "") MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult RejectInsuranceClaim(const uint256& claim_id, const CPubKey& approver_key, 
                                    const std::string& reason) MMP_STRONG_EXCEPTION_SAFETY;
JobActionResult PayoutInsuranceClaim(const uint256& claim_id, const uint256& payout_txid, 
                                    uint32_t vout) MMP_STRONG_EXCEPTION_SAFETY;
std::vector<InsuranceClaim> GetPendingInsuranceClaims() MMP_NOEXCEPT;
std::vector<InsuranceClaim> GetInsuranceClaimsByStatus(InsuranceClaim::ClaimStatus status) MMP_NOEXCEPT;

// Worker application and assignment functions
JobActionResult ApplyForJob(JobContract& contract, const WorkerApplication& application);
JobActionResult AssignWorker(JobContract& contract, const CPubKey& employer_key, const CPubKey& selected_worker);
std::vector<WorkerApplication> GetJobApplications(const JobContract& contract);
JobActionResult OpenJobForApplications(JobContract& contract);  // Transition from CREATED to OPEN

// Job discovery functions
std::vector<JobContract> ListOpenJobs();  // List all jobs in OPEN state
std::vector<JobContract> ListJobsByState(JobState state);
JobContract* FindJobById(const uint256& job_id);

// Middleman discovery functions
std::vector<MiddlemanInfo> ListAvailableMiddlemen();
std::vector<MiddlemanInfo> SearchMiddlemen(const std::string& specialty = "", double min_reputation = 0.0, 
                                          uint64_t max_fee_sats = UINT64_MAX);
MiddlemanInfo* FindMiddlemanByKey(const CPubKey& middleman_key);
std::vector<MiddlemanInfo> GetTopRatedMiddlemen(uint32_t limit = 10);

// Middleman registration functions
JobActionResult RegisterAsMiddleman(const MiddlemanInfo& middleman_info);
JobActionResult UpdateMiddlemanInfo(const CPubKey& middleman_key, const MiddlemanInfo& updated_info);
JobActionResult DeactivateMiddleman(const CPubKey& middleman_key);

// User-specific job functions
struct UserJobFilter {
    CPubKey user_key;
    bool include_employer_jobs;    // Jobs created by user as employer
    bool include_worker_jobs;      // Jobs assigned to user as worker
    bool include_applied_jobs;     // Jobs user has applied to
    bool include_middleman_jobs;   // Jobs where user is the middleman
    std::vector<JobState> states;  // Filter by specific states (empty = all states)
    
    UserJobFilter(const CPubKey& key) : user_key(key), include_employer_jobs(true), 
                                       include_worker_jobs(true), include_applied_jobs(true),
                                       include_middleman_jobs(true) {}
};

std::vector<JobContract> ListMyJobs(const UserJobFilter& filter);
std::vector<JobContract> ListJobsAsEmployer(const CPubKey& employer_key);
std::vector<JobContract> ListJobsAsWorker(const CPubKey& worker_key);
std::vector<JobContract> ListJobsAppliedTo(const CPubKey& worker_key);
std::vector<JobContract> ListJobsAsMiddleman(const CPubKey& middleman_key);
bool HasUserAppliedToJob(const JobContract& contract, const CPubKey& worker_key);
bool IsUserMiddlemanForJob(const JobContract& contract, const CPubKey& middleman_key);

// Job management functions
JobActionResult CancelJob(JobContract& contract, const CPubKey& employer_key, const std::string& reason);
JobActionResult WithdrawApplication(JobContract& contract, const CPubKey& worker_key);
JobActionResult ExtendJobTimeout(JobContract& contract, const CPubKey& employer_key, uint32_t additional_blocks);
JobActionResult UpdateJobMetadata(JobContract& contract, const CPubKey& employer_key, const std::string& new_metadata_url, const std::string& new_hash);

// Notification and messaging functions
struct JobNotification {
    uint256 job_id;
    CPubKey recipient;
    std::string notification_type;  // "application", "assignment", "completion", "dispute", etc.
    std::string message;
    int64_t timestamp;
    bool read;
    
    std::string ToString() const;
};

std::vector<JobNotification> GetUserNotifications(const CPubKey& user_key, bool unread_only = false);
bool MarkNotificationAsRead(const uint256& job_id, const CPubKey& user_key, const std::string& notification_type);
JobActionResult SendJobMessage(const JobContract& contract, const CPubKey& sender_key, const CPubKey& recipient_key, const std::string& message);

// Statistics and analytics functions
struct JobStatistics {
    uint32_t total_jobs_created;
    uint32_t total_jobs_completed;
    uint32_t total_jobs_disputed;
    uint64_t total_value_locked_sats;
    uint64_t total_value_resolved_sats;
    double average_completion_time_hours;
    double dispute_rate_percentage;
    
    std::string ToString() const;
};

JobStatistics GetGlobalJobStatistics();
JobStatistics GetUserStatistics(const CPubKey& user_key);
std::vector<JobContract> GetRecentJobs(uint32_t limit = 10);

// Advanced search and filtering
struct JobSearchFilter {
    std::vector<JobState> states;
    uint64_t min_amount_sats;
    uint64_t max_amount_sats;
    uint32_t max_timeout_blocks;
    std::string keyword_search;        // Search in metadata URL/description
    CPubKey employer_filter;           // Filter by specific employer
    CPubKey middleman_filter;          // Filter by specific middleman
    bool exclude_expired;
    uint32_t created_after_height;     // Only jobs created after this block height
    uint32_t limit;                    // Max results to return
    
    JobSearchFilter() : min_amount_sats(0), max_amount_sats(UINT64_MAX), 
                       max_timeout_blocks(UINT32_MAX), exclude_expired(true), 
                       created_after_height(0), limit(100) {}
};

std::vector<JobContract> SearchJobs(const JobSearchFilter& filter);
std::vector<JobContract> GetJobsByAmountRange(uint64_t min_sats, uint64_t max_sats);
std::vector<JobContract> GetJobsByTimeout(uint32_t max_timeout_blocks);
std::vector<JobContract> GetExpiredJobs(uint32_t current_height);

// Reputation and rating system
struct UserRating {
    CPubKey user_key;
    double average_rating;        // 0.0 to 5.0
    uint32_t total_ratings;
    uint32_t jobs_completed;
    uint32_t jobs_disputed;
    double completion_rate;       // Percentage of jobs completed successfully
    double dispute_rate;          // Percentage of jobs that went to dispute
    int64_t last_activity_timestamp;
    
    std::string ToString() const;
};

struct JobRating {
    uint256 job_id;
    CPubKey rater_key;           // Who gave the rating
    CPubKey rated_user_key;      // Who was rated
    uint8_t rating;              // 1-5 stars
    std::string review_text;     // Optional review text
    int64_t timestamp;
    
    bool IsValid() const;
    std::string ToString() const;
};

UserRating GetUserRating(const CPubKey& user_key);
std::vector<JobRating> GetUserReviews(const CPubKey& user_key, uint32_t limit = 10);
JobActionResult SubmitJobRating(const JobContract& contract, const CPubKey& rater_key, 
                               const CPubKey& rated_user_key, uint8_t rating, const std::string& review);
std::vector<UserRating> GetTopRatedUsers(bool employers_only = false, uint32_t limit = 10);

// ============================================================================
// TRANSACTION BUILDING
// ============================================================================
CTransaction BuildCooperativeResolution(const JobContract& contract, const CScript& destination);
CTransaction BuildDisputeResolution(const JobContract& contract, ResolutionPath path, const CScript& destination);
CTransaction BuildTimeoutRefund(const JobContract& contract);

// ============================================================================
// VALIDATION FUNCTIONS
// ============================================================================
bool ValidateJobContract(const JobContract& contract);
std::pair<bool, MMPError> ValidateJobContractEx(const JobContract& contract);
bool ValidateResolutionTransaction(const CTransaction& tx, const JobContract& contract);
bool IsTimeoutExpired(const JobContract& contract, uint32_t current_height);
bool HasMinimumConfirmations(const uint256& txid, uint32_t required_confirmations);

// ============================================================================
// TAPROOT CONTROL BLOCK OPERATIONS
// ============================================================================
std::vector<unsigned char> BuildControlBlock(const KeyAggregationContext& ctx, const CScript& leaf_script);
bool ValidateControlBlock(const std::vector<unsigned char>& control_block, const KeyAggregationContext& ctx);
/**
 * @struct TaprootWitness
 * @brief Type-safe container for Taproot witness data
 * @var control_block Control block for script path spending (33 + 32n bytes)
 * @var witness_stack Stack of witness elements (each ≤ 520 bytes)
 * 
 * @note IsValid() performs the following checks:
 *       - Control block size: 33 + 32n bytes (1 key + n hashes)
 *       - Each witness element ≤ 520 bytes (Bitcoin limit)
 *       - Witness stack size ≤ 100 elements (reasonable limit)
 */
struct TaprootWitness {
    std::vector<unsigned char> control_block;
    std::vector<std::vector<unsigned char>> witness_stack;
    
    /**
     * @brief Validates Taproot witness structure
     * @return true if witness is valid according to Bitcoin protocol rules
     * @note Thread-safe validation using Bitcoin protocol limits
     */
    bool IsValid() const MMP_THREAD_SAFE;
    
    /**
     * @brief Estimates serialized size of witness
     * @return Estimated size in bytes for transaction weight calculation
     */
    size_t EstimateSize() const MMP_THREAD_SAFE;
    
    /**
     * @brief Returns human-readable representation
     * @return String representation for debugging and logging
     */
    std::string ToString() const MMP_THREAD_SAFE;
};

TaprootWitness BuildTaprootWitness(const KeyAggregationContext& ctx, const CScript& script, 
                                  const std::vector<std::vector<unsigned char>>& stack);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================
std::string JobStateToString(JobState state);
std::string ResolutionPathToString(ResolutionPath path);
uint256 ComputeJobId(const CPubKey& employer, const CPubKey& worker, const std::string& metadata_hash, 
                    uint32_t nonce, uint32_t timestamp = 0);
bool VerifyMetadataHash(const std::string& metadata_content, const std::string& expected_hash);

// Validation utilities
bool IsValidURL(const std::string& url);
bool IsValidSHA256Hash(const std::string& hash);
bool IsValidJobAmount(uint64_t amount_sats);
bool IsValidTimeout(uint32_t timeout_blocks);
bool IsValidBlockHeight(uint32_t height);
bool IsValidJobId(const uint256& job_id);

// New validation utilities for applications and messages
bool IsValidApplicationMessage(const std::string& message);
bool IsValidContactInfo(const std::string& contact_info);
bool IsValidJobMessage(const std::string& message);
bool IsValidCancelReason(const std::string& reason);
bool IsValidTimeoutExtension(uint32_t extension_blocks);
bool IsValidWorkerApplication(const WorkerApplication& application);
bool IsValidJobNotification(const JobNotification& notification);
bool IsValidRating(uint8_t rating);
bool IsValidReviewText(const std::string& review_text);
bool IsValidJobRating(const JobRating& rating);

// Middleman and dispute resolution validation utilities
bool IsValidMiddlemanInfo(const MiddlemanInfo& middleman_info);
bool IsValidMiddlemanName(const std::string& name);
bool IsValidSpecialty(const std::string& specialty);
bool IsValidDisputeResolutionConfig(const DisputeResolutionConfig& config);
bool IsValidMiddlemanSelectionMethod(MiddlemanSelectionMethod method);
bool IsValidDisputeReason(const std::string& reason);

// Security utilities
bool IsSecureURL(const std::string& url);  // Check for HTTPS/IPFS
bool HasValidURLScheme(const std::string& url);
size_t EstimateMetadataSize(const JobMetadata& metadata);
bool IsWithinSizeLimit(const JobMetadata& metadata);

// Version compatibility with robust checking
bool IsCompatibleVersion(uint8_t version);
bool IsMinimumVersion(uint8_t version, uint8_t min_version);
bool IsMaximumVersion(uint8_t version, uint8_t max_version);
uint8_t GetCurrentVersion() MMP_THREAD_SAFE;
uint8_t GetMinimumCompatibleVersion() MMP_THREAD_SAFE;
uint8_t GetMaximumCompatibleVersion() MMP_THREAD_SAFE;
std::string GetVersionString(uint8_t version) MMP_THREAD_SAFE;

// ============================================================================
// STATE MANAGEMENT UTILITIES
// ============================================================================
std::vector<ResolutionPath> GetAvailableResolutionPaths(const JobContract& contract);
bool IsAllowedStateTransition(JobState from, JobState to);
StateTransitionResult TransitionJobState(JobContract& contract, JobState new_state, 
                                        const std::string& memo = "");

// ============================================================================
// SECURITY UTILITIES
// ============================================================================
bool IsReplay(const JobContract& contract, const CTransaction& tx);
bool ValidateNonce(uint32_t nonce, uint32_t timestamp);
/**
 * @fn GenerateSecureNonce
 * @brief Generates a cryptographically secure nonce
 * @return 32-bit nonce suitable for replay protection
 * @note Uses system entropy sources and is thread-safe
 * @warning Must not be used for cryptographic key material
 */
uint32_t GenerateSecureNonce();

// Serialization versioning and checksums
struct SerializationHeader {
    char tag[3];        // "MMP" (exactly 3 bytes, no null terminator)
    uint8_t version;    // Protocol version
    uint32_t checksum;  // CRC32 of data
    uint32_t length;    // Data length
};

bool ValidateSerializationHeader(const SerializationHeader& header, size_t data_size);
uint32_t ComputeSerializationChecksum(const std::vector<unsigned char>& data);

// Memory safety utilities
bool ValidateVectorSizes(const std::vector<std::vector<unsigned char>>& witness_stack);
bool ValidateBufferSize(const std::vector<unsigned char>& buffer, size_t max_size);
bool ValidateStringLength(const std::string& str, size_t max_length);

// ============================================================================
// ERROR HANDLING UTILITIES
// ============================================================================
std::string MMPErrorToString(MMPError error) MMP_THREAD_SAFE;
std::string LockFundsResultToString(LockFundsResult result) MMP_THREAD_SAFE;
std::string StateTransitionResultToString(StateTransitionResult result) MMP_THREAD_SAFE;
std::string JobActionResultToString(JobActionResult result) MMP_THREAD_SAFE;
const char* MMPErrorToCString(MMPError error) MMP_THREAD_SAFE;

// Error categorization
bool IsProtocolError(MMPError error) MMP_THREAD_SAFE;
bool IsValidationError(MMPError error) MMP_THREAD_SAFE;
bool IsFatalError(MMPError error) MMP_THREAD_SAFE;
bool IsRecoverableError(MMPError error) MMP_THREAD_SAFE;

// Thread-local error state management
MMPError GetLastError(); // Thread-local error state
void SetLastError(MMPError error);
void ClearLastError();

// ============================================================================
// DEBUG AND TESTING UTILITIES
// ============================================================================
#ifdef MMP_DEBUG
bool EnableFuzzTesting(bool enable);
bool IsFuzzTestingEnabled();
void SetDebugValidationMode(bool strict);

// Internal consistency validation
void ValidateInternalConsistency(const JobContract& contract);
void ValidateInternalConsistency(const KeyAggregationContext& ctx);
void ValidateInternalConsistency(const JobMetadata& metadata);

// Debug state dumping
void DumpContractState(const JobContract& contract);
void DumpKeyAggregationState(const KeyAggregationContext& ctx);
void DumpMetadataState(const JobMetadata& metadata);

// Fuzz testing integration points
void FuzzJobMetadata(JobMetadata& metadata);
void FuzzKeyAggregation(KeyAggregationContext& ctx);
void FuzzJobContract(JobContract& contract);
#endif

} // namespace mmp

#endif // GOTHAM_MMP_H