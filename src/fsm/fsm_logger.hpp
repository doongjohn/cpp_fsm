#pragma once

#include <iostream>
#include <string>

#ifndef LDJ_FSM_LOGGER
#define LDJ_FSM_LOGGER ConsoleLogger
#endif

namespace LDJ {

struct ConsoleLogger {
  static auto Log(std::string msg, std::string prefix) -> void {
    std::cout << prefix << msg << '\n';
  }
};

template <typename Logger>
inline auto fsm_log_template(std::string msg, std::string prefix) {
  Logger::Log(msg, prefix);
}

inline auto fsm_log(std::string msg, std::string prefix = "[FSM] ") {
  fsm_log_template<LDJ_FSM_LOGGER>(msg, prefix);
}

} // namespace LDJ
