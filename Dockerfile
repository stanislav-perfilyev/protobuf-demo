# Build stage
FROM ubuntu:24.04 AS builder

RUN apt-get update -q && \
    apt-get install -y --no-install-recommends \
        cmake \
        g++-14 \
        libprotobuf-dev \
        protobuf-compiler \
        ca-certificates \
        git \
    && rm -rf /var/lib/apt/lists/*

ENV CC=gcc-14 CXX=g++-14

WORKDIR /src
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --parallel "$(nproc)"

# Test stage
FROM builder AS test
RUN ctest --test-dir build --output-on-failure

# Runtime stage
FROM ubuntu:24.04 AS runtime

RUN apt-get update -q && \
    apt-get install -y --no-install-recommends \
        libprotobuf32t64 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /src/build/protobuf_demo /usr/local/bin/protobuf_demo

ENTRYPOINT ["/usr/local/bin/protobuf_demo"]
