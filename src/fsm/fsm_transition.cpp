#include "fsm_transition.hpp"

using namespace LDJ;

// run on any bindings
auto FsmTransition::WhenAny(std::function<FsmTransitionResult()> fnGetNext) -> FsmTransition * {
  transitions.push_back(TransitionLogic(MODE_ANY, {}, fnGetNext));
  return this;
}

// run on specified bindings
auto FsmTransition::When(std::initializer_list<std::string> bindings, std::function<FsmTransitionResult()> fnGetNext)
  -> FsmTransition * {
  transitions.emplace_back(MODE_MATCH, bindings, fnGetNext);
  return this;
}
auto FsmTransition::When(std::vector<std::string> bindings, std::function<FsmTransitionResult()> fnGetNext)
  -> FsmTransition * {
  transitions.emplace_back(MODE_MATCH, bindings, fnGetNext);
  return this;
}
auto FsmTransition::When(std::string binding, std::function<FsmTransitionResult()> fnGetNext) -> FsmTransition * {
  return When({binding}, fnGetNext);
}

// run on not specified binding
auto FsmTransition::WhenNot(std::initializer_list<std::string> bindings, std::function<FsmTransitionResult()> fnGetNext)
  -> FsmTransition * {
  transitions.emplace_back(MODE_NOT_MATCH, bindings, fnGetNext);
  return this;
}
auto FsmTransition::WhenNot(std::vector<std::string> bindings, std::function<FsmTransitionResult()> fnGetNext)
  -> FsmTransition * {
  transitions.emplace_back(MODE_NOT_MATCH, bindings, fnGetNext);
  return this;
}
auto FsmTransition::WhenNot(std::string binding, std::function<FsmTransitionResult()> fnGetNext) -> FsmTransition * {
  return WhenNot(std::vector{binding}, fnGetNext);
}

auto FsmTransition::RunTransitionLogic(std::string currentBinding) -> std::optional<FsmTransitionResult> {
  // do binding
  if (immediateBinding != "") {
    std::string result = immediateBinding;
    immediateBinding = "";
    return result;
  }

  bool nextFound = false;
  for (auto transition : transitions) {
    // check mode
    switch (transition.mode) {
    case MODE_MATCH:
      if (!transition.bindings.contains(currentBinding))
        continue;
      break;
    case MODE_NOT_MATCH:
      if (transition.bindings.contains(currentBinding))
        continue;
    }
    nextFound = true;
    // return result
    FsmTransitionResult result = transition.fnGetNext();
    if (result.index() == 0 || (result.index() == 1 && std::get<1>(result) != nullptr)) {
      return result;
    }
  }

  if (nextFound)
    return nullptr;

  return std::nullopt;
}

auto FsmTransition::DoAction(std::string binding) -> FsmTransition * {
  // TODO: check if binding is possible
  immediateBinding = binding;
  return this;
}
