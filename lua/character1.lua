local action_default = TestCharacter1.Action.new(TestCharacter1.state.default, {}, nil)
local action_hello = TestCharacter1.Action.new(TestCharacter1.state.hello, {}, nil)
local action_wow = TestCharacter1.Action.new(TestCharacter1.state.wow, {}, nil)
local action_pow = TestCharacter1.Action.new(TestCharacter1.state.pow, {}, nil)
local action_combo1 = TestCharacter1.Action.new(TestCharacter1.state.pow, {}, function() return ActionCompleted end)
local action_combo2 = TestCharacter1.Action.new(TestCharacter1.state.pow, {}, function() return ActionCompleted end)

local fsm = TestCharacter1.Fsm.new()
local tr1 = fsm:new_transition('tr1')
local tr2 = fsm:new_transition('tr2')

tr1
  :when('default', function() return 'hello' end)
  :when('combo', function() return 'hello' end)
  :when('hello', function() return tr2 end)

tr2
  :when('hello', function() return 'wow' end)
  :when('wow', function() return tr1:do_action('combo') end)

return fsm
  :bind_default(tr1, action_default)
  :bind('hello', action_hello)
  :bind('wow', action_wow)
  :bind('pow', action_pow)
  :bind('combo', { action_combo1, action_combo2 })
