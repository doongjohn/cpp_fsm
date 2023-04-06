#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <variant>

namespace LDJ {

using FsmTransitionResult = std::variant<std::string, class FsmTransition *>;
constexpr class FsmTransition *FsmContinue = nullptr;
//                string : transition to next binding
//            Transition : transition to next transition
// FsmContinue (nullptr) : continue run next transition logic

class FsmTransition {
  static constexpr const int MODE_ANY = 0;
  static constexpr const int MODE_MATCH = 1;
  static constexpr const int MODE_NOT_MATCH = 2;

  struct TransitionLogic {
    int mode;
    std::unordered_set<std::string> bindings;
    std::function<FsmTransitionResult()> fn_get_next;

  public:
    TransitionLogic(int mode, std::initializer_list<std::string> bindings,
                    std::function<FsmTransitionResult()> fn_get_next)
        : mode(mode), bindings(bindings), fn_get_next(std::move(fn_get_next)) {}

    TransitionLogic(int mode, std::vector<std::string> bindings, std::function<FsmTransitionResult()> fn_get_next)
        : mode(mode), bindings(bindings.begin(), bindings.end()), fn_get_next(std::move(fn_get_next)) {}
  };

  std::string name;
  std::string immediate_binding;
  std::vector<TransitionLogic> transition_logics;

public:
  FsmTransition(std::string name) : name(std::move(name)) {}

  // Copy constructor
  FsmTransition(const FsmTransition &other) {
    name = other.name;
    immediate_binding = other.immediate_binding;
    transition_logics = other.transition_logics;
  }

  inline auto GetName() -> std::string {
    return name;
  }

  // transition to specified action after transition
  auto DoAction(std::string binding) -> FsmTransition *;

  // run on any bindings
  auto WhenAny(std::function<FsmTransitionResult()> fn_get_next) -> FsmTransition *;

  // run on specified bindings
  auto When(std::initializer_list<std::string> bindings, const std::function<FsmTransitionResult()> &fn_get_next)
    -> FsmTransition *;
  auto When(const std::vector<std::string> &bindings, const std::function<FsmTransitionResult()> &fn_get_next)
    -> FsmTransition *;
  auto When(std::string binding, const std::function<FsmTransitionResult()> &fn_get_next) -> FsmTransition *;

  // run on not specified binding
  auto WhenNot(std::initializer_list<std::string> bindings, const std::function<FsmTransitionResult()> &fn_get_next)
    -> FsmTransition *;
  auto WhenNot(const std::vector<std::string> &bindings, const std::function<FsmTransitionResult()> &fn_get_next)
    -> FsmTransition *;
  auto WhenNot(std::string binding, const std::function<FsmTransitionResult()> &fn_get_next) -> FsmTransition *;

  // for internal use only
  auto RunTransitionLogic(const std::string &current_binding) -> std::optional<FsmTransitionResult>;
};

} // namespace LDJ
