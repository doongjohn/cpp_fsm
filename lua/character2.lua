local action_default = TestCharacter2.Action.new(TestCharacter2.state.default, {}, nil)
local action_hello = TestCharacter2.Action.new(TestCharacter2.state.hello, {}, nil)

local fsm = TestCharacter2.Fsm.new()
local tr1 = fsm:new_transition('tr1')

tr1
  :when('default', function() return 'hello' end)
  :when('hello', function() return 'default' end)

return fsm
  :bind_default(tr1, action_default)
  :bind('hello', action_hello)
