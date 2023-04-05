#pragma once

#include <sol/sol.hpp>

#include "fsm.hpp"

namespace LDJ {

inline auto init_fsm_lua() -> sol::state {
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);

  // Lua binding: fsm_log
  lua["log"] = sol::overload(&::LDJ::fsm_log, [](std::string msg) { fsm_log(msg, "[FSM Lua] "); });

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
  lua.new_enum<LDJ::FsmActionResult>("ActionResult", {
                                                       {"Running", LDJ::FsmActionResult::Running},
                                                       {"Completed", LDJ::FsmActionResult::Completed},
                                                       {"ActionBreak", LDJ::FsmActionResult::Break},
                                                     });

  return lua;
}

template <typename T, typename State>
inline auto prepare_fsm_lua_base(sol::state &lua) -> void {
  // Lua binding: Action
  auto ut_Action = lua.new_usertype<LDJ::FsmAction<State *>>("Action");

  // Lua binding: Fsm
  using Fsm = LDJ::Fsm<T *, State *>;
  auto ut_Fsm = lua.new_usertype<Fsm>("Fsm");
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
                  static_cast<LDJ::FsmTransition *(Fsm::*)(LDJ::FsmTransition *transition)>(&Fsm::SkipCurrent));
}

template <typename FsmBase, typename StateBase, typename FsmT, typename StateT>
inline auto prepare_fsm_lua_instance(sol::state &lua, FsmT *self) -> LDJ::Fsm<FsmBase *, StateBase *> * {
  // Lua binding: States table
  lua["State"] = lua.create_table();

  // Lua binding: Action
  lua["Action"] = [](StateT *state, std::vector<StateT *> extra, std::function<LDJ::FsmActionResult()> fn_result) {
    std::vector<StateBase *> v(extra.size());
    for (size_t i = 0; i < extra.size(); ++i)
      v[i] = dynamic_cast<StateBase *>(extra[i]);

    return LDJ::FsmAction<StateBase *>(state, v, fn_result);
  };

  // Create a new Fsm
  auto fsm = new LDJ::Fsm<FsmBase *, StateBase *>(self);
  lua["Fsm"] = fsm;

  // get owner as current class
  lua["This"] = dynamic_cast<FsmT *>(self);

  return fsm;
}

template <typename T, typename State>
inline auto execute_fsm_lua(sol::state &lua, std::string script_path) -> void {
  // Run lua script
  try {
    lua.script_file(script_path);
  } catch (const std::exception &e) {
    fsm_log(e.what());
  }
}

} // namespace LDJ
