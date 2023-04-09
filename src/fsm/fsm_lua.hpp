#pragma once

#include <sol/sol.hpp>

#include "fsm.hpp"

namespace LDJ {

inline auto init_fsm_lua() -> sol::state {
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);

  // Lua binding: fsm_log
  lua["log"] = sol::overload(&fsm_log, [](const std::string &msg) { fsm_log(msg, "[FSM Lua] "); });

  // Lua binding: FsmAction
  lua.new_enum<FsmActionStatus>("Status", {
                                            {"Running", FsmActionStatus::Running},
                                            {"Completed", FsmActionStatus::Completed},
                                            {"Break", FsmActionStatus::Break},
                                          });
  lua["Duration"] = [](float f) {
    return FsmActionDuration{f};
  };

  // Lua binding: FsmTransition
  using FnWhenVec =
    FsmTransition *(FsmTransition::*)(const std::vector<std::string> &, const std::function<FsmTransitionResult()> &);
  using FnWhen = FsmTransition *(FsmTransition::*)(std::string, const std::function<FsmTransitionResult()> &);

  auto ut_FsmTransition = lua.new_usertype<FsmTransition>("FsmTransition");
  ut_FsmTransition["do_action"] = &FsmTransition::DoAction;
  ut_FsmTransition["when_any"] = &FsmTransition::WhenAny;
  ut_FsmTransition["when"] =
    sol::overload(static_cast<FnWhenVec>(&FsmTransition::When), static_cast<FnWhen>(&FsmTransition::When));
  ut_FsmTransition["when_not"] =
    sol::overload(static_cast<FnWhenVec>(&FsmTransition::WhenNot), static_cast<FnWhen>(&FsmTransition::WhenNot));
  lua["FsmContinue"] = FsmContinue;

  return lua;
}

template <typename T, typename State>
inline auto prepare_fsm_lua_base(sol::state &lua) -> void {
  // Lua binding: Fsm
  using Fsm = Fsm<T *, State *>;
  auto ut_Fsm = lua.new_usertype<Fsm>("Fsm");
  ut_Fsm["current_binding"] = &Fsm::current_binding;
  ut_Fsm["previous_binding"] = &Fsm::previous_binding;
  ut_Fsm["new_transition"] = &Fsm::NewTransition;
  ut_Fsm["bind_default"] = &Fsm::BindDefault;
  ut_Fsm["bind"] = sol::overload(static_cast<Fsm *(Fsm::*)(std::string, std::vector<FsmAction<State *>>)>(&Fsm::Bind),
                                 static_cast<Fsm *(Fsm::*)(std::string, FsmAction<State *>)>(&Fsm::Bind));
  ut_Fsm["reenter"] = &Fsm::Reenter;
  ut_Fsm["skip_current"] =
    sol::overload(static_cast<std::string (Fsm::*)(std::string)>(&Fsm::SkipCurrent),
                  static_cast<FsmTransition *(Fsm::*)(FsmTransition *transition)>(&Fsm::SkipCurrent));
}

template <typename FsmBase, typename StateBase, typename FsmT, typename StateT>
inline auto prepare_fsm_lua_instance(sol::state &lua, FsmT *self) -> Fsm<FsmBase *, StateBase *> * {
  // Lua binding: States table
  lua["State"] = lua.create_table();

  // Lua binding: Action
  // NOTE: State can have at most one parent class.
  const auto new_action0 = [](StateBase *state, std::vector<StateBase *> extra,
                              std::function<FsmActionResult()> fn_result) {
    return FsmAction<StateBase *>(state, extra, fn_result);
  };
  const auto new_action1 = [](StateT *state, std::vector<StateBase *> extra,
                              std::function<FsmActionResult()> fn_result) {
    return FsmAction<StateBase *>(state, extra, fn_result);
  };
  lua["Action"] = sol::overload(new_action0, new_action1);

  // Create a new Fsm
  auto fsm = new Fsm<FsmBase *, StateBase *>(self);
  lua["Fsm"] = fsm;
  lua["Owner"] = dynamic_cast<FsmT *>(self); // <-- get owner as current class

  return fsm;
}

inline auto execute_fsm_lua(sol::state &lua, const std::string &script_path) -> void {
  // Run lua script
  try {
    lua.script_file(script_path);
  } catch (const std::exception &e) {
    fsm_log(e.what());
  }
}

} // namespace LDJ
