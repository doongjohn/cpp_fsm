#pragma once

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

template <typename State>
class FsmAction {
public:
  State state;
  std::vector<State> extras;
  std::function<bool()> fnIsEnded;

  FsmAction() = default;
  FsmAction(State state, std::initializer_list<State> extras, std::function<bool()> fnIsEnded = nullptr);
  FsmAction(State state, std::vector<State> extras, std::function<bool()> fnIsEnded = nullptr);

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
FsmAction<State>::FsmAction(State state, std::initializer_list<State> extras, std::function<bool()> fnIsEnded)
    : state(state), extras(std::move(extras)), fnIsEnded(std::move(fnIsEnded)) {}

template <typename State>
FsmAction<State>::FsmAction(State state, std::vector<State> extras, std::function<bool()> fnIsEnded)
    : state(state), extras(std::move(extras)), fnIsEnded(std::move(fnIsEnded)) {}

template <typename State>
FsmAction<State>::FsmAction(const FsmAction &other)
    : state(other.state), extras(other.extras), fnIsEnded(other.fnIsEnded) {}

template <typename State>
auto FsmAction<State>::operator=(const FsmAction &other) -> FsmAction & {
  if (this != &other) {
    state = other.state;
    extras = other.extras;
    fnIsEnded = other.fnIsEnded;
  }
  return *this;
}

template <typename T, typename State>
class Fsm {
public:
  T owner;
  std::unordered_map<std::string, std::vector<FsmAction<State>>> actions;
  std::string currentBinding;
  std::string previousBinding;

  std::function<void()> fnErrOnExcessiveTransition = nullptr;
  std::function<void()> fnErrOnNoPossibleTransition = nullptr;

private:
  bool bReenterTransition = false;
  bool bSkipWaiting = false;
  bool bIsWaitingForActionEnd = false;

  FsmAction<State> currentAction;
  size_t currentActionListIndex = 0;
  std::optional<std::span<FsmAction<State>>> currentActionList = std::nullopt;

  FsmTransition *currentTransition = nullptr;

  // NOTE: these are for debugging
  const int maxTransitionCount = 50;
  int curTransitionCount = 0;
  std::list<std::string> transitionTrace;

public:
  Fsm() = default;
  Fsm(FsmTransition *initialTransition);

  // Copy constructor
  Fsm(const Fsm &other);

  auto operator=(const Fsm &other) -> Fsm &;

  auto Init(T owner) -> void;

  auto BindDefault(FsmAction<State> action) -> Fsm<T, State> *;
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
Fsm<T, State>::Fsm(FsmTransition *initialTransition) : owner(owner), currentTransition(initialTransition) {}

template <typename T, typename State>
Fsm<T, State>::Fsm(const Fsm<T, State> &other) {
  std::cout << "Fsm copied\n";
  owner = other.owner;
  actions = other.actions;
  currentBinding = other.currentBinding;
  previousBinding = other.previousBinding;
  currentAction = other.currentAction;
  currentTransition = other.currentTransition;
  // NOTE: does this copy?
  fnErrOnExcessiveTransition = other.fnErrOnExcessiveTransition;
  fnErrOnNoPossibleTransition = other.fnErrOnNoPossibleTransition;
}

template <typename T, typename State>
auto Fsm<T, State>::operator=(const Fsm &other) -> Fsm & {
  if (this != &other) {
    owner = other.owner;
    actions = other.actions;
    currentBinding = other.currentBinding;
    previousBinding = other.previousBinding;
    currentAction = other.currentAction;
    currentTransition = other.currentTransition;
    // NOTE: does this copy?
    fnErrOnExcessiveTransition = other.fnErrOnExcessiveTransition;
    fnErrOnNoPossibleTransition = other.fnErrOnNoPossibleTransition;
  }
  return *this;
}

template <typename T, typename State>
auto Fsm<T, State>::Init(T owner) -> void {
  this->owner = owner;
}

template <typename T, typename State>
auto Fsm<T, State>::BindDefault(FsmAction<State> action) -> Fsm<T, State> * {
  fsm_assert_msg(action.fnIsEnded == nullptr, "Default action must not end.");
  actions["default"] = std::vector{action};
  currentBinding = "default";
  currentAction = action;
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
    fsm_assert_msg(action.fnIsEnded != nullptr, "Sequenced action must have fnIsEnded function.");

  this->actions[name] = std::vector(actions);
  return this;
}

template <typename T, typename State>
auto Fsm<T, State>::Bind(std::string name, std::vector<FsmAction<State>> actions) -> Fsm<T, State> * {
  fsm_assert_msg(!this->actions.contains(name), "Cannot bind to a name that is already bound.");
  fsm_assert_msg(actions.size() > 1, "Must contain more than 2 actions.");
  for (auto &action : actions)
    fsm_assert_msg(action.fnIsEnded != nullptr, "Sequenced action must have fnIsEnded function.");

  this->actions[name] = std::vector(actions);
  return this;
}

template <typename T, typename State>
auto Fsm<T, State>::Reenter(std::string binding) -> std::string {
  bReenterTransition = true;
  return binding;
}

template <typename T, typename State>
auto Fsm<T, State>::SkipCurrent(std::string binding) -> std::string {
  bIsWaitingForActionEnd = false;
  bSkipWaiting = true;
  return binding;
}

template <typename T, typename State>
auto Fsm<T, State>::SkipCurrent(FsmTransition *transition) -> FsmTransition * {
  bIsWaitingForActionEnd = false;
  bSkipWaiting = true;
  return transition;
}

template <typename T, typename State>
auto Fsm<T, State>::FsmStart() -> void {
  fsm_assert_msg(actions.contains("default"), "Must call the BindDefault() function before starting.");
  std::cout << "[FSM] action enter: " << currentBinding << '\n';
  currentAction.OnEnter(owner);
}

template <typename T, typename State>
auto Fsm<T, State>::FsmUpdate() -> void {
  // std::cout << "FsmUpdate\n";

  // check current action is ended
  if (currentAction.fnIsEnded != nullptr && currentAction.fnIsEnded())
    bIsWaitingForActionEnd = false;

  curTransitionCount = 0;
  bool bCheckNextTransition = true;
  while (bCheckNextTransition) {
    // check transition count
    curTransitionCount += 1;
    if (curTransitionCount > maxTransitionCount) {
      // run error callback
      if (fnErrOnExcessiveTransition)
        fnErrOnExcessiveTransition();

      // print trace
      std::cout << "[FSM] transition trace: (from most recent)\n";
      for (auto e : transitionTrace)
        std::cout << e << '\n';

      fsm_assert_msg(false, "Excessive transition detected.");
      return;
    }

    // update transition trace
    transitionTrace.push_front(currentTransition->GetName() + " : \"" + currentBinding + "\"");
    if ((int)transitionTrace.size() > maxTransitionCount)
      transitionTrace.pop_back();

    // get next transition result
    std::optional<FsmTransitionResult> optionalNext = currentTransition->RunTransitionLogic(currentBinding);
    if (!optionalNext.has_value()) {
      // run error callback
      if (fnErrOnNoPossibleTransition)
        fnErrOnNoPossibleTransition();

      fsm_assert_msg(false, "No transition is possible from the current binding : \"" + currentBinding + "\"");
    }
    auto next = optionalNext.value();

    // wait for an action to end
    if (bIsWaitingForActionEnd)
      return;

    // sequenced action
    if (!bSkipWaiting && currentActionList && currentAction.fnIsEnded != nullptr && currentAction.fnIsEnded()) {
      // std::cout << "[FSM] action exit seq[" << currentActionListIndex << "]: " << currentBinding << '\n';
      currentAction.OnExit(owner);

      if (currentActionList.value().size() > currentActionListIndex + 1) {
        bIsWaitingForActionEnd = true;
        currentActionListIndex += 1;
        currentAction = currentActionList.value()[currentActionListIndex];
        std::cout << "[FSM] action enter seq[" << currentActionListIndex << "]: " << currentBinding << '\n';
        currentAction.OnEnter(owner);
        return;
      }

      // sequence ended
      bIsWaitingForActionEnd = false;
      currentActionListIndex = 0;
      currentActionList = std::nullopt;
      continue;
    }

    if (bSkipWaiting) {
      bSkipWaiting = false;
      currentActionListIndex = 0;
      currentActionList = std::nullopt;
    }

    switch (next.index()) {
    case 0: {
      bCheckNextTransition = false;

      // get next binding
      std::string nextBinding = std::get<0>(next);
      if (!actions.contains(nextBinding))
        fsm_assert_msg(false, "Unbound action: \"" + nextBinding + "\"");

      if (bReenterTransition || currentBinding != nextBinding) {
        bReenterTransition = false;

        // std::cout << "[FSM] action exit: " << currentBinding << '\n';
        previousBinding = currentBinding;
        currentBinding = nextBinding;
        currentAction.OnExit(owner);

        currentActionList = actions[nextBinding];
        currentActionListIndex = 0;
        currentAction = currentActionList.value()[0];

        // wait for the action to end
        if (currentAction.fnIsEnded != nullptr)
          bIsWaitingForActionEnd = true;

        if (currentActionList.value().size() > 1) {
          std::cout << "[FSM] action enter seq[0]: " << currentBinding << '\n';
        } else {
          std::cout << "[FSM] action enter: " << currentBinding << '\n';
        }
        currentAction.OnEnter(owner);
      }
    } break;
    case 1: {
      // get next transition
      FsmTransition *transition = std::get<1>(next);
      if (transition) {
        currentTransition = transition;

        // update transition trace
        transitionTrace.front() += " -> " + currentTransition->GetName();
      }
    } break;
    }
  }
}

template <typename T, typename State>
auto Fsm<T, State>::Update() -> void {
  currentAction.OnUpdate(owner);
}

} // namespace LDJ
