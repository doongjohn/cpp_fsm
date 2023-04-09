#pragma once

#include <iostream>
#include <string>

#include "fsm_logger.hpp"

namespace LDJ {

struct FsmAssert {
  inline static std::function<void(bool, std::string)> fn_assert = nullptr;
};

inline auto fsm_assert_msg(bool expr, const std::string &msg) -> void {
  if (!expr) {
    fsm_log("Assertion failed: " + msg);
    if (FsmAssert::fn_assert) {
      FsmAssert::fn_assert(expr, msg);
    } else {
      throw msg;
    }
  }
}

} // namespace LDJ
