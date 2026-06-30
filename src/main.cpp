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
#include "company.pb.h"

#include <google/protobuf/stubs/common.h>
#include <iostream>
#include <string>

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // ── Build a Company message ────────────────────────────────────────────
    company::Company yandex;
    yandex.set_founded_year(2000);
    yandex.set_legal_address("Moscow, Russia");
    yandex.set_name("Yandex LLC");
    yandex.set_industry("Internet / Technology");
    yandex.set_has_foreign_trade(true);

    std::cout << "=== Original ===\n"
              << "  name:             " << yandex.name()          << '\n'
              << "  legal_address:    " << yandex.legal_address() << '\n'
              << "  founded_year:     " << yandex.founded_year()  << '\n'
              << "  industry:         " << yandex.industry()       << '\n'
              << "  has_foreign_trade:" << std::boolalpha
                                        << yandex.has_foreign_trade() << '\n';

    // ── Validate ───────────────────────────────────────────────────────────
    try {
        CompanyRepository::validate(yandex);
        std::cout << "\n[validate]  OK\n";
    } catch (const ValidationError& e) {
        std::cerr << "[validate]  FAIL: " << e.what() << '\n';
        return 1;
    }

    // ── Serialize ──────────────────────────────────────────────────────────
    const std::string bytes = CompanyRepository::serialize(yandex);
    std::cout << "[serialize] " << bytes.size() << " bytes\n";

    // ── Deserialize ────────────────────────────────────────────────────────
    const company::Company loaded = CompanyRepository::deserialize(bytes);
    std::cout << "\n=== Deserialized ===\n"
              << "  name:          " << loaded.name()          << '\n'
              << "  legal_address: " << loaded.legal_address() << '\n'
              << "  founded_year:  " << loaded.founded_year()  << '\n';

    // ── Round-trip check ───────────────────────────────────────────────────
    const bool ok = CompanyRepository::roundtrip(yandex);
    std::cout << "\n[roundtrip] " << (ok ? "PASS" : "FAIL") << '\n';

    // ── Repository ─────────────────────────────────────────────────────────
    CompanyRepository repo;
    repo.add(yandex);

    company::Company sber;
    sber.set_founded_year(1991);
    sber.set_legal_address("Moscow, Vavilova 19");
    sber.set_name("Sberbank PJSC");
    sber.set_industry("Banking");
    repo.add(sber);

    std::cout << "\n[repository] " << repo.size() << " company(ies) stored\n";
    for (const auto& c : repo.all())
        std::cout << "  - " << c.name() << " (" << c.founded_year() << ")\n";

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
