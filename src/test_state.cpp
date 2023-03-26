#include "test_state.hpp"
#include "test_character.hpp"
#include "utils.hpp"

#include <iostream>

void TestStateBase::OnEnter(TestCharacter *self) {
  self->bIsHit = random_range(0, 2) == 0;
}
void TestStateBase::OnExit(TestCharacter *self) {}
void TestStateBase::OnUpdate(TestCharacter *self) {}

void TestExtra::OnEnter(TestCharacter *self) {
  std::cout << "extra enter!\n";
}
void TestExtra::OnExit(TestCharacter *self) {}
void TestExtra::OnUpdate(TestCharacter *self) {}
