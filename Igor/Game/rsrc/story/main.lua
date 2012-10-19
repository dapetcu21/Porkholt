input = IGInput:new()
input:attachToWorld()

player = IGPlayer:new()
player:setPosition(vec2(1, 2))
player:attachToWorld()

mob = IGBasicMob:new()
mob:setPosition(vec2(4, 2))
mob:attachToWorld()

function input:touchMoved(delta)
    player:applyLinearImpulse(delta * player:mass() * 1.5)
end

onFrame:addCallback(function (elapsed)
    local imp = -player:linearVelocity() * player:mass()
    local maximpulse = 3 * elapsed;
    local l = imp:length();
    if (l>maximpulse) then
        imp = imp * (maximpulse / l)
    end
    player:applyLinearImpulse(imp)
end)

time = 0
onFrame:addCallback(function (elapsed)
   time = time - elapsed * 2
   mob:setPosition(vec2(4 + math.cos(time), 2 + math.sin(time)))
   mob:setRotation(time)
end)

