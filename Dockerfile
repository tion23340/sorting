FROM alpine:3.19 AS builder

# install build dependencies
RUN apk add --no-cache \
    cmake \
    make \
    g++ \
    gcc \
    git \
    linux-headers

# build google benchmark from source
RUN git clone --branch v1.8.3 --depth 1 https://github.com/google/benchmark.git /benchmark
RUN cmake -S /benchmark -B /benchmark/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON \
        -DBENCHMARK_ENABLE_TESTING=OFF
RUN cmake --build /benchmark/build -j$(nproc)
RUN cmake --install /benchmark/build

# build project
WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)

# -------------------------------------------------------
# final image — copy only the binary
# -------------------------------------------------------
FROM alpine:3.19

RUN apk add --no-cache libstdc++

COPY --from=builder /app/build/CPP /app/CPP

ENTRYPOINT ["/app/CPP"]