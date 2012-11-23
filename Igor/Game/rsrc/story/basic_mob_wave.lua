IBasicMobWave = IWave:new()

function IBasicMobWave:init(mobsPerSecond, duration, speed)
    self = IWave.init(self)
    if self then
        self.objects = {}
        self.duration = duration
        self.speed = speed;
        self.mps = mobsPerSecond
    end
    return self
end
function IBasicMobWave:frame(elapsed)
    if self.duration > 0 then
        self.duration = self.duration - elapsed
        local prob = self.mps * elapsed;
        if math.random() <= prob then
            local mob = IGBasicMob:new()
            mob:setHealth(2.0)
            mob:setPosition(vec2(8, math.random()* (4-2*0.7) + 0.7))
            mob:setRestrictRotation(false)
            mob:attachToWorld()
            mob.onDie:addCallback(function (m)
                self.objects[m] = nil
            end)
            self.objects[mob] = true
        end
    end

    for mob,d in pairs(self.objects) do
        if mob:position().x < -1.5 then
            mob:removeFromWorld()
            self.objects[mob] = nil;
        else
            local l = player:position()
            local p = mob:desiredPosition()
            local vel;
            if p.x < l.x then
                vel = vec2(-self.speed, 0)
            else
                vel = l - p
                local len = vel:length()
                local long = self.speed
                if len > long then
                    vel = vel * (long / len)
                end
            end

            mob:setRotation(vel:angle() - math.pi)
            mob:setPosition(p + vel * elapsed)
        end
    end
end

function IBasicMobWave:waveFinished()
    if self.duration > 0 or next(self.objects) then
        return false
    else
        return true
    end
end


