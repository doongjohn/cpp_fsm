#include "test_character.hpp"
#include "utils.hpp"

TestCharacter::State *TestCharacter::stateDefault = new State();
TestCharacter::State *TestCharacter::stateHello = new State();
TestCharacter::State *TestCharacter::stateWow = new State();
TestCharacter::State *TestCharacter::statePow = new State();

TestCharacter::TestCharacter() {
  auto extraHello = new Extra();

  LDJ::FsmAction<State *> actionDefault(stateDefault, {});
  LDJ::FsmAction<State *> actionHello(stateHello, {extraHello});
  LDJ::FsmAction<State *> actionWow(stateWow, {});
  LDJ::FsmAction<State *> actionPow(statePow, {});
  LDJ::FsmAction<State *> combo1(stateHello, {}, [] { return true; });
  LDJ::FsmAction<State *> combo2(stateHello, {}, [] { return true; });

  auto tr1 = new LDJ::FsmTransition("tr1");
  auto tr2 = new LDJ::FsmTransition("tr2");

  // clang-format off

  (new (&fsm) LDJ::Fsm<Self *, State *>(this, tr1))
    ->BindDefault(actionDefault)
    ->Bind("hit", actionHello)
    ->Bind("hello", actionHello)
    ->Bind("wow", actionWow)
    ->Bind("pow", actionPow)
    ->Bind("combo", {combo1, combo2});

  tr1
    ->When("default", [] { return "hello"; })
    ->WhenNot("hit", [this] () -> LDJ::FsmTransitionResult {
      if (bIsHit)
        return fsm.SkipCurrent("hit");
      return LDJ::FsmContinue;
    })
    ->When("hit", [] { return "default"; })
    ->When("combo", [] { return "hello"; })
    ->When("hello", [=] { return tr2; });

  tr2
    ->When("hello", [] { return random_range(0, 1) ? "wow" : "pow"; })
    ->When({"wow", "pow"}, [=] { return tr1->Do("combo"); });

  // clang-format on
}
