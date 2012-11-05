IWallManager = IObject:new()
function IWallManager:init(lower, upper, vel)
    self = IObject.init(self)
    if self then
        self.upper = upper
        self.lower = lower
        self.velocity = vel
        self.width = 0.45
        self.epsilon = 0.25
        self.time = 0

        onFrame:addCallback(function (elapsed)
            self:advanceAnimation(elapsed)
        end)
    end
    return self
end

function IWallManager:f(x)
    local u = self.upper
    local l = self.lower
    return (math.sin(self.adv + x) * (u-l) + (u+l))/2
end

function IWallManager:advancef(a)
    self.adv = self.adv + a
end

function IWallManager:resetf()
    self.adv = 0
end

function IWallManager:advanceAnimation(elapsed)
    self.time = self.time + elapsed
    local p = self.first
    while p do
        local v = p:desiredPosition()
        v.x = v.x - self.velocity * elapsed
        p:setPosition(v)
        p = p.next
    end

    while self.first and self.first:position().x + self.width*1.5 + self.epsilon < 0 do
        local c = self.first
        c:removeFromWorld()
        self.first = c.next
    end
    while true do
        local l = self.last
        local ag, pos
        if l then
            ag = l:rotation()
            pos = l:worldPoint(vec2(self.width/2, 0))
        else
            ag = 0
            self:resetf()
            pos = vec2(-self.epsilon, self:f(0))
        end
        if pos.x > IGScripting:screenSize().x or math.abs(ag) >= math.pi/2 then
            break
        end

        function sleep(n)  -- seconds
            local t0 = os.clock()
            while os.clock() - t0 <= n do end
        end

        local tx = self.width
        local ty = self:f(tx)
        local oy = pos.y
        local dy = ty-oy
        local ln = math.sqrt(dy*dy + tx*tx)
        local f = tx/ln
        tx = tx * f
        ty = self:f(tx)
        dy = ty-oy
        ln = math.sqrt(dy*dy + tx*tx)
        ag = math.asin(dy/ln)
        self:advancef(tx)

        local cell = IGWallCell:new()
        cell.initialPos = pos
        cell:setRestrictRotation(false)
        cell:setPosition(worldPoint(vec2(self.width/2, 0), pos, ag))
        cell:setRotation(ag)
        cell:attachToWorld()
        
        if not self.first then
            self.first = cell
        end
        if l then
            l.next = cell
            cell:weldToObject(self.last);
        end
        self.last = cell
    end
end

local h = IGScripting:screenSize().y
upperWall = IWallManager:new(h-0.5, h, 1)
lowerWall = IWallManager:new(0, 0.5, 1)
