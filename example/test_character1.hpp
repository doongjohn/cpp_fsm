#pragma once

#include "test_character.hpp"

class TestCharacter1 : public TestCharacter {
  using Self = TestCharacter1;
  using State = TestState1;
  using Extra = TestExtra;
  friend State;
  friend Extra;

public:
  inline static State *state_default = new TestState1();
  inline static State *state_hello = new TestState1();
  inline static State *state_wow = new TestState1();
  inline static State *state_pow = new TestState1();

  int power = 10;

  static auto LuaBindMembers(sol::state &lua) -> void {
    auto ut = TestCharacter::LuaBindMembers<Self>(lua, "TestCharacter1");
    ut["power"] = &Self::power;
  }
  auto LuaBindStates() -> void {
    lua["State"]["default"] = Self::state_default;
    lua["State"]["hello"] = Self::state_hello;
    lua["State"]["wow"] = Self::state_wow;
    lua["State"]["pow"] = Self::state_pow;
  }

  TestCharacter1() {
    fsm = LDJ::prepare_fsm_lua_instance<TestCharacter, TestStateBase, Self, State>(TestCharacter::lua, this);
    fsm->print_log = true;

    LuaBindStates();
    LDJ::execute_fsm_lua<TestCharacter, TestStateBase>(TestCharacter::lua, "example/lua/character1.lua");
  }
};
