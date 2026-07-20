#pragma once

#include "company.pb.h"

#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Serialization / deserialization errors.
 */
class ProtoError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @brief Validation errors (required fields missing or out of range).
 */
class ValidationError : public ProtoError {
    using ProtoError::ProtoError;
};

/**
 * @brief In-memory repository of Company protobuf messages.
 *
 * Demonstrates how to wrap generated protobuf code in a clean C++ API:
 *   - serialize()   → binary wire format
 *   - deserialize() → Company message
 *   - validate()    → business-rule checks on required fields
 *   - roundtrip()   → self-test: serialize then deserialize, compare fields
 *
 * All methods returning data are marked [[nodiscard]].
 */
class CompanyRepository {
public:
    CompanyRepository() = default;

    /** @brief Minimum valid founded_year (inclusive). */
    static constexpr int kMinFoundedYear = 1800;
    /** @brief Maximum valid founded_year (inclusive). */
    static constexpr int kMaxFoundedYear = 2100;

    /**
     * @brief Serialize a Company to binary wire format.
     * @param company The Company message to serialize.
     * @return Binary serialized bytes.
     * @throws ProtoError if serialization fails.
     */
    [[nodiscard]] static std::string serialize(const company::Company& company);

    /**
     * @brief Deserialize binary data into a Company message.
     * @param data Binary wire-format bytes.
     * @return Parsed Company message.
     * @throws ProtoError if the data is malformed.
     */
    [[nodiscard]] static company::Company deserialize(const std::string& data);

    /**
     * @brief Validate required business rules on a Company.
     * @param company The Company to validate.
     * @throws ValidationError if any rule is violated.
     *
     * Rules:
     *   - name must not be empty
     *   - legal_address must not be empty
     *   - founded_year must be in [1800, 2100]
     */
    static void validate(const company::Company& company);

    /**
     * @brief Serialize then immediately deserialize; verify field equality.
     * @param company The Company to round-trip.
     * @return true if all required fields survive the round-trip unchanged.
     * @throws ProtoError on serialization/deserialization failure.
     */
    [[nodiscard]] static bool roundtrip(const company::Company& company);

    /**
     * @brief Add a validated company to the in-memory store.
     * @throws ValidationError if validation fails.
     */
    void add(const company::Company& company);

    /** @brief Number of companies in the store. */
    [[nodiscard]] std::size_t size() const noexcept { return store_.size(); }

    /** @brief Retrieve all stored companies. */
    [[nodiscard]] const std::vector<company::Company>& all() const noexcept { return store_; }

private:
    std::vector<company::Company> store_;
};
