#pragma once

#include <chrono>
#include <iostream>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cout << #x ": " << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::steady_clock::now() - bench_##x).count() << "ms" << std::endl;
#define TOCKN(x, n) std::cout << #x ": " << std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::steady_clock::now() - bench_##x).count() / n << "ms" << std::endl;