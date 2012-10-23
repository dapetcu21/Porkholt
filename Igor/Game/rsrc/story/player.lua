input = IGInput:new()
input:attachToWorld()

player = IGPlayer:new()
player:setPosition(vec2(1, 2))
player:attachToWorld()

function input:touchMoved(delta)
    player:applyLinearImpulse(delta * player:mass() * 2.5)
end

onFrame:addCallback(function (elapsed)
    local imp = -player:linearVelocity() * player:mass()
    local maximpulse = 5 * elapsed;
    local l = imp:length();
    if (l>maximpulse) then
        imp = imp * (maximpulse / l)
    end
    player:applyLinearImpulse(imp)
end)

cooldown = 0
onFrame:addCallback(function (elapsed)
    if not input:isFiring() then 
        return
    end
    cooldown = cooldown - elapsed;
    while (cooldown < 0) do
        bullets:addBullet(1, player:position() + vec2(0.5, 0), 0, 1); --fire
        cooldown = cooldown + 0.2
    end
end)

onFrame:addCallback(function (elapsed)
    local p = player:position()
    local v = player:linearVelocity()
    local limit = 0.5
    if p.x < limit then
        local f = (limit - (p.x + v.x * 0.07)) * 50
        player:applyForce(vec2(f, 0))
    end
    limit = 3
    if p.x > limit then
        local f = (limit - (p.x + v.x * 0.07)) * 50
        player:applyForce(vec2(f, 0))
    end
end)


