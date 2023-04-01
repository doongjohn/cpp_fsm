#pragma once

class TestStateBase {
public:
  TestStateBase() = default;
  virtual ~TestStateBase() = default;

  virtual void OnEnter(class TestCharacter *owner);
  virtual void OnExit(class TestCharacter *owner);
  virtual void OnUpdate(class TestCharacter *owner);
};

class TestExtra : public TestStateBase {
public:
  void OnEnter(class TestCharacter *owner) override;
  void OnExit(class TestCharacter *owner) override;
  void OnUpdate(class TestCharacter *owner) override;
};
