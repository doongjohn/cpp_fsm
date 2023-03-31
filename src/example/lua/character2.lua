local action_default = Character2.Action.new(Character2.state.default, {}, nil)
local action_hello = Character2.Action.new(Character2.state.hello, {}, nil)

local fsm = Character2.Fsm.new()
local tr1 = fsm:new_transition('tr1')

fsm
  :bind_default(tr1, action_default)
  :bind('hello', action_hello)

tr1
  :when('default', function() return 'hello' end)
  :when('hello', function() return 'default' end)

return fsm
