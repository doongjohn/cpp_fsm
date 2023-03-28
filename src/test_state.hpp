#pragma once

class TestCharacter;

class TestStateBase {
public:
  TestStateBase() = default;
  virtual ~TestStateBase() = default;

  virtual void OnEnter(TestCharacter *owner);
  virtual void OnExit(TestCharacter *owner);
  virtual void OnUpdate(TestCharacter *owner);
};

class TestExtra : public TestStateBase {
public:
  void OnEnter(TestCharacter *owner) override;
  void OnExit(TestCharacter *owner) override;
  void OnUpdate(TestCharacter *owner) override;
};
