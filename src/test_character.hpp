#pragma once

#include <sol/sol.hpp>
#include <string>

#include "fsm/fsm.hpp"
#include "test_state.hpp"

class TestCharacter {
  using Self = TestCharacter;
  using State = TestStateBase;
  using Extra = TestExtra;
  friend TestStateBase;
  friend Extra;

  bool b_is_hit = false;

  std::string name = "John";

  auto FsmTransitionHit() -> LDJ::FsmTransitionResult {
    if (b_is_hit)
      return fsm->SkipCurrent("hit");
    return LDJ::FsmContinue;
  }

public:
  static State *state_default;
  static State *state_hello;
  static State *state_wow;
  static State *state_pow;
  static auto LuaBindStates(sol::table namespace_table) -> void {
    namespace_table["default"] = TestCharacter::state_default;
    namespace_table["hello"] = TestCharacter::state_hello;
    namespace_table["wow"] = TestCharacter::state_wow;
    namespace_table["pow"] = TestCharacter::state_pow;
  }

  LDJ::Fsm<Self *, State *> * fsm;

  TestCharacter();
};
