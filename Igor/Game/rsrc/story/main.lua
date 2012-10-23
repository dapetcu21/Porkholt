require("bullets")
require("player")

mob = IGBasicMob:new()
mob:setPosition(vec2(4, 2))
mob:attachToWorld()

time = 0
onFrame:addCallback(function (elapsed)
    time = time + elapsed * 2
    mob:setPosition(vec2(4 + math.cos(time), 2 + math.sin(time)))
    mob:setRotation(time)
end)


