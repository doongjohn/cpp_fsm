#include <sol/sol.hpp>
#include <thread>

// #define LDJ_FSM_LOGGER TestLogger
// struct TestLogger {
//   static auto Log(std::string msg, std::string prefix) -> void {
//     std::cout << "Test logger: " << prefix << msg << '\n';
//   }
// };

#include "../fsm/fsm_lua.hpp"
#include "test_character.hpp"

auto main() -> int {
  std::vector<TestCharacter *> characters;

  auto c1 = new TestCharacter();
  LDJ::prepare_fsm_lua<TestCharacter, TestStateBase>(c1->lua, "Character1");
  c1->fsm = LDJ::get_fsm_lua<TestCharacter, TestStateBase>(c1->lua, "src/example/lua/character1.lua");
  c1->fsm->Init(c1);
  characters.push_back(c1);

  // auto c2 = new TestCharacter();
  // LDJ::prepare_fsm_lua<TestCharacter, TestStateBase>(c2->lua, "Character2");
  // c2->fsm = LDJ::get_fsm_lua<TestCharacter, TestStateBase>(c2->lua, "src/example/lua/character2.lua");
  // c2->fsm->Init(c2);
  // characters.push_back(c2);

  // auto c3 = new TestCharacter();
  // LDJ::prepare_fsm_lua<TestCharacter, TestStateBase>(c3->lua, "Character3");
  // c3->fsm = LDJ::get_fsm_lua<TestCharacter, TestStateBase>(c3->lua, "src/example/lua/character3.lua");
  // c3->fsm->Init(c3);
  // characters.push_back(c3);

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
