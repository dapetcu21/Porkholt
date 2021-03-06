IKnifeMobWave = IWave:new()
table.insert(IWave.waves, IKnifeMobWave)

function IKnifeMobWave:init(difficulty, duration)
  self = IWave.init(self)
  if self then
    self.objects = {}
    self.duration = duration
    self.speed = difficulty;
    self.mps = 1
  end
  return self
end
function IKnifeMobWave:frame(elapsed)
  if self.duration > 0 then
    self.duration = self.duration - elapsed
    local prob = self.mps * elapsed;
    if math.random() <= prob then
      local mob = IGKnifeMob:new()
      mob:setHealth(2.0)
      mob:setPosition(vec2(8, math.random()* (4-2*0.8) + 0.8))
      mob:setRestrictTranslation(false)
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
      local v = mob:linearVelocity()
      local delta = vec2(-self.speed, 0) - v
      local len = delta:length()
      if (len > 0.1) then
        delta = delta * (0.1/len)
      end
      mob:applyLinearImpulse(delta * mob:mass())
      mob:setRotation(mob:desiredRotation() + self.speed * 5 * elapsed);
    end
  end
end

function IKnifeMobWave:waveFinished()
  if self.duration > 0 or next(self.objects) then
    return false
  else
    return true
  end
end


