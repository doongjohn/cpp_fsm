#include "fsm_transition.hpp"

using namespace LDJ;

// transition to specified action after transition
auto FsmTransition::DoAction(std::string binding) -> FsmTransition * {
  immediate_binding = binding;
  return this;
}

// run on any bindings
auto FsmTransition::WhenAny(std::function<FsmTransitionResult()> fn_get_next) -> FsmTransition * {
  transition_logics.push_back(TransitionLogic(MODE_ANY, {}, fn_get_next));
  return this;
}

// run on specified bindings
auto FsmTransition::When(std::initializer_list<std::string> bindings, std::function<FsmTransitionResult()> fn_get_next)
  -> FsmTransition * {
  transition_logics.emplace_back(MODE_MATCH, bindings, fn_get_next);
  return this;
}
auto FsmTransition::When(std::vector<std::string> bindings, std::function<FsmTransitionResult()> fn_get_next)
  -> FsmTransition * {
  transition_logics.emplace_back(MODE_MATCH, bindings, fn_get_next);
  return this;
}
auto FsmTransition::When(std::string binding, std::function<FsmTransitionResult()> fn_get_next) -> FsmTransition * {
  return When({binding}, fn_get_next);
}

// run on not specified binding
auto FsmTransition::WhenNot(std::initializer_list<std::string> bindings,
                            std::function<FsmTransitionResult()> fn_get_next) -> FsmTransition * {
  transition_logics.emplace_back(MODE_NOT_MATCH, bindings, fn_get_next);
  return this;
}
auto FsmTransition::WhenNot(std::vector<std::string> bindings, std::function<FsmTransitionResult()> fn_get_next)
  -> FsmTransition * {
  transition_logics.emplace_back(MODE_NOT_MATCH, bindings, fn_get_next);
  return this;
}
auto FsmTransition::WhenNot(std::string binding, std::function<FsmTransitionResult()> fn_get_next) -> FsmTransition * {
  return WhenNot(std::vector{binding}, fn_get_next);
}

auto FsmTransition::RunTransitionLogic(std::string current_binding) -> std::optional<FsmTransitionResult> {
  // do binding
  if (immediate_binding != "") {
    std::string result = immediate_binding;
    immediate_binding = "";
    return result;
  }

  bool nextFound = false;
  for (auto &transition : transition_logics) {
    // check mode
    switch (transition.mode) {
    case MODE_MATCH:
      if (!transition.bindings.contains(current_binding))
        continue;
      break;
    case MODE_NOT_MATCH:
      if (transition.bindings.contains(current_binding))
        continue;
    }
    nextFound = true;

    // return result
    FsmTransitionResult result = transition.fn_get_next();
    if (result.index() == 0 || (result.index() == 1 && std::get<1>(result) != nullptr)) {
      return result;
    }
  }

  if (nextFound) {
    return nullptr;
  }

  return std::nullopt;
}
