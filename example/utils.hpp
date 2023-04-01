#pragma once
#include <random>

inline auto random_range(int min, int max) -> int {
  static std::random_device dev;
  static std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
  return static_cast<int>(dist(rng));
}
