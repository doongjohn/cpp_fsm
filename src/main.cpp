#include "test_character.hpp"
#include <thread>

auto main() -> int {
  // create a character
  TestCharacter character;

  // game loop
  character.fsm.FsmStart();
  while (true) {
    character.fsm.FsmUpdate();
    character.fsm.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}
