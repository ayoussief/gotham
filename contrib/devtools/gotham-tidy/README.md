# Gotham Tidy

Example Usage:

```bash
cmake -S . -B build -DLLVM_DIR=$(llvm-config --cmakedir) -DCMAKE_BUILD_TYPE=Release

cmake --build build -j$(nproc)

cmake --build build --target gotham-tidy-tests -j$(nproc)
```
