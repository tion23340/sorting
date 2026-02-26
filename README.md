# Sorting Algorithms Benchmark

This project benchmarks multiple sorting algorithms across different data types and configurations.
All benchmarks are built and executed inside a Docker container to ensure portability, consistency, and ease of setup.

---

## Requirements

- Docker (must be installed and running)
- Git
- Linux / WSL / macOS

---

## Setup

IMPORTANT: All benchmarks must be run from the `benchmark` branch.

Command:
git checkout benchmark

The Docker image is used only to simplify and standardize the CMake build process (including Google Benchmark).
No manual dependency installation is required on the host system.

---

## Running Benchmarks

All benchmarks are launched via the `RUNNING.SH` script.

### Run everything

./RUNNING.SH

---

### Run by algorithm

./RUNNING.SH QuickSort  
./RUNNING.SH BubbleSort  
./RUNNING.SH MergeSort  
./RUNNING.SH RadixSort  
./RUNNING.SH BitonicSort  
./RUNNING.SH QuickSortParallel  

---

### Run by data type

./RUNNING.SH IntSigned  
./RUNNING.SH IntUnsigned  
./RUNNING.SH FloatSigned  
./RUNNING.SH FloatUnsigned  
./RUNNING.SH DoubleSigned  
./RUNNING.SH DoubleUnsigned  

---

### Run by data category

./RUNNING.SH Signed      # all signed (int, float, double)  
./RUNNING.SH Unsigned    # all unsigned  
./RUNNING.SH Int         # all int benchmarks  
./RUNNING.SH Float       # all float benchmarks  
./RUNNING.SH Double      # all double benchmarks  

---

### Run a specific benchmark

./RUNNING.SH QuickSort_IntSigned  
./RUNNING.SH MergeSort_DoubleSigned  

---

### Run multiple algorithms

./RUNNING.SH "QuickSort|MergeSort"  
./RUNNING.SH "RadixSort|BitonicSort"  

---

### Algorithm + type combinations (regex)

./RUNNING.SH "QuickSort.*IntSigned"  
./RUNNING.SH "MergeSort.*Double"  
./RUNNING.SH ".*Parallel.*Signed"  

---

## Docker Notes

- Uses a multi-stage Docker build
- Builder stage installs CMake, builds Google Benchmark, and compiles the project
- Final image contains only runtime dependencies (including OpenMP)
- Docker is used strictly for portability and build consistency
- Ensures identical benchmarking conditions across all systems
