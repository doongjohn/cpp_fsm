# cpp_fsm

`C++` FSM framework with `lua` binding.

## Example cpp

```cpp
// `Extra` inherits from `State` and runs in the same frame as `State`.
auto extra_hello = new Extra();

LDJ::FsmAction<State *> action_default(state_default, {});
LDJ::FsmAction<State *> action_hello(state_hello, {extra_hello});
LDJ::FsmAction<State *> action_wow(state_wow, {});
LDJ::FsmAction<State *> action_pow(state_pow, {});
LDJ::FsmAction<State *> action_combo1(state_wow, {}, [] { return LDJ::Completed; });
LDJ::FsmAction<State *> action_combo2(state_pow, {}, [] { return LDJ::Completed; });

fsm = new LDJ::Fsm<Self *, State *>();
auto tr1 = fsm->NewTransition("tr1");
auto tr2 = fsm->NewTransition("tr2");

fsm->Init(this);
fsm
  ->BindDefault(tr1, action_default) // `action_default` will be bound as "default"
  ->Bind("hello", action_hello)
  ->Bind("wow", action_wow)
  ->Bind("pow", action_pow)
  ->Bind("combo", {action_combo1, action_combo2});

tr1
  ->When("default", [] { return "hello"; })
  ->When("combo", [] { return "hello"; })
  ->When("hello", [=] { return tr2; });

tr2
  ->When("hello", [] { return random_range(0, 1) ? "wow" : "pow"; })
  ->When({"wow", "pow"}, [=] { return tr1->DoAction("combo"); });
```

## Example lua

```lua
local action_default = Character1.Action.new(Character1.state.default, {}, nil)
local action_hello = Character1.Action.new(Character1.state.hello, {}, nil)
local action_wow = Character1.Action.new(Character1.state.wow, {}, nil)
local action_pow = Character1.Action.new(Character1.state.pow, {}, nil)
local action_combo1 = Character1.Action.new(Character1.state.wow, {}, function() return ActionResult.Completed end)
local action_combo2 = Character1.Action.new(Character1.state.pow, {}, function() return ActionResult.Completed end)

local fsm = Character1.Fsm.new()
local tr1 = fsm:new_transition('tr1')
local tr2 = fsm:new_transition('tr2')

fsm
  :bind_default(tr1, action_default)
  :bind('hello', action_hello)
  :bind('wow', action_wow)
  :bind('pow', action_pow)
  :bind('combo', { action_combo1, action_combo2 })

tr1
  :when('default', function() return 'hello' end)
  :when('combo', function() return 'hello' end)
  :when('hello', function() return tr2 end)

tr2
  :when('hello', function() return 'wow' end)
  :when('wow', function() return tr1:do_action('combo') end)

return fsm
```

```cpp
#include <sol/sol.hpp>

#include "fsm/fsm.hpp"
#include "your_state.hpp"

class YourCharacter {
  using Self = YourCharacter;
  using State = YourStateBase;
  using Extra = YourStateExtra;
  friend State;
  friend Extra;

  std::string name = "John";

public:
  static State *state_default;
  static State *state_hello;
  static State *state_wow;
  static State *state_pow;

  static auto LuaBindStates(sol::table namespace_table) -> void {
    namespace_table["default"] = TestCharacter::state_default;
    namespace_table["hello"] = TestCharacter::state_hello;
    namespace_table["wow"] = TestCharacter::state_wow;
    namespace_table["pow"] = TestCharacter::state_pow;
  }
  static auto LuaBindMembers(sol::table namespace_table) -> void {
    auto ut_Self = namespace_table.new_usertype<Self>("YourCharacter");
    ut_Self["name"] = &Self::name;
  }

  sol::state lua;
  LDJ::Fsm<Self *, State *> *fsm;
};
```

```cpp
auto c = new YourCharacter();
LDJ::prepare_fsm_lua<YourCharacter, YourStateBase>(c->lua, "Character");
c->fsm = LDJ::get_fsm_lua<YourCharacter, YourStateBase>(c->lua, "src/example/lua/character.lua");
c->fsm->Init(c);
```

