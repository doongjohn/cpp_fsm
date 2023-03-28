local action_default = TestCharacter.Action.new(TestCharacter.state.default, {}, nil)
local action_hello = TestCharacter.Action.new(TestCharacter.state.hello, {}, nil)
local action_wow = TestCharacter.Action.new(TestCharacter.state.wow, {}, nil)
local action_pow = TestCharacter.Action.new(TestCharacter.state.pow, {}, nil)

-- TODO: fnIsEnded must return enum { Ended, Running, Failed }
local action_combo1 = TestCharacter.Action.new(TestCharacter.state.pow, {}, function() return true end)
local action_combo2 = TestCharacter.Action.new(TestCharacter.state.pow, {}, function() return true end)

local tr1 = FsmTransition.new('tr1')
local tr2 = FsmTransition.new('tr2')

tr1
  :when('default', function() return 'hello' end)
  :when('combo', function() return 'hello' end)
  :when('hello', function() return tr2 end)

tr2
  :when('hello', function() return 'wow' end)
  :when('wow', function() return tr1:do_action('combo') end)

return TestCharacter.Fsm.new(tr1)
  :bind_default(action_default)
  :bind('hello', action_hello)
  :bind('wow', action_wow)
  :bind('pow', action_pow)
  :bind('combo', { action_combo1, action_combo2 })
