#pragma once

#include "test_character.hpp"

class TestCharacter2 : public TestCharacter {
  using Self = TestCharacter2;
  using State = TestState2;
  using Extra = TestStateEx;

public:
  inline static State *state_default = new TestState2();
  inline static State *state_hello = new TestState2();
  inline static State *state_wow = new TestState2();
  inline static State *state_pow = new TestState2();

  int mana = 1000;

  static auto LuaBindMembers(sol::state &lua) -> void {
    auto ut = TestCharacter::LuaBindMembers<Self>(lua, "TestCharacter2");
    ut["mana"] = &Self::mana;
  }
  auto LuaBindStates() -> void {
    TestCharacter::LuaBindStates();
    lua["State"]["default"] = state_default;
    lua["State"]["hello"] = state_hello;
    lua["State"]["wow"] = state_wow;
    lua["State"]["pow"] = state_pow;
  }

  TestCharacter2() {
    fsm = LDJ::prepare_fsm_lua_instance<TestCharacter, TestStateBase, Self, State>(lua, this);
    fsm->print_log = true;

    LuaBindStates();
    LDJ::execute_fsm_lua<TestCharacter, TestStateBase>(lua, "example/lua/character2.lua");
  }
};
