local input = IGInput:new()
input:attachToWorld()

local player = IGPlayer:new()
player:setPosition(vec2(2, 2))
player:attachToWorld()

local img = IGImageProp:new()
img:setPosition(vec2(2, 2))
img:setImage("player")
img:setBounds(rect(-1, -1, 2, 2))
--img:attachToWorld()

function input:touchMoved(delta)
    print(player:mass())
    player:applyLinearImpulse(delta * player:mass())
    print("delta", delta.x, delta.y)
end

