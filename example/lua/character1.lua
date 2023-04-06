local action_default = Action(State.default, {}, nil)
local action_hello = Action(State.hello, { State.state_ex }, nil)
local action_wow = Action(State.wow, {}, nil)
local action_pow = Action(State.pow, {}, nil)
local action_combo1 = Action(State.wow, {}, function() return ActionResult.Completed end)
local action_combo2 = Action(State.pow, {}, function() return ActionResult.Completed end)

local fsm = Fsm
local this = This
print(this.name)
print('power = ' .. tostring(this.power))

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
