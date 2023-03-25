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

  std::string name = "안유찬";
  bool bIsHit = false;

public:
  LDJ::Fsm<Self *, State *> fsm;
  TestCharacter();
};
