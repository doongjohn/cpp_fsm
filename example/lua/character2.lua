local action_default = Action(State.default, {}, nil)
local action_hello = Action(State.hello, {}, nil)

local fsm = Fsm
local this = This
local tr1 = fsm:new_transition('tr1')

print('mana = ' .. tostring(this.mana))

fsm
  :bind_default(tr1, action_default)
  :bind('hello', action_hello)

tr1
  :when('default', function()
    return 'hello'
  end)
  :when('hello', function()
    return 'default'
  end)
