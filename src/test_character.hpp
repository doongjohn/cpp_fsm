#pragma once

#include <string>

#include "fsm/fsm.hpp"
#include "test_state.hpp"

class TestCharacter {
  using Self = TestCharacter;
  using State = TestStateBase;
  using Extra = TestExtra;
  friend TestStateBase;
  friend Extra;

  static State *stateDefault;
  static State *stateHello;
  static State *stateWow;
  static State *statePow;

  bool bIsHit = false;

  std::string name = "John";

  auto FsmTransitionHit() -> LDJ::FsmTransitionResult {
    if (bIsHit)
      return fsm.SkipCurrent("hit");
    return LDJ::FsmContinue;
  }

public:
  LDJ::Fsm<Self *, State *> fsm;
  TestCharacter();
};
