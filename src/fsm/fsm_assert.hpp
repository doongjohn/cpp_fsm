#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include "fsm_logger.hpp"

namespace LDJ {

inline auto fsm_assert_msg(bool expr, std::string msg) -> void {
  if (!expr)
    fsm_log("Assertion failed: " + msg);
  assert(expr);
}

} // namespace LDJ
