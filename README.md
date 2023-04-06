# cpp_fsm

`C++ 20` FSM framework with `lua` binding.

## Example cpp

```cpp
// `StateEx` inherits from `State` and runs in the same frame as `State`.
auto ex_hello = new StateEx();

LDJ::FsmAction<State *> action_default(state_default, {});
LDJ::FsmAction<State *> action_hello(state_hello, {ex_hello});
LDJ::FsmAction<State *> action_wow(state_wow, {});
LDJ::FsmAction<State *> action_pow(state_pow, {});
LDJ::FsmAction<State *> action_combo1(state_wow, {}, [] { return LDJ::Completed; });
LDJ::FsmAction<State *> action_combo2(state_pow, {}, [] { return LDJ::Completed; });

fsm = new LDJ::Fsm<Self *, State *>(this);
auto tr1 = fsm->NewTransition("tr1");
auto tr2 = fsm->NewTransition("tr2");

fsm
  ->BindDefault(tr1, action_default) // `action_default` automatically bound as "default"
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
local action_default = Action(State.default, {}, nil)
local action_hello = Action(State.hello, { State.ex_hello }, nil)
local action_wow = Action(State.wow, {}, nil)
local action_pow = Action(State.pow, {}, nil)
local action_combo1 = Action(State.wow, {}, function() return ActionResult.Completed end)
local action_combo2 = Action(State.pow, {}, function() return ActionResult.Completed end)

local fsm = Fsm
local this = This
print(this.name)

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
  :when('combo', function()
    if math.random(3) == 1 then return fsm:skip_current(fsm:reenter('combo')) end
    if math.random(3) == 1 then return fsm:skip_current('hello') end
    return 'hello'
  end)
  :when('hello', function() return tr2 end)

tr2
  :when('hello', function() return 'wow' end)
  :when('wow', function() return tr1:do_action('combo') end)
```

```cpp
auto c = new Character();
c->fsm = LDJ::prepare_fsm_lua_instance<CharacterBase, StateBase, Character, State>(Character::lua, this);
//                                                                                            ^^^ --> sol::state
c->fsm->print_log = true;

Character::lua["State"]["ex_hello"] = Character::ex_hello;
Character::lua["State"]["default"] = Character::state_default;
Character::lua["State"]["hello"] = Character::state_hello;
Character::lua["State"]["wow"] = Character::state_wow;
Character::lua["State"]["pow"] = Character::state_pow;
LDJ::execute_fsm_lua<CharacterBase, StateBase>(Character::lua, "lua/character.lua");
```

## Unreal engine

```cpp
LDJ::FsmLogger::fn_logger = [](std::string msg, std::string prefix)
{
    UE_LOG(LogTemp, Log, TEXT("%s"), *FString(UTF8_TO_TCHAR((prefix + msg).c_str())));
};

LDJ::FsmAssert::fn_assert = [](bool expr)
{
    check(expr);
};
```
