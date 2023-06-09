#pragma once

#define STATE_API_DEF(T_SUPER, T_DERIVED) \
  void OnEnter(T_SUPER *owner) override; \
  void OnExit(T_SUPER *owner) override; \
  void OnUpdate(T_SUPER *owner) override; \
  static void OnEnter(T_DERIVED *owner); \
  static void OnExit(T_DERIVED *owner); \
  static void OnUpdate(T_DERIVED *owner);

#define STATE_API_IMPL(T_SUPER, T_DERIVED, STATE) \
  void STATE::OnEnter(T_SUPER *owner) { \
    OnEnter(dynamic_cast<T_DERIVED *>(owner)); \
  } \
  void STATE::OnExit(T_SUPER *owner) { \
    OnExit(dynamic_cast<T_DERIVED *>(owner)); \
  } \
  void STATE::OnUpdate(T_SUPER *owner) { \
    OnUpdate(dynamic_cast<T_DERIVED *>(owner)); \
  }

class TestStateBase {
public:
  TestStateBase() = default;
  virtual ~TestStateBase() = default;

  virtual void OnEnter(class TestCharacter *owner) = 0;
  virtual void OnExit(class TestCharacter *owner) = 0;
  virtual void OnUpdate(class TestCharacter *owner) = 0;
};

class TestStateEx : public TestStateBase {
public:
  void OnEnter(class TestCharacter *owner) override;
  void OnExit(class TestCharacter *owner) override;
  void OnUpdate(class TestCharacter *owner) override;
};

class TestState1 : public TestStateBase {
public:
  STATE_API_DEF(class TestCharacter, class TestCharacter1)
};

class TestState2 : public TestStateBase {
public:
  STATE_API_DEF(class TestCharacter, class TestCharacter2)
};
