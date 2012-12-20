master:setDynamic(false)
player:setFreezed(true)

local timer = PHTimer:new(nil,1,false)
timer:setCallback(function()
  master:addDialog("Ok Beeno, now that you're in shape,\nIt's time to start training!")
  master:addDialog("First things first. You must learn to jump properly")
  player:addDialog("But I know how to jump")
  master:addDialog("Then, this is not going to be a problem",function()
  player: setFreezed(false);
  end)
end)
timer:schedule()
function sensordash:objectEntered()
  if (sensordash.beentheredonethat) then
    return
  end
  sensordash.beentheredonethat = true
  player:setFreezed(true)
  sign3:display(function()
  player:setFreezed(false)
  sign3.displayed = true
  end)
end
timer:schedule()
function sensordash:objectEntered()
  if (sensordash.beentheredonethat) then
    return
  end
  sensordash.beentheredonethat = true
  player:setFreezed(true)
  sign4:display(function()
  player:setFreezed(false)
  sign4.displayed = true
  end)
end
