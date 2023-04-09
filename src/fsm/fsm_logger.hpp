#pragma once

#include <functional>
#include <iostream>
#include <string>

namespace LDJ {

struct FsmLogger {
  inline static std::function<void(std::string, std::string)> fn_logger = nullptr;
};

inline auto fsm_log(const std::string &msg, const std::string &prefix = "[FSM] ") -> void {
  if (FsmLogger::fn_logger) {
    FsmLogger::fn_logger(msg, prefix);
  } else {
    std::cout << prefix << msg << '\n';
  }
}

} // namespace LDJ
