#pragma once

#ifdef _MSC_VER
// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4458
#pragma warning(disable : 4458)
#pragma warning(disable : 4800)
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

enum FsmActionStatus {
  Running,
  Completed,
  Break,
};
struct FsmActionDuration {
  float duration;
};
using FsmActionResult = std::variant<FsmActionStatus, FsmActionDuration>;

template <typename State>
class FsmAction {
public:
  State *state;
  std::vector<State *> extras;
  float timer;
  std::function<FsmActionResult()> fn_result;

  FsmAction() = default;
  FsmAction(State *state, std::initializer_list<State *> extras, std::function<FsmActionResult()> fn_result = nullptr);
  FsmAction(State *state, std::vector<State *> extras, std::function<FsmActionResult()> fn_result = nullptr);

  // Copy constructor
  FsmAction(const FsmAction &other);

  auto operator=(const FsmAction &other) -> FsmAction &;

  template <typename T>
  auto OnEnter(T &owner) -> void {
    state->OnEnter(owner);
    for (auto &ex : extras) {
      ex->OnEnter(owner);
    }
  }

  template <typename T>
  auto OnExit(T &owner) -> void {
    state->OnExit(owner);
    for (auto &ex : extras) {
      ex->OnExit(owner);
    }
    timer = 0;
  }

  template <typename T>
  auto OnUpdate(T &owner) -> void {
    state->OnUpdate(owner);
    for (auto &ex : extras) {
      ex->OnUpdate(owner);
    }
  }
};

template <typename State>
FsmAction<State>::FsmAction(State *state, std::initializer_list<State *> extras,
                            std::function<FsmActionResult()> fn_result)
    : state(state), extras(std::move(extras)), timer(0), fn_result(std::move(fn_result)) {
  fsm_assert_msg(state, "State is null!");
  for (auto &ex : extras) {
    fsm_assert_msg(ex, "State (ex) is null!");
  }
}

template <typename State>
FsmAction<State>::FsmAction(State *state, std::vector<State *> extras, std::function<FsmActionResult()> fn_result)
    : state(state), extras(std::move(extras)), timer(0), fn_result(std::move(fn_result)) {
  fsm_assert_msg(state, "State is null!");
  for (auto &ex : extras) {
    fsm_assert_msg(ex, "State (ex) is null!");
  }
}

template <typename State>
FsmAction<State>::FsmAction(const FsmAction &other)
    : state(other.state), extras(other.extras), timer(other.timer), fn_result(other.fn_result) {
  fsm_assert_msg(state, "State is null!");
  for (auto &ex : extras) {
    fsm_assert_msg(ex, "State (ex) is null!");
  }
}

template <typename State>
auto FsmAction<State>::operator=(const FsmAction &other) -> FsmAction & {
  if (this != &other) {
    state = other.state;
    extras = other.extras;
    timer = other.timer;
    fn_result = other.fn_result;
  }
  return *this;
}

template <typename T, typename State>
class Fsm {
public:
  T *owner;
  std::unordered_map<std::string, std::vector<FsmAction<State>>> actions;
  std::string current_binding;
  std::string previous_binding;

  State *current_state;
  FsmAction<State> current_action;
  FsmTransition *current_transition = nullptr;

  bool b_print_log = false;
  std::function<void()> fn_err_excessive_transition = nullptr;
  std::function<void()> fn_err_no_possible_transition = nullptr;

  Fsm() = default;
  Fsm(T *owner);
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
  auto NewTransition(std::string name) -> FsmTransition *;

  auto BindDefault(FsmTransition *default_transition, FsmAction<State> default_action) -> Fsm<T, State> *;
  auto Bind(std::string name, FsmAction<State> action) -> Fsm<T, State> *;
  auto Bind(std::string name, std::initializer_list<FsmAction<State>> actions) -> Fsm<T, State> *;
  auto Bind(std::string name, std::vector<FsmAction<State>> actions) -> Fsm<T, State> *;

  auto Reenter(std::string binding) -> std::string;
  auto SkipCurrent(std::string binding) -> std::string;
  auto SkipCurrent(FsmTransition *transition) -> FsmTransition *;

  auto FsmStart() -> void;
  auto FsmUpdate(float delta_time) -> void;
  auto Update() -> void;

  auto GetTimer() -> float &;
};

template <typename T, typename State>
Fsm<T, State>::Fsm(T *owner) : owner(owner) {}

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
  b_skip_waiting = true;
  return binding;
}

template <typename T, typename State>
auto Fsm<T, State>::SkipCurrent(FsmTransition *transition) -> FsmTransition * {
  b_skip_waiting = true;
  return transition;
}

template <typename T, typename State>
auto Fsm<T, State>::FsmStart() -> void {
  fsm_assert_msg(current_transition != nullptr, "Must call the BindDefault() function before starting.");
  fsm_assert_msg(actions.contains("default"), "Must call the BindDefault() function before starting.");
  if (b_print_log)
    fsm_log("action enter: " + current_binding);
  current_state = current_action.state;
  current_action.OnEnter(owner);
}

template <typename T, typename State>
auto Fsm<T, State>::FsmUpdate(float delta_time) -> void {
  // increment action timer
  float current_timer = current_action.timer;
  current_action.timer += delta_time;

  // do transition
  cur_transition_count = 0;
  bool b_check_next_transition = true;
  while (b_check_next_transition) {
    b_check_next_transition = false;

    // check excessive transition
    cur_transition_count += 1;
    if (cur_transition_count > max_transition_count) {
      // print trace
      if (b_print_log) {
        fsm_log("transition trace: (from most recent)");
        for (const auto &trace : transition_trace)
          fsm_log(trace, "");
      }

      if (fn_err_excessive_transition) {
        // run error callback
        fn_err_excessive_transition();
      } else {
        fsm_assert_msg(false, "Excessive transition detected.");
      }
      return;
    }

    // update transition trace
    transition_trace.push_front(current_transition->GetName() + " : \"" + current_binding + "\"");
    if ((int)transition_trace.size() > max_transition_count)
      transition_trace.pop_back();

    // get next transition result
    std::optional<FsmTransitionResult> opt_next = current_transition->RunTransitionLogic(current_binding);
    if (!opt_next.has_value()) {
      if (fn_err_no_possible_transition) {
        if (b_print_log)
          fsm_log("No transition is possible from the current binding : \"" + current_binding + "\"");
        // run error callback
        fn_err_no_possible_transition();
      } else {
        fsm_assert_msg(false, "No transition is possible from the current binding : \"" + current_binding + "\"");
      }
      return;
    }
    FsmTransitionResult next = opt_next.value();

    // get current action status
    std::optional<FsmActionStatus> action_status = std::nullopt;
    if (current_action.fn_result != nullptr) {
      auto action_result = current_action.fn_result();
      switch (action_result.index()) {
      case 0: // <-- FsmActionStatus
        action_status = std::get<0>(action_result);
        break;
      case 1: // <-- FsmActionDuration
        float action_dur = std::max(std::get<1>(action_result).duration, 0.f);
        action_status = current_timer >= action_dur ? Completed : Running;
        break;
      }
    }

    // check current action is ended
    if (action_status != Running)
      b_is_waiting = false;

    // skip waiting
    if (b_skip_waiting) {
      b_skip_waiting = false;
      b_is_waiting = false;
      current_action_list_index = 0;
      current_action_list = std::nullopt;
    }

    // wait for an action to end
    if (b_is_waiting)
      return;

    if (current_action_list && action_status.has_value()) {
      // sequenced action
      if (current_action_list.value().size() > 1) {
        bool b_is_sequence_has_next_state = current_action_list.value().size() > current_action_list_index + 1;
        if (action_status == Completed && b_is_sequence_has_next_state) {
          current_action.OnExit(owner);

          // run next action
          current_action_list_index += 1;
          current_action = current_action_list.value()[current_action_list_index];
          b_is_waiting = true;

          if (b_print_log)
            fsm_log("action enter seq[" + std::to_string(current_action_list_index) + "]: " + current_binding);

          current_state = current_action.state;
          current_action.OnEnter(owner);
          return;
        }

        if (action_status != Running) {
          // sequence ended
          b_is_waiting = false;
          current_action_list_index = 0;
          current_action_list = std::nullopt;
          b_check_next_transition = true;
          continue;
        }
      }
    }

    switch (next.index()) {
    case 0: { // <-- string
      // get next binding
      std::string next_binding = std::get<0>(next);
      if (!actions.contains(next_binding)) {
        fsm_assert_msg(false, "Unbound action: " + current_transition->GetName() + " -> \"" + next_binding + "\"");
        return;
      }

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

        if (b_print_log) {
          if (current_action_list.value().size() > 1) {
            fsm_log("action enter seq[0]: " + current_binding);
          } else {
            fsm_log("action enter: " + current_binding);
          }
        }

        current_state = current_action.state;
        current_action.OnEnter(owner);
      }
    } break;
    case 1: { // <-- FsmTransition *
      // get next transition
      FsmTransition *transition = std::get<1>(next);
      // `transition == nullptr` means `FsmContinue`
      if (transition) {
        // update transition trace
        transition_trace.front() += " -> " + transition->GetName();

        // change transition
        current_transition = transition;
        // check next transition
        b_check_next_transition = true;
      }
    } break;
    }
  }
}

template <typename T, typename State>
auto Fsm<T, State>::Update() -> void {
  current_action.OnUpdate(owner);
}

template <typename T, typename State>
auto Fsm<T, State>::GetTimer() -> float & {
  return current_action.timer;
}

} // namespace LDJ
