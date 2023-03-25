#pragma once

class TestCharacter;

class TestStateBase {
public:
  TestStateBase() = default;
  virtual ~TestStateBase() = default;

  virtual void OnEnter(TestCharacter *self);
  virtual void OnExit(TestCharacter *self);
  virtual void OnUpdate(TestCharacter *self);
};

class TestExtra : public TestStateBase {
public:
  void OnEnter(TestCharacter *self) override;
  void OnExit(TestCharacter *self) override;
  void OnUpdate(TestCharacter *self) override;
};
