#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <thread>

#include "test_character.hpp"

// https://www.lua.org/
// https://github.com/LuaJIT/LuaJIT
// https://daley-paley.medium.com/super-simple-example-of-adding-lua-to-c-710730e9528a
// https://www.youtube.com/watch?v=kDHGfHxwymI

// https://github.com/ThePhD/sol2
// https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html
// https://sol2.readthedocs.io/en/latest/tutorial/customization.html

auto init_lua_fsm() -> sol::state {
  sol::state lua;

  lua.open_libraries(sol::lib::base, sol::lib::string);

  // Lua binding: FsmTransition
  using FnWhenVec =
    LDJ::FsmTransition *(LDJ::FsmTransition::*)(std::vector<std::string>, std::function<LDJ::FsmTransitionResult()>);
  using FnWhen = LDJ::FsmTransition *(LDJ::FsmTransition::*)(std::string, std::function<LDJ::FsmTransitionResult()>);

  auto ut_FsmTransition =
    lua.new_usertype<LDJ::FsmTransition>("FsmTransition", sol::constructors<LDJ::FsmTransition(std::string)>());
  ut_FsmTransition["do_action"] = &LDJ::FsmTransition::DoAction;
  ut_FsmTransition["when_any"] = &LDJ::FsmTransition::WhenAny;
  ut_FsmTransition.set("when", sol::overload(static_cast<FnWhenVec>(&LDJ::FsmTransition::When),
                                             static_cast<FnWhen>(&LDJ::FsmTransition::When)));
  ut_FsmTransition.set("when_not", sol::overload(static_cast<FnWhenVec>(&LDJ::FsmTransition::WhenNot),
                                                 static_cast<FnWhen>(&LDJ::FsmTransition::WhenNot)));

  lua["FsmContinue"] = sol::lua_nil;
  lua["ActionRunning"] = LDJ::FsmActionState::Running;
  lua["ActionCompleted"] = LDJ::FsmActionState::Completed;
  lua["ActionBreak"] = LDJ::FsmActionState::Break;

  return lua;
}

template <typename T, typename State>
auto get_lua_fsm(sol::state &lua, std::string name, std::string script_path) -> LDJ::Fsm<T *, State *> * {
  // Create a table for namespace
  auto namespace_table = lua[name].get_or_create<sol::table>();

  // Lua binding: Action
  auto ut_Action = namespace_table.new_usertype<LDJ::FsmAction<State *>>(
    "Action",
    sol::constructors<LDJ::FsmAction<State *>(State *, std::vector<State *>, std::function<LDJ::FsmActionState()>)>());

  // Lua binding: States
  namespace_table["state"] = lua.create_table();
  T::LuaBindStates(namespace_table["state"]);
  // TODO: T::LuaBindMember

  // Lua binding: Fsm
  using Fsm = LDJ::Fsm<T *, State *>;
  auto ut_Fsm = namespace_table.new_usertype<Fsm>("Fsm", sol::constructors<Fsm()>());
  ut_Fsm["owner"] = &Fsm::owner;
  ut_Fsm["current_binding"] = &Fsm::current_binding;
  ut_Fsm["previous_binding"] = &Fsm::previous_binding;
  ut_Fsm["new_transition"] = &Fsm::NewTransition;
  ut_Fsm["bind_default"] = &Fsm::BindDefault;
  ut_Fsm.set("bind",
             sol::overload(static_cast<Fsm *(Fsm::*)(std::string, std::vector<LDJ::FsmAction<State *>>)>(&Fsm::Bind),
                           static_cast<Fsm *(Fsm::*)(std::string, LDJ::FsmAction<State *>)>(&Fsm::Bind)));

  // Get Fsm from a lua script
  try {
    sol::optional<Fsm *> opt_fsm = lua.script_file(script_path);
    if (opt_fsm) {
      return opt_fsm.value();
    } else {
      return nullptr;
    }
  } catch (std::exception e) {
    std::cout << e.what() << '\n';
    return nullptr;
  }
}

auto main() -> int {
  sol::state lua = init_lua_fsm();

  // create characters
  std::vector<TestCharacter *> characters;

  characters.emplace_back(new TestCharacter());
  characters.back()->fsm = get_lua_fsm<TestCharacter, TestStateBase>(lua, "TestCharacter1", "lua/character1.lua");
  characters.back()->fsm->Init(characters.back());

  // characters.emplace_back(new TestCharacter());
  // characters.back()->fsm = get_lua_fsm<TestCharacter, TestStateBase>(lua, "TestCharacter2", "lua/character2.lua");
  // characters.back()->fsm->Init(characters.back());

  // characters.emplace_back(new TestCharacter());
  // characters.back()->fsm = get_lua_fsm<TestCharacter, TestStateBase>(lua, "TestCharacter3", "lua/character3.lua");
  // characters.back()->fsm->Init(characters.back());

  // game start
  for (size_t i = 0; i < characters.size(); ++i) {
    std::cout << "character " << i << " ";
    characters[i]->fsm->FsmStart();
  }
  // game loop
  while (true) {
    for (size_t i = 0; i < characters.size(); ++i) {
      std::cout << "character " << i << " ";
      characters[i]->fsm->FsmUpdate();
      characters[i]->fsm->Update();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}
