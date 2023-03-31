#pragma once

#include <sol/sol.hpp>
#include <string>

#include "../fsm/fsm.hpp"
#include "test_state.hpp"

class TestCharacter {
  using Self = TestCharacter;
  using State = TestStateBase;
  using Extra = TestExtra;
  friend TestStateBase;
  friend Extra;

  std::string name = "John";
  bool b_is_hit = false;

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
  static auto LuaBindMembers(sol::table namespace_table) -> void {
    auto ut_Self = namespace_table.new_usertype<Self>("TestCharacter");
    ut_Self["name"] = &Self::name;
    ut_Self["name1"] = &Self::name;
    ut_Self["name2"] = &Self::name;
    ut_Self["name3"] = &Self::name;
    ut_Self["is_hit"] = &Self::b_is_hit;
  }

  sol::state lua;
  LDJ::Fsm<Self *, State *> *fsm;

  TestCharacter();
};