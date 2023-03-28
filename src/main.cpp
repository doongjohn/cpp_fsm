#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <thread>

#include "test_character.hpp"

auto setup_lua(sol::state &lua) -> void {
  // https://www.lua.org/
  // https://daley-paley.medium.com/super-simple-example-of-adding-lua-to-c-710730e9528a
  // https://www.youtube.com/watch?v=kDHGfHxwymI
  // https://github.com/LuaJIT/LuaJIT

  // https://github.com/ThePhD/sol2
  // https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html
  // https://sol2.readthedocs.io/en/latest/tutorial/customization.html

  // FsmTransition
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

  // TODO: create a macro for this

  // TestCharacter
  auto namespace_TestCharacter = lua["TestCharacter"].get_or_create<sol::table>();

  // Action
  auto ut_TestCharacterAction = namespace_TestCharacter.new_usertype<LDJ::FsmAction<TestStateBase *>>(
    "Action", sol::constructors<LDJ::FsmAction<TestStateBase *>(TestStateBase *, std::vector<TestStateBase *>,
                                                                std::function<bool()>)>());

  // States
  namespace_TestCharacter["state"] = lua.create_table();
  namespace_TestCharacter["state"]["default"] = TestCharacter::stateDefault;
  namespace_TestCharacter["state"]["hello"] = TestCharacter::stateHello;
  namespace_TestCharacter["state"]["wow"] = TestCharacter::stateWow;
  namespace_TestCharacter["state"]["pow"] = TestCharacter::statePow;

  // TestCharacter Fsm
  using TestCharacterFsm = LDJ::Fsm<TestCharacter *, TestStateBase *>;
  auto ut_TestCharacterFsm = namespace_TestCharacter.new_usertype<TestCharacterFsm>(
    "Fsm", sol::constructors<TestCharacterFsm(LDJ::FsmTransition *)>());
  ut_TestCharacterFsm["owner"] = &TestCharacterFsm::owner;
  ut_TestCharacterFsm["current_binding"] = &TestCharacterFsm::currentBinding;
  ut_TestCharacterFsm["previous_binding"] = &TestCharacterFsm::previousBinding;
  ut_TestCharacterFsm["previous_binding"] = &TestCharacterFsm::previousBinding;
  ut_TestCharacterFsm["bind_default"] = &TestCharacterFsm::BindDefault;
  ut_TestCharacterFsm.set(
    "bind",
    sol::overload(
      static_cast<TestCharacterFsm *(TestCharacterFsm::*)(std::string, std::vector<LDJ::FsmAction<TestStateBase *>>)>(
        &TestCharacterFsm::Bind),
      static_cast<TestCharacterFsm *(TestCharacterFsm::*)(std::string, LDJ::FsmAction<TestStateBase *>)>(
        &TestCharacterFsm::Bind)));
}

auto main() -> int {
  // Create lua state
  sol::state lua;

  // Open defualt libraries
  lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::string);

  // Bind c++ class and functions to lua
  setup_lua(lua);

  // Get Fsm from lua
  sol::optional<LDJ::Fsm<TestCharacter *, TestStateBase *> *> opt_fsm = lua.script_file("lua/script.lua");
  if (!opt_fsm) {
    std::cout << "Must return fsm.\n";
    return 1;
  }

  // Create TestCharacter
  TestCharacter character;
  character.fsm = *opt_fsm.value();
  character.fsm.Init(&character);

  // run gc
  lua.collect_gc();

  // game loop
  character.fsm.FsmStart();
  while (true) {
    character.fsm.FsmUpdate();
    character.fsm.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}
