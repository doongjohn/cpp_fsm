#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <thread>

#include "../fsm/lua_fsm.hpp"
#include "test_character.hpp"

auto main() -> int {
  sol::state lua = LDJ::init_lua_fsm();

  // create characters
  std::vector<TestCharacter *> characters;

  characters.emplace_back(new TestCharacter());
  characters.back()->fsm = LDJ::get_lua_fsm<TestCharacter, TestStateBase>(lua, "TestCharacter1", "lua/character1.lua");
  characters.back()->fsm->Init(characters.back());

  // characters.emplace_back(new TestCharacter());
  // characters.back()->fsm = LDJ::get_lua_fsm<TestCharacter, TestStateBase>(lua, "TestCharacter2", "lua/character2.lua");
  // characters.back()->fsm->Init(characters.back());

  // characters.emplace_back(new TestCharacter());
  // characters.back()->fsm = LDJ::get_lua_fsm<TestCharacter, TestStateBase>(lua, "TestCharacter3", "lua/character3.lua");
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
