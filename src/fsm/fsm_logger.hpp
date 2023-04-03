#pragma once

#include <functional>
#include <iostream>
#include <string>

namespace LDJ {

struct FsmLogger {
  static std::function<void(std::string, std::string)> fn_logger;
};

inline std::function<void(std::string, std::string)> FsmLogger::fn_logger = nullptr;

inline auto fsm_log(std::string msg, std::string prefix = "[FSM] ") -> void {
  if (FsmLogger::fn_logger) {
    FsmLogger::fn_logger(msg, prefix);
  } else {
    std::cout << prefix << msg << '\n';
  }
}

} // namespace LDJ
