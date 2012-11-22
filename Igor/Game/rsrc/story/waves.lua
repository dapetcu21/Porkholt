IWaveManager = IObject:new()

function IWaveManager:frame(elapsed)
    local w = self.wave
    if w then
        w:frame(elapsed)
        if w:waveFinished() then
            self.wave = nil
            self:dispatchWave(w)
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

IWaitWave = IWave:new()
function IWaitWave:init(time)
    self = IWave.init(self)
    if self then
        self.time = time
    end
    return self
end

function IWaitWave:frame(elapsed)
    self.time = self.time - elapsed
end

function IWaitWave:waveFinished()
    return self.time < 0
end

require("basic_mob_wave")
require("knife_mob_wave")
require("mixed_wave")

function startGame()
    IWaveManager:reset()
    IWaveManager:dispatchWave()
end

function IWaveManager:reset()
    self.difficulty = 1
end
function IWaveManager:dispatchWave(oldWave)
    if not oldWave or oldWave:isKindOfClass(IWaitWave) then
        local w = IMixedWave:new(self.difficulty)
        self.wave = w
    else
        self.difficulty = self.difficulty + 1
        local w = IWaitWave:new(5)
        self.wave = w
    end
end
