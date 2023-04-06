#pragma once

#include <fsm/fsm_lua.hpp>
#include <sol/sol.hpp>
#include <string>

#include "test_state.hpp"

class TestCharacter {
  using Self = TestCharacter;
  using State = TestStateBase;
  using Extra = TestStateEx;

public:
  inline static sol::state lua = LDJ::init_fsm_lua();
  LDJ::Fsm<Self *, State *> *fsm;

  inline static State *state_ex = new TestStateEx();

  std::string name = "John";

protected:
  template <typename T>
  static auto LuaBindMembers(sol::state &lua, std::string class_name) -> sol::usertype<T> {
    auto ut = lua.new_usertype<T>(class_name);
    ut["name"] = &T::name;
    return ut;
  }
  static auto LuaBindStates() -> void {
    lua["State"]["state_ex"] = state_ex;
  }

public:
  TestCharacter() = default;
  virtual ~TestCharacter() = default;
};
