
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
    local o = { className = name }
    o.__index = self
    setmetatable(o, o)
    return o
end

function IGObject:init()
    return self
end

function vec2(x, y)
    return { x = x, y = y }
end
function rect(x, y, w, h)
    return { x = x, y = y, width = w, height = h }
end

IGProp = IGObject:subclass("IGProp")

IGImageProp = IGProp:subclass("IGImageProp")

function frame(elapsed)
--    print("frame", elapsed)
end
