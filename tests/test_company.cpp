#include <gtest/gtest.h>
#include "company_repository.h"
#include "company.pb.h"

namespace {

company::Company makeValid()
{
    company::Company c;
    c.set_founded_year(2000);
    c.set_legal_address("Moscow, Russia");
    c.set_name("Test Corp");
    return c;
}

} // namespace

// ── Roundtrip ─────────────────────────────────────────────────────────────

TEST(CompanyRoundtrip, RequiredFieldsSurvive)
{
    auto c = makeValid();
    c.set_industry("Software");
    c.set_has_foreign_trade(true);
    EXPECT_TRUE(CompanyRepository::roundtrip(c));
}

TEST(CompanyRoundtrip, MinimalFieldsSurvive)
{
    EXPECT_TRUE(CompanyRepository::roundtrip(makeValid()));
}

// ── Serialize / Deserialize ───────────────────────────────────────────────

TEST(CompanySerialization, SerializedBytesNonEmpty)
{
    const std::string bytes = CompanyRepository::serialize(makeValid());
    EXPECT_FALSE(bytes.empty());
}

TEST(CompanySerialization, DeserializeRestoresName)
{
    auto c = makeValid();
    c.set_name("Yandex LLC");
    const auto rt = CompanyRepository::deserialize(CompanyRepository::serialize(c));
    EXPECT_EQ(rt.name(), "Yandex LLC");
}

TEST(CompanySerialization, DeserializeRestoresFoundedYear)
{
    auto c = makeValid();
    c.set_founded_year(1998);
    const auto rt = CompanyRepository::deserialize(CompanyRepository::serialize(c));
    EXPECT_EQ(rt.founded_year(), 1998);
}

TEST(CompanySerialization, DeserializeRestoresOptionalFields)
{
    auto c = makeValid();
    c.set_industry("Banking");
    c.set_has_foreign_trade(true);
    const auto rt = CompanyRepository::deserialize(CompanyRepository::serialize(c));
    EXPECT_EQ(rt.industry(), "Banking");
    EXPECT_TRUE(rt.has_foreign_trade());
}

TEST(CompanySerialization, MalformedDataThrowsProtoError)
{
    EXPECT_THROW(CompanyRepository::deserialize("not-valid-protobuf-data"), ProtoError);
}

// ── Validation ────────────────────────────────────────────────────────────

TEST(CompanyValidation, ValidCompanyDoesNotThrow)
{
    EXPECT_NO_THROW(CompanyRepository::validate(makeValid()));
}

TEST(CompanyValidation, EmptyNameThrows)
{
    auto c = makeValid();
    c.set_name("");
    EXPECT_THROW(CompanyRepository::validate(c), ValidationError);
}

TEST(CompanyValidation, EmptyAddressThrows)
{
    auto c = makeValid();
    c.set_legal_address("");
    EXPECT_THROW(CompanyRepository::validate(c), ValidationError);
}

TEST(CompanyValidation, FoundedYearTooEarlyThrows)
{
    auto c = makeValid();
    c.set_founded_year(1799);
    EXPECT_THROW(CompanyRepository::validate(c), ValidationError);
}

TEST(CompanyValidation, FoundedYearTooLateThrows)
{
    auto c = makeValid();
    c.set_founded_year(2101);
    EXPECT_THROW(CompanyRepository::validate(c), ValidationError);
}

TEST(CompanyValidation, ValidationErrorIsProtoError)
{
    auto c = makeValid();
    c.set_name("");
    EXPECT_THROW(CompanyRepository::validate(c), ProtoError);  // base class catchable
}

// ── Repository ────────────────────────────────────────────────────────────

TEST(CompanyRepository, EmptyOnConstruction)
{
    CompanyRepository repo;
    EXPECT_EQ(repo.size(), 0u);
}

TEST(CompanyRepository, AddIncreasesSize)
{
    CompanyRepository repo;
    repo.add(makeValid());
    EXPECT_EQ(repo.size(), 1u);
    repo.add(makeValid());
    EXPECT_EQ(repo.size(), 2u);
}

TEST(CompanyRepository, AddInvalidThrows)
{
    CompanyRepository repo;
    auto c = makeValid();
    c.set_name("");
    EXPECT_THROW(repo.add(c), ValidationError);
    EXPECT_EQ(repo.size(), 0u);  // rollback: nothing added
}

TEST(CompanyRepository, AllReturnsStoredCompanies)
{
    CompanyRepository repo;
    auto c = makeValid();
    c.set_name("Alpha");
    repo.add(c);
    ASSERT_EQ(repo.all().size(), 1u);
    EXPECT_EQ(repo.all()[0].name(), "Alpha");
}
