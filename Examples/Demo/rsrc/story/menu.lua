function dismissMenu()
  if but_start then
    but_start:dismiss()
    but_start = nil
  end
  if but_achiv then
    but_achiv:dismiss()
    but_achiv = nil
  end
  if but_credits then
    but_credits:dismiss()
    but_credits = nil
  end
end

function loadMenu()
  local s = IGScripting:screenSize()
  local center = vec2(s.x - 2, s.y/2)
  local r = 0.7
  local p
  local button
  
--  p = center + vec2(r, 0)
  p = center
  button = IGBouncyButton:new()
  button:setImage("play_button")
  button:setDampingFrequency(0.5)
  button:setTarget(p)
  button:setRotationSize(vec2(0.15, 0.15))
  button:setSize(vec2(1.5 ,1.5))
  button:attachToWorld()
  button:setPositionImmediately(p + vec2(2.5, 0))
  function button:onTap() 
    startGame() 
    dismissMenu()
  end
  but_start = button

--  p = vec2(s.x - 0.1, s.y - 0.1)
--  button = IGBouncyButton:new()
--  button:setImage("back_button")
--  button:setDampingFrequency(0.5)
--  button:setTarget(p)
--  button:setRotationSize(vec2(0.15, 0.15))
--  button:setSize(vec2(0.2, 0.2))
--  button:attachToWorld()
--  button:setPositionImmediately(p)
--  function button:onTap() 
--    startGame() 
--    dismissMenu()
--  end
 
  
--  p = center + vec2(r, 0):rotate(math.pi * 2/3)
--  button = IGBouncyButton:new()
--  button:setImage("achiv_game")
--  button:setDampingFrequency(0.5)
--  button:setTarget(p)
--  button:setRotationSize(vec2(0.05, 0.05))
--  button:setSize(vec2(0.7 ,0.7))
--  button:attachToWorld()
--  button:setPositionImmediately(p + vec2(2.5, 0))
--  function button:onTap() 
--    print("button")
--  end
--  but_achiv = button
--
--  p = center + vec2(r, 0):rotate(math.pi * 4/3)
--  local button = IGBouncyButton:new()
--  button:setImage("credits_game")
--  button:setDampingFrequency(0.5)
--  button:setTarget(p)
--  button:setRotationSize(vec2(0.05, 0.05))
--  button:setSize(vec2(0.7 ,0.7))
--  button:attachToWorld()
--  button:setPositionImmediately(p + vec2(2.5, 0))
--  function button:onTap() 
--    print("credits")
--  end
--  but_credits = button

end
