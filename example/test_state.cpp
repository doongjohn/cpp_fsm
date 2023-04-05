#include "test_state.hpp"

#include <iostream>

#include "test_character1.hpp"
#include "test_character2.hpp"

void TestStateBase::OnEnter(TestCharacter *owner) {}
void TestStateBase::OnExit(TestCharacter *owner) {}
void TestStateBase::OnUpdate(TestCharacter *owner) {}

void TestExtra::OnEnter(TestCharacter *owner) {
  std::cout << "Hello! I'm " << owner->name << ".\n";
}
void TestExtra::OnExit(TestCharacter *owner) {}
void TestExtra::OnUpdate(TestCharacter *owner) {}

STATE_API_IMPL(TestState1, TestCharacter, TestCharacter1)
void TestState1::OnEnter(TestCharacter1 *owner) {}
void TestState1::OnExit(TestCharacter1 *owner) {}
void TestState1::OnUpdate(TestCharacter1 *owner) {}

STATE_API_IMPL(TestState2, TestCharacter, TestCharacter2)
void TestState2::OnEnter(TestCharacter2 *owner) {}
void TestState2::OnExit(TestCharacter2 *owner) {}
void TestState2::OnUpdate(TestCharacter2 *owner) {}
