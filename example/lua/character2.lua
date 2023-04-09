local fsm = Fsm
local this = This
print(this.name)
print('mana = ' .. tostring(this.mana))

local action_default = Action(State.default, {}, nil)
local action_hello = Action(State.hello, {}, function()
  return Duration(5)
end)

local tr1 = fsm:new_transition('tr1')

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
