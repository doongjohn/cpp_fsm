#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>

namespace LDJ {

class FsmTransition;
using FsmTransitionResult = std::variant<std::string, FsmTransition *>;
// - string     : next binding name
// - Transition : next transition
// - nullptr    : check bellow
constexpr FsmTransition *FsmContinue = nullptr;

class FsmTransition {
  static constexpr const int MODE_ANY = 0;
  static constexpr const int MODE_MATCH = 1;
  static constexpr const int MODE_NOT_MATCH = 2;

  struct TransitionLogic {
    int mode;
    std::unordered_set<std::string> bindings;
    std::function<FsmTransitionResult()> fnGetNext;

  public:
    TransitionLogic(int mode, std::initializer_list<std::string> bindings,
                    std::function<FsmTransitionResult()> fnGetNext)
        : mode(mode), bindings(bindings), fnGetNext(std::move(fnGetNext)) {}
  };
  std::string name;
  std::vector<TransitionLogic> transitions;
  std::string doBinding;

public:
  FsmTransition(std::string name) : name(std::move(name)) {}

  inline auto GetName() -> std::string {
    return name;
  }

  // run on any bindings
  auto WhenAny(std::function<FsmTransitionResult()> fnGetNext) -> FsmTransition *;

  // run on specified bindings
  auto When(std::initializer_list<std::string> bindings, std::function<FsmTransitionResult()> fnGetNext)
    -> FsmTransition *;
  auto When(std::string binding, std::function<FsmTransitionResult()> fnGetNext) -> FsmTransition *;

  // run on not specified binding
  auto WhenNot(std::initializer_list<std::string> bindings, std::function<FsmTransitionResult()> fnGetNext)
    -> FsmTransition *;
  auto WhenNot(std::string binding, std::function<FsmTransitionResult()> fnGetNext) -> FsmTransition *;

  // for internal use only
  auto RunTransitionLogic(std::string currentBinding) -> std::optional<FsmTransitionResult>;

  // TODO: comment
  auto Do(std::string binding) -> FsmTransition *;
};

} // namespace LDJ
