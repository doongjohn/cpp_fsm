#pragma once

#ifdef _MSC_VER
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4458
#pragma warning(disable : 4458)
#endif

#include <functional>
#include <iostream>
#include <list>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

#include "fsm_assert.hpp"
#include "fsm_transition.hpp"

namespace LDJ {

enum FsmActionResult {
  Running,
  Completed,
  Break,
};

template <typename State>
class FsmAction {
public:
  State state;
  std::vector<State> extras;
  std::function<FsmActionResult()> fn_result;

  FsmAction() = default;
  FsmAction(State state, std::initializer_list<State> extras, std::function<FsmActionResult()> fn_result = nullptr);
  FsmAction(State state, std::vector<State> extras, std::function<FsmActionResult()> fn_result = nullptr);

  // Copy constructor
  FsmAction(const FsmAction &other);

  auto operator=(const FsmAction &other) -> FsmAction &;

  template <typename T>
  auto OnEnter(T &owner) -> void {
    if constexpr (std::is_pointer_v<State>) {
      state->OnEnter(owner);
      for (auto &e : extras)
        e->OnEnter(owner);
    } else {
      state.OnEnter(owner);
      for (auto &e : extras)
        e.OnEnter(owner);
    }
  }

  template <typename T>
  auto OnExit(T &owner) -> void {
    if constexpr (std::is_pointer_v<State>) {
      state->OnExit(owner);
      for (auto &e : extras)
        e->OnExit(owner);
    } else {
      state.OnExit(owner);
      for (auto &e : extras)
        e.OnExit(owner);
    }
  }

  template <typename T>
  auto OnUpdate(T &owner) -> void {
    if constexpr (std::is_pointer_v<State>) {
      state->OnUpdate(owner);
      for (auto &e : extras)
        e->OnUpdate(owner);
    } else {
      state.OnUpdate(owner);
      for (auto &e : extras)
        e.OnUpdate(owner);
    }
  }
};

template <typename State>
FsmAction<State>::FsmAction(State state, std::initializer_list<State> extras,
                            std::function<FsmActionResult()> fn_result)
    : state(state), extras(std::move(extras)), fn_result(std::move(fn_result)) {}

template <typename State>
FsmAction<State>::FsmAction(State state, std::vector<State> extras, std::function<FsmActionResult()> fn_result)
    : state(state), extras(std::move(extras)), fn_result(std::move(fn_result)) {}

template <typename State>
FsmAction<State>::FsmAction(const FsmAction &other)
    : state(other.state), extras(other.extras), fn_result(other.fn_result) {}

template <typename State>
auto FsmAction<State>::operator=(const FsmAction &other) -> FsmAction & {
  if (this != &other) {
    state = other.state;
    extras = other.extras;
    fn_result = other.fn_result;
  }
  return *this;
}

template <typename T, typename State>
class Fsm {
public:
  T owner;
  std::unordered_map<std::string, std::vector<FsmAction<State>>> actions;
  std::string current_binding;
  std::string previous_binding;

  FsmAction<State> current_action;
  FsmTransition *current_transition = nullptr;

  std::function<void()> fn_on_action_change = nullptr;
  std::function<void()> fn_err_excessive_transition = nullptr;
  std::function<void()> fn_err_no_possible_transition = nullptr;

  Fsm() = default;
  Fsm(T owner);
  ~Fsm();

private:
  bool b_reenter = false;
  bool b_skip_waiting = false;
  bool b_is_waiting = false;

  size_t current_action_list_index = 0;
  std::optional<std::span<FsmAction<State>>> current_action_list = std::nullopt;
  std::vector<FsmTransition *> transitions;

  // NOTE: these are for debugging
  const int max_transition_count = 50;
  int cur_transition_count = 0;
  std::list<std::string> transition_trace;

public:
  bool print_log = false;

  auto NewTransition(std::string name) -> FsmTransition *;

  auto BindDefault(FsmTransition *default_transition, FsmAction<State> default_action) -> Fsm<T, State> *;
  auto Bind(std::string name, FsmAction<State> action) -> Fsm<T, State> *;
  auto Bind(std::string name, std::initializer_list<FsmAction<State>> actions) -> Fsm<T, State> *;
  auto Bind(std::string name, std::vector<FsmAction<State>> actions) -> Fsm<T, State> *;

  auto Reenter(std::string binding) -> std::string;
  auto SkipCurrent(std::string binding) -> std::string;
  auto SkipCurrent(FsmTransition *transition) -> FsmTransition *;

  auto FsmStart() -> void;
  auto FsmUpdate() -> void;
  auto Update() -> void;
};

template <typename T, typename State>
Fsm<T, State>::Fsm(T owner) : owner(owner) {}

template <typename T, typename State>
Fsm<T, State>::~Fsm() {
  for (auto t : transitions) {
    delete t;
  }
}

template <typename T, typename State>
auto Fsm<T, State>::NewTransition(std::string name) -> FsmTransition * {
  auto transition_ptr = new FsmTransition(std::move(name));
  transitions.push_back(transition_ptr);
  return transition_ptr;
}

template <typename T, typename State>
auto Fsm<T, State>::BindDefault(FsmTransition *default_transition, FsmAction<State> default_action) -> Fsm<T, State> * {
  fsm_assert_msg(default_transition != nullptr, "Default transition must not be null.");
  current_transition = default_transition;

  fsm_assert_msg(default_action.fn_result == nullptr, "Default action must not end.");
  actions["default"] = std::vector{default_action};
  current_binding = "default";
  current_action = default_action;
  return this;
}

template <typename T, typename State>
auto Fsm<T, State>::Bind(std::string name, FsmAction<State> action) -> Fsm<T, State> * {
  fsm_assert_msg(!actions.contains(name), "Cannot bind to a name that is already bound.");
  actions[name] = std::vector{action};
  return this;
}

template <typename T, typename State>
auto Fsm<T, State>::Bind(std::string name, std::initializer_list<FsmAction<State>> actions) -> Fsm<T, State> * {
  fsm_assert_msg(!this->actions.contains(name), "Cannot bind to a name that is already bound.");
  fsm_assert_msg(actions.size() > 1, "Must contain more than 2 actions.");
  for (auto &action : actions)
    fsm_assert_msg(action.fn_result != nullptr, "Sequenced action must have fn_result function.");

  this->actions[name] = std::vector(actions);
  return this;
}

template <typename T, typename State>
auto Fsm<T, State>::Bind(std::string name, std::vector<FsmAction<State>> actions) -> Fsm<T, State> * {
  fsm_assert_msg(!this->actions.contains(name), "Cannot bind to a name that is already bound.");
  fsm_assert_msg(actions.size() > 1, "Must contain more than 2 actions.");
  for (auto &action : actions)
    fsm_assert_msg(action.fn_result != nullptr, "Sequenced action must have fn_result function.");

  this->actions[name] = std::vector(actions);
  return this;
}

template <typename T, typename State>
auto Fsm<T, State>::Reenter(std::string binding) -> std::string {
  b_reenter = true;
  return binding;
}

template <typename T, typename State>
auto Fsm<T, State>::SkipCurrent(std::string binding) -> std::string {
  b_is_waiting = false;
  b_skip_waiting = true;
  return binding;
}

template <typename T, typename State>
auto Fsm<T, State>::SkipCurrent(FsmTransition *transition) -> FsmTransition * {
  b_is_waiting = false;
  b_skip_waiting = true;
  return transition;
}

template <typename T, typename State>
auto Fsm<T, State>::FsmStart() -> void {
  fsm_assert_msg(current_transition != nullptr, "Must call the BindDefault() function before starting.");
  fsm_assert_msg(actions.contains("default"), "Must call the BindDefault() function before starting.");
  if (print_log)
    fsm_log("action enter: " + current_binding);
  current_action.OnEnter(owner);
}

template <typename T, typename State>
auto Fsm<T, State>::FsmUpdate() -> void {
  cur_transition_count = 0;
  bool b_check_next_transition = true;
  while (b_check_next_transition) {
    // check transition count
    cur_transition_count += 1;
    if (cur_transition_count > max_transition_count) {
      // run error callback
      if (fn_err_excessive_transition)
        fn_err_excessive_transition();

      // print trace
      if (print_log) {
        fsm_log("transition trace: (from most recent)");
        for (const auto &trace : transition_trace)
          fsm_log(trace, "");
      }

      fsm_assert_msg(false, "Excessive transition detected.");
      return;
    }

    // update transition trace
    transition_trace.push_front(current_transition->GetName() + " : \"" + current_binding + "\"");
    if ((int)transition_trace.size() > max_transition_count)
      transition_trace.pop_back();

    // get current action result
    std::optional<FsmActionResult> action_result;
    if (current_action.fn_result != nullptr)
      action_result = current_action.fn_result();

    // check current action is ended
    if (action_result.has_value() && action_result != Running)
      b_is_waiting = false;

    // get next transition result
    std::optional<FsmTransitionResult> opt_next = current_transition->RunTransitionLogic(current_binding);
    if (!opt_next.has_value()) {
      // run error callback
      if (fn_err_no_possible_transition)
        fn_err_no_possible_transition();

      fsm_assert_msg(false, "No transition is possible from the current binding : \"" + current_binding + "\"");
    }
    auto next = opt_next.value();

    // wait for an action to end
    if (b_is_waiting)
      return;

    // sequenced action
    if (!b_skip_waiting && current_action_list && current_action.fn_result != nullptr) {
      if (action_result == Completed) {
        current_action.OnExit(owner);

        if (current_action_list.value().size() > current_action_list_index + 1) {
          // run next action
          b_is_waiting = true;
          current_action_list_index += 1;
          current_action = current_action_list.value()[current_action_list_index];

          if (print_log)
            fsm_log("action enter seq[" + std::to_string(current_action_list_index) + "]: " + current_binding);
          current_action.OnEnter(owner);
          return;
        }
      }

      if (action_result != Running) {
        // sequence ended
        b_is_waiting = false;
        current_action_list_index = 0;
        current_action_list = std::nullopt;
        continue;
      }
    }

    if (b_skip_waiting) {
      b_skip_waiting = false;
      current_action_list_index = 0;
      current_action_list = std::nullopt;
    }

    switch (next.index()) {
    case 0: {
      b_check_next_transition = false;

      // get next binding
      std::string next_binding = std::get<0>(next);
      if (!actions.contains(next_binding))
        fsm_assert_msg(false, "Unbound action: \"" + next_binding + "\"");

      if (b_reenter || current_binding != next_binding) {
        b_reenter = false;

        previous_binding = current_binding;
        current_binding = next_binding;
        current_action.OnExit(owner);

        current_action_list = actions[next_binding];
        current_action_list_index = 0;
        current_action = current_action_list.value()[0];

        // wait for the action to end
        if (current_action.fn_result != nullptr)
          b_is_waiting = true;

        if (print_log) {
          if (current_action_list.value().size() > 1) {
            fsm_log("action enter seq[0]: " + current_binding);
          } else {
            fsm_log("action enter: " + current_binding);
          }
        }
        current_action.OnEnter(owner);

        if (fn_on_action_change)
          fn_on_action_change();
      }
    } break;
    case 1: {
      // get next transition
      FsmTransition *transition = std::get<1>(next);
      if (transition) {
        // change transition
        current_transition = transition;
        // update transition trace
        transition_trace.front() += " -> " + current_transition->GetName();
      } else {
        // FsmContinue
        b_check_next_transition = false;
      }
    } break;
    }
  }
}

template <typename T, typename State>
auto Fsm<T, State>::Update() -> void {
  current_action.OnUpdate(owner);
}

} // namespace LDJ
