local action_default = TestCharacter3.Action.new(TestCharacter3.state.default, {}, nil)
local action_hello = TestCharacter3.Action.new(TestCharacter3.state.hello, {}, nil)

local fsm = TestCharacter3.Fsm.new()
local tr1 = fsm:new_transition('tr1')

tr1
  :when('default', function() return 'hello' end)
  :when('hello', function() return 'default' end)

return fsm
  :bind_default(tr1, action_default)
  :bind('hello', action_hello)