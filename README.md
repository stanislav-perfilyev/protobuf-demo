# protobuf-demo

C++20 demo: serialise and deserialise company records with Protocol Buffers (proto3).

## Architecture

- `proto/company.proto` — data schema (name, address, founded_year, industry, has_foreign_trade)
- `include/company_repository.h` / `src/company_repository.cpp` — repository with typed exceptions (`ValidationError`, `ProtoError`), validate / serialize / deserialize / roundtrip / add / all
- `src/main.cpp` — CLI demo
- `tests/test_company.cpp` — 14 GTest cases (roundtrip, serialization, validation, repository)

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Requirements

- CMake ≥ 3.15
- C++20 compiler (GCC 12+, Clang 15+)
- Protocol Buffers ≥ 3.x (`libprotobuf-dev protobuf-compiler` on Ubuntu)
- Google Test — fetched automatically via CMake FetchContent
