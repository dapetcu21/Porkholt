IWaveManager = IObject:new()

function IWaveManager:frame(elapsed)
    if self.wave then
        self.wave:frame(elapsed)
        if self.wave:waveFinished() then
            print("wave finished")
            self.wave = nil
            self:dispatchWave()
        end
    end
end 

onFrame:addCallback(function (elapsed)
    IWaveManager:frame(elapsed)
end)

IWave = IObject:new()
function IWave:waveFinished()
    return true
end

function IWave:frame(elapsed)
end

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
            mob:setPosition(vec2(8, math.random()* 3 + 0.5))
            mob:attachToWorld()
            mob.onDie:addCallback(function (m)
                self.objects[m] = nil
                print('mob died')
            end)
            self.objects[mob] = true
        end
    end

    for mob,d in pairs(self.objects) do
        if mob:position().x < -1.5 then
            mob:removeFromWorld()
            self.objects[mob] = nil;
            print("mob expired")
        else
            local l = player:position()
            local p = mob:desiredPosition()
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

--------------------------

ISimWave = IWave:new()

function ISimWave:init(v, miu, k, g)
    self = IWave.init(self)
    if self then
        self.obj = IGBasicMob:new()
        self.obj:setPosition(vec2(3, 2))
        self.obj:attachToWorld()
        self.miu = miu
        self.k = k
        self.g = g
        self.obj:setLinearVelocity(vec2(v, 0))
    end
    return self
end

function ISimWave:frame(elapsed)
    local x = self.obj:position().x - 3
    local vel = self.obj:linearVelocity().x
    self.obj:applyForce(vec2(- x * self.k, 0))
    if vel > 0.001 then
        self.obj:applyForce(vec2(self.obj:mass() * self.g * self.miu * -(vel / math.abs(vel)), 0 ))
    end
    print (x)
end

----------------------------

local w = IBasicMobWave:new(2, 10, 1)
IWaveManager.wave = w

function IWaveManager:dispatchWave()
end