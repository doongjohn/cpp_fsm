#include "test_state.hpp"
#include "test_character.hpp"
#include "utils.hpp"

#include <iostream>

void TestStateBase::OnEnter(TestCharacter *owner) {
  owner->bIsHit = random_range(0, 2) == 0;
}
void TestStateBase::OnExit(TestCharacter *owner) {}
void TestStateBase::OnUpdate(TestCharacter *owner) {}

void TestExtra::OnEnter(TestCharacter *owner) {
  std::cout << "Hello! My name is " << owner->name << '\n';
}
void TestExtra::OnExit(TestCharacter *owner) {}
void TestExtra::OnUpdate(TestCharacter *owner) {}
