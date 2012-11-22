function PHLog(...)
    print(string.format(...))
end

log = PHLog

function print(...)
    local s = ''
    for i = 1, select("#", ...) do 
        s = s..tostring(select(i, ...))
    end
    _PHLog(s)
end

IGObject = { 
    className = "IGObject"
}

IGScripting = {}

function IGObject:new(...)
    local o = IGScripting:_classFromName(self.className)
    return o:init(...)
end

function IGObject:subclass(name)
    local o = { className = name, __index = self }
    setmetatable(o, o)
    return o
end

function IGObject:isKindOfClass(class)
    local s = nil
    if type(class) == "string" then
        s = class
    elseif type(class) == "table" then
        s = class.className
    end
    if s then
        while self do
            if self.className == s then
                return true
            end
            self = self.__index
        end
    end
    return false
end

function IGObject:init()
    return self
end

IObject = {}
function IObject:new(...)
    local o = { __index = self; }
    setmetatable(o, o)
    return self.init(o, ...)
end

function IObject:init()
    return self
end

function IObject:isKindOfClass(class)
    while self do
        if self == class then
            return true
        end
        self = self.__index
    end
    return false
end

PHVector2_meta = {}
PHRect_meta = {}

function PHVector2_meta.__add(a, b)
    return vec2(a.x + b.x, a.y + b.y)
end
function PHVector2_meta.__sub(a, b)
    return vec2(a.x - b.x, a.y - b.y)
end
function PHVector2_meta.__unm(a)
    return vec2(-a.x, -a.y)
end
function PHVector2_meta.__mul(a, b)
    if (type(b) == "number") then
        return vec2(a.x*b, a.y*b)
    else
        return vec2(a.x*b.x, a.y*b.y)
    end
end
function PHVector2_meta.__div(a, b)
    if (type(b) == "number") then
        return vec2(a.x/b, a.y/b)
    else
        return vec2(a.x/b.x, a.y/b.y)
    end
end
PHVector2_meta.__index = PHVector2_meta
function PHVector2_meta.length(o)
    return math.sqrt(o.x*o.x + o.y*o.y)
end
function PHVector2_meta.lengthSquared(o)
    return o.x*o.x + o.y*o.y
end

function PHVector2_meta.rotate(p, tetha)
    local s = math.sin(tetha)
    local c = math.cos(tetha)
    return vec2(p.x * c - p.y * s, p.x * s + p.y * c)
end

function vec2(x, y)
    local o = { x = x, y = y }
    setmetatable(o, PHVector2_meta)
    return o
end

function rect(x, y, w, h)
    local o = { x = x, y = y, width = w, height = h }
    setmetatable(o, PHRect_meta)
    return o
end

IGCallbackTable = {}
function IGCallbackTable:new(o)
    o = o or {}
    o.__index = self
    setmetatable(o, o)
    return o
end
function IGCallbackTable:addCallback(cb)
    local n = #self + 1
    self[n] = cb
    self[cb]  = n
end
function IGCallbackTable:removeCallback(cb)
    table.remove(self, self[cb])
    self[cb] = nil
end
function IGCallbackTable:call(...)
    for i,v in ipairs(self) do
        v(...)
    end
end
onFrame = IGCallbackTable:new()

IGProp = IGObject:subclass("IGProp")
IGImageProp = IGProp:subclass("IGImageProp")
IGInput = IGObject:subclass("IGInput")
IGDampingProp = IGProp:subclass("IGDampingProp")
IGPlayer = IGProp:subclass("IGPlayer")
IGMob = IGDampingProp:subclass("IGMob")
IGBasicMob = IGMob:subclass("IGBasicMob")
IGBulletManager = IGObject:subclass("IGBulletManager")
IGScreenBounds = IGObject:subclass("IGScreenBounds")
IGWallCell = IGDampingProp:subclass("IGWallCell")
IGKinematic = IGProp:subclass("IGKinematic")
IGWallManager = IGObject:subclass("IGWallManager")
IGBouncyButton = IGDampingProp:subclass("IGBouncyButton")
IGBackgroundParticles = IGObject:subclass("IGBackgroundParticles")
IGRipples = IGObject:subclass("IGRipples")
IGKnifeMob = IGMob:subclass("IGKnifeMob")

function IGMob:init()
    self = IGDampingProp.init(self)
    if self then
        self.onDie = IGCallbackTable:new()
    end
    return self
end

function IGWallManager:init(lower, upper, vel, flat)
    self = IGObject.init(self)
    if self then
        self:setLowerMargin(lower)
        self:setUpperMargin(upper)
        self:setWallVelocity(vel or 1)
        self:setCellWidth(0.45)
        self:setLeftEpsilon(0.25)
        self:setFlatMargin(flat or 0)
    end
    return self
end

function IGBasicMob:init()
    self = IGMob.init(self)
    if self then
        self.onDie:addCallback(function (mob)
            mob:removeFromWorld()
        end)
    end
    return self
end

function IGKnifeMob:init()
    self = IGMob.init(self)
    if self then
        self.onDie:addCallback(function (mob)
            mob:removeFromWorld()
        end)
    end
    return self
end

function IGMob:dieCallback()
    self.onDie:call(self)
end

function IGBasicMob:beginContact(o)
    if o == player then
        player:loseHealth(2)
        self:die()
    end
end

function IGKnifeMob:beginContact(o)
    if o == player and not self.touchedPlayer then
        self.touchedPlayer = true
        player:loseHealth(2);
    end
end

function frame(elapsed)
    onFrame:call(elapsed)
end

function lowpass(last, new, period, cutoff)
    local RC = 1 / cutoff
    local alpha = period/(period + RC)
    return new * alpha + last * (1 - alpha)
end

function rand(lower, upper)
    return lower + math.random() * (upper-lower)
end
