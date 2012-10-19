input = IGInput:new()
input:attachToWorld()

player = IGPlayer:new()
player:setPosition(vec2(2, 2))
player:attachToWorld()

img = IGImageProp:new()
img:setPosition(vec2(2, 2))
img:setImage("player")
img:setBounds(rect(-1, -1, 2, 2))
--img:attachToWorld()

function input:touchMoved(delta)
    player:applyLinearImpulse(delta * player:mass() * 1.5)
end

onFrame:addCallback(function (elapsed)
    player:linearVelocity()
    local imp = -player:linearVelocity() * player:mass()
    local maximpulse = 3 * elapsed;
    local l = imp:length();
    if (l>maximpulse) then
        imp = imp * (maximpulse / l)
    end
    player:applyLinearImpulse(imp)
end)
