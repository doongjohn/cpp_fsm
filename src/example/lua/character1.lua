local action_default = Character1.Action.new(Character1.state.default, {}, nil)
local action_hello = Character1.Action.new(Character1.state.hello, {}, nil)
local action_wow = Character1.Action.new(Character1.state.wow, {}, nil)
local action_pow = Character1.Action.new(Character1.state.pow, {}, nil)
local action_combo1 = Character1.Action.new(Character1.state.pow, {}, function() return ActionResult.Completed end)
local action_combo2 = Character1.Action.new(Character1.state.pow, {}, function() return ActionResult.Completed end)

local fsm = Character1.Fsm.new()
local tr1 = fsm:new_transition('tr1')
local tr2 = fsm:new_transition('tr2')

tr1
  :when('default', function()
    print(fsm.owner.name)
    return 'hello'
  end)
  :when('combo', function()
    if math.random(3) == 1 then return fsm:skip_current(fsm:reenter('combo')) end
    if math.random(3) == 1 then return fsm:skip_current('hello') end
    return 'hello'
  end)
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
