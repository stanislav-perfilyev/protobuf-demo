#include "company_repository.h"
#include "company.pb.h"

#include <sstream>
#include <stdexcept>

// ── Serialization ──────────────────────────────────────────────────────────

std::string CompanyRepository::serialize(const company::Company& c)
{
    std::string out;
    if (!c.SerializeToString(&out))
        throw ProtoError("Failed to serialize Company '" + c.name() + "'");
    return out;
}

company::Company CompanyRepository::deserialize(const std::string& data)
{
    company::Company c;
    if (!c.ParseFromString(data))
        throw ProtoError("Failed to parse Company from binary data ("
                         + std::to_string(data.size()) + " bytes)");
    return c;
}

// ── Validation ─────────────────────────────────────────────────────────────

void CompanyRepository::validate(const company::Company& c)
{
    if (c.name().empty())
        throw ValidationError("Company name must not be empty");
    if (c.legal_address().empty())
        throw ValidationError("Company legal_address must not be empty");
    if (c.founded_year() < 1800 || c.founded_year() > 2100) {
        throw ValidationError(
            "founded_year " + std::to_string(c.founded_year())
            + " is out of range [1800, 2100]");
    }
}

// ── Round-trip ─────────────────────────────────────────────────────────────

bool CompanyRepository::roundtrip(const company::Company& original)
{
    const std::string bytes   = serialize(original);
    const company::Company rt = deserialize(bytes);

    return rt.name()          == original.name()
        && rt.legal_address() == original.legal_address()
        && rt.founded_year()  == original.founded_year()
        && rt.industry()      == original.industry()
        && rt.has_foreign_trade() == original.has_foreign_trade();
}

// ── Repository ─────────────────────────────────────────────────────────────

void CompanyRepository::add(const company::Company& c)
{
    validate(c);
    store_.push_back(c);
}
