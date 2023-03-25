#pragma once

#include <cassert>
#include <iostream>
#include <string>

namespace LDJ {

inline auto assert_msg(bool expr, std::string msg) -> void {
  if (!expr)
    std::cout << "Assertion failed: " << msg << '\n';
  assert(expr);
}

} // namespace LDJ
