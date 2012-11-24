IMixedWave = IWave:new()

function IMixedWave:init(dif)
    self = IWave.init(self)
    if self then
        local duration = 13 + math.random()*4
        local waves = IWave.waves
        local n = #waves
        local m = math.min(math.random(3), n)
        local d = dif / m
        local nwaves = {}
        local i = 1
        while i <= m do
            local w = waves[math.random(n)]
            local min = w.minDifficulty or 0
            local max = w.maxDifficulty or 10000
            local dd
            if i == m then
                dd = dif
            else
                dd = d * (1+math.random()* 0.2) 
            end
            print('ham')
            if dd >= min and dd <= max then
                dif = dif - dd

                table.insert(nwaves, w:new(dd, duration))
                i = i + 1
            end
        end
        self.waves = nwaves
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
        if not v:waveFinished() then
            return false
        end
    end 
    return true 
end

