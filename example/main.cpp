#define SOL_PRINT_ERRORS 1
#include <sol/sol.hpp>
#include <thread>

#include "fsm/fsm_lua.hpp"
#include "test_character1.hpp"
#include "test_character2.hpp"

class FsmLuaBinder {
public:
  static auto Init() -> int {
    LDJ::prepare_fsm_lua_base<TestCharacter, TestStateBase>(TestCharacter::lua);
    TestCharacter1::LuaBindMembers(TestCharacter::lua);
    TestCharacter2::LuaBindMembers(TestCharacter::lua);
    return 0;
  }

  inline static int FsmLuaBind = FsmLuaBinder::Init();
};

auto main() -> int {
  std::vector<TestCharacter *> characters;

  auto c1 = new TestCharacter1();
  characters.push_back(c1);

  auto c2 = new TestCharacter2();
  characters.push_back(c2);

  auto c3 = new TestCharacter1();
  characters.push_back(c3);

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
