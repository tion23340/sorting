FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

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
# final image
# -------------------------------------------------------
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# libgomp1 = OpenMP runtime (libgomp.so)
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libgomp1 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/CPP /app/CPP

ENTRYPOINT ["/app/CPP"]
