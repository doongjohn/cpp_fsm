#include "test_character.hpp"
#include "utils.hpp"

#include "../fsm/fsm_lua.hpp"

TestCharacter::State *TestCharacter::state_default = new State();
TestCharacter::State *TestCharacter::state_hello = new State();
TestCharacter::State *TestCharacter::state_wow = new State();
TestCharacter::State *TestCharacter::state_pow = new State();

TestCharacter::TestCharacter() : lua(LDJ::init_fsm_lua()) {}
// TestCharacter::TestCharacter() {
//   auto extra_hello = new Extra();

//   LDJ::FsmAction<State *> action_default(state_default, {});
//   LDJ::FsmAction<State *> action_hello(state_hello, {extra_hello});
//   LDJ::FsmAction<State *> action_wow(state_wow, {});
//   LDJ::FsmAction<State *> action_pow(state_pow, {});
//   LDJ::FsmAction<State *> action_combo1(state_wow, {}, [] { return LDJ::Completed; });
//   LDJ::FsmAction<State *> action_combo2(state_pow, {}, [] { return LDJ::Completed; });

//   fsm = new LDJ::Fsm<Self *, State *>();
//   auto tr1 = fsm->NewTransition("tr1");
//   auto tr2 = fsm->NewTransition("tr2");

//   // clang-format off

//   fsm->Init(this);
//   fsm
//     ->BindDefault(tr1, action_default)
//     ->Bind("hit", action_hello)
//     ->Bind("hello", action_hello)
//     ->Bind("wow", action_wow)
//     ->Bind("pow", action_pow)
//     ->Bind("combo", {action_combo1, action_combo2});

//   tr1
//     ->When("default", [] { return "hello"; })
//     ->WhenNot("hit", [this] { return FsmTransitionHit(); })
//     ->When("hit", [] { return "default"; })
//     ->When("combo", [] { return "hello"; })
//     ->When("hello", [=] { return tr2; });

//   tr2
//     ->WhenNot("hit", [this] { return FsmTransitionHit(); })
//     ->When("hit", [=] { return tr1; })
//     ->When("hello", [] { return random_range(0, 1) ? "wow" : "pow"; })
//     ->When({"wow", "pow"}, [=] { return tr1->DoAction("combo"); });

//   // clang-format on
// }
