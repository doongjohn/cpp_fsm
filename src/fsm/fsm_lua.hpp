#pragma once

#include <sol/sol.hpp>

#include "fsm.hpp"

// https://www.lua.org/
// https://github.com/LuaJIT/LuaJIT
// https://github.com/ThePhD/sol2
// https://daley-paley.medium.com/super-simple-example-of-adding-lua-to-c-710730e9528a
// https://www.youtube.com/watch?v=kDHGfHxwymI

namespace LDJ {

inline auto init_fsm_lua() -> sol::state {
  sol::state lua;

  lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);

  // Lua binding: FsmTransition
  using FnWhenVec =
    LDJ::FsmTransition *(LDJ::FsmTransition::*)(std::vector<std::string>, std::function<LDJ::FsmTransitionResult()>);
  using FnWhen = LDJ::FsmTransition *(LDJ::FsmTransition::*)(std::string, std::function<LDJ::FsmTransitionResult()>);

  auto ut_FsmTransition =
    lua.new_usertype<LDJ::FsmTransition>("FsmTransition", sol::constructors<LDJ::FsmTransition(std::string)>());
  ut_FsmTransition["do_action"] = &LDJ::FsmTransition::DoAction;
  ut_FsmTransition["when_any"] = &LDJ::FsmTransition::WhenAny;
  ut_FsmTransition["when"] =
    sol::overload(static_cast<FnWhenVec>(&LDJ::FsmTransition::When), static_cast<FnWhen>(&LDJ::FsmTransition::When));
  ut_FsmTransition["when_not"] = sol::overload(static_cast<FnWhenVec>(&LDJ::FsmTransition::WhenNot),
                                               static_cast<FnWhen>(&LDJ::FsmTransition::WhenNot));

  lua["FsmContinue"] = LDJ::FsmContinue;
  lua.new_enum<LDJ::FsmActionState>("ActionResult", {
                                                      {"Running", LDJ::FsmActionState::Running},
                                                      {"Completed", LDJ::FsmActionState::Completed},
                                                      {"ActionBreak", LDJ::FsmActionState::Break},
                                                    });

  return lua;
}

template <typename T, typename State>
inline auto prepare_fsm_lua(sol::state &lua, std::string name) -> void {
  // Create a table for namespace
  auto namespace_table = lua[name].get_or_create<sol::table>();

  // Lua binding: Action
  auto ut_Action = namespace_table.new_usertype<LDJ::FsmAction<State *>>(
    "Action",
    sol::constructors<LDJ::FsmAction<State *>(State *, std::vector<State *>, std::function<LDJ::FsmActionState()>)>());

  // Lua binding: States
  namespace_table["state"] = lua.create_table();
  T::LuaBindStates(namespace_table["state"]);
  // TODO: T::LuaBindMembers

  // Lua binding: Fsm
  using Fsm = LDJ::Fsm<T *, State *>;
  auto ut_Fsm = namespace_table.new_usertype<Fsm>("Fsm", sol::constructors<Fsm()>());
  ut_Fsm["owner"] = &Fsm::owner;
  ut_Fsm["current_binding"] = &Fsm::current_binding;
  ut_Fsm["previous_binding"] = &Fsm::previous_binding;
  ut_Fsm["new_transition"] = &Fsm::NewTransition;
  ut_Fsm["bind_default"] = &Fsm::BindDefault;
  ut_Fsm["bind"] =
    sol::overload(static_cast<Fsm *(Fsm::*)(std::string, std::vector<LDJ::FsmAction<State *>>)>(&Fsm::Bind),
                  static_cast<Fsm *(Fsm::*)(std::string, LDJ::FsmAction<State *>)>(&Fsm::Bind));
  ut_Fsm["reenter"] = &Fsm::Reenter;
  ut_Fsm["skip_current"] =
    sol::overload(static_cast<std::string (Fsm::*)(std::string)>(&Fsm::SkipCurrent),
                  static_cast<LDJ::FsmTransition *(Fsm::*)(LDJ::FsmTransition * transition)>(&Fsm::SkipCurrent));
}

template <typename T, typename State>
inline auto get_fsm_lua(sol::state &lua, std::string script_path) -> LDJ::Fsm<T *, State *> * {
  // Get Fsm from a lua script
  try {
    sol::optional<LDJ::Fsm<T *, State *> *> opt_fsm = lua.script_file(script_path);
    if (opt_fsm) {
      return opt_fsm.value();
    } else {
      return nullptr;
    }
  } catch (const std::exception &e) {
    std::cout << e.what() << '\n';
    return nullptr;
  }
}

} // namespace LDJ
