#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include "fsm_logger.hpp"

namespace LDJ {

struct FsmAssert {
  static std::function<void(bool)> fn_assert;
};

inline std::function<void(bool)> FsmAssert::fn_assert = nullptr;

inline auto fsm_assert_msg(bool expr, const std::string& msg) -> void {
  if (!expr)
    fsm_log("Assertion failed: " + msg);

  if (FsmAssert::fn_assert) {
    FsmAssert::fn_assert(expr);
  } else {
    assert(expr);
  }
}

} // namespace LDJ
