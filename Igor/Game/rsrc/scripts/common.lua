
IGObject = { 
    className = "IGObject"
}

IGScripting = {}

function IGObject:new(...)
    local o = IGScripting:_classFromName(self.className)
    o:init(unpack(arg))
    return o
end

function IGObject:subclass(name)
    local o = { className = name, __index = self }
    setmetatable(o, o)
    return o
end

function IGObject:init()
    return self
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
        v(unpack(arg))
    end
end
onFrame = IGCallbackTable:new()

IGProp = IGObject:subclass("IGProp")
IGImageProp = IGProp:subclass("IGImageProp")
IGInput = IGObject:subclass("IGInput")
IGDampingProp = IGProp:subclass("IGDampingProp")
IGPlayer = IGProp:subclass("IGPlayer")
IGMob = IGDampingProp:subclass("IGMob")
IGBasicMob = IGDampingProp:subclass("IGBasicMob")
IGBulletManager = IGObject:subclass("IGBulletManager")
IGScreenBounds = IGObject:subclass("IGScreenBounds")


function frame(elapsed)
    onFrame:call(elapsed)
end
