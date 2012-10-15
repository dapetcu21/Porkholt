
IGObject = { 
    className = "IGObject"
}

function IGObject:new(...)
    local o = _classFromName(self.className)
    setmetatable(o, o)
    o.__index = self
    return self.init(o, unpack(arg))
end

function IGObject:subclass(name)
    local o = { className = name }
    setmetatable(o, o)
    o.__index = self
    return o
end

function IGObject:init()
    return self
end

IGProp = IGObject:subclass("IGProp")

IGImageProp = IGProp:subclass("IGImageProp")

