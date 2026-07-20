/**
 * protobuf-demo/src/main.cpp
 *
 * Demonstrates serialization / deserialization of a Company protobuf message
 * using the CompanyRepository wrapper class.
 *
 * Build:  cmake -B build && cmake --build build
 * Run:    ./build/protobuf_demo
 */

#include "company_repository.h"

#include <google/protobuf/stubs/common.h>
#include <iostream>
#include <string>

static void print_company(const char* label, const company::Company& c)
{
    std::cout << "\n=== " << label << " ===\n"
              << "  name:             " << c.name()             << '\n'
              << "  legal_address:    " << c.legal_address()    << '\n'
              << "  founded_year:     " << c.founded_year()     << '\n'
              << "  industry:         " << c.industry()         << '\n'
              << "  has_foreign_trade:" << std::boolalpha
                                        << c.has_foreign_trade() << '\n';
}

static company::Company make_yandex()
{
    constexpr int kFoundedYear = 2000;
    company::Company c;
    c.set_name("Yandex LLC");
    c.set_legal_address("Moscow, Russia");
    c.set_founded_year(kFoundedYear);
    c.set_industry("Internet / Technology");
    c.set_has_foreign_trade(true);
    return c;
}

static company::Company make_sber()
{
    constexpr int kFoundedYear = 1991;
    company::Company c;
    c.set_name("Sberbank PJSC");
    c.set_legal_address("Moscow, Vavilova 19");
    c.set_founded_year(kFoundedYear);
    c.set_industry("Banking");
    return c;
}

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const company::Company yandex = make_yandex();
    print_company("Original", yandex);

    try {
        CompanyRepository::validate(yandex);
        std::cout << "\n[validate]  OK\n";
    } catch (const ValidationError& e) {
        std::cerr << "[validate]  FAIL: " << e.what() << '\n';
        return 1;
    }

    const std::string bytes = CompanyRepository::serialize(yandex);
    std::cout << "[serialize] " << bytes.size() << " bytes\n";

    const company::Company loaded = CompanyRepository::deserialize(bytes);
    print_company("Deserialized", loaded);

    const bool ok = CompanyRepository::roundtrip(yandex);
    std::cout << "\n[roundtrip] " << (ok ? "PASS" : "FAIL") << '\n';

    CompanyRepository repo;
    repo.add(yandex);
    repo.add(make_sber());

    std::cout << "\n[repository] " << repo.size() << " company(ies) stored\n";
    for (const auto& c : repo.all())
        std::cout << "  - " << c.name() << " (" << c.founded_year() << ")\n";

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
