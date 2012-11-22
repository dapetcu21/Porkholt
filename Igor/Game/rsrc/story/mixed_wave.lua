IMixedWave = IWave:new()

function IMixedWave:init(dif)
    self = IWave.init(self)
    if self then
        self.waves = {
            IKnifeMobWave:new(1, 15, 1),
            IBasicMobWave:new(2, 15, 1),
        }
    end
    return self
end

function IMixedWave:frame(elapsed)
    for i,v in ipairs(self.waves) do
        v:frame(elapsed)
    end
end

function IMixedWave:waveFinished()
    for i,v in ipairs(self.waves) do
        if v:waveFinished() then
            return true
        end
    end 
    return false
end
