IDeadpoolWave = IWave:new()

function IDeadpoolWave:init()
  self = IWave.init(self)
  if self then
    local s = IGScripting:screenSize()
    local p = vec2(s.x - 2, s.y/2)
    local d = IGDeadpool:new()
    --d:setPosition(p + vec2(2.5, 0))
    d:setPosition(p)
    d:setDampingFrequency(0.5)
    d:attachToWorld()
    d:setPositionImmediately(p + vec2(2.5, 0))
    local ss = self
    function d:dialog3()
      bullets:addBullet(bullets:newHomingBullet(self:position() + vec2(-0.5, -0.43), vec2(-4, math.random() * 4 - 2), player, 2))
      ss.interval = 0.1;
      ss.stage = 2
    end
    self.deadpool = d
    self.time = 2
    self.interval = 2
    self.stage = 0
  end
  return self
end

function IDeadpoolWave:waveFinished()
  return false
end

function IDeadpoolWave:frame(elapsed)
  self.time = self.time - elapsed
  if self.time < 0 then
    self.stage = self.stage + 1
    local f = self.deadpool["dialog"..tostring(self.stage)]
    if type(f) == "function" then
      f(self.deadpool)
    end
    self.time = self.interval
  end
end
