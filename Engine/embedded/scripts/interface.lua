PHUDMeta = {}

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

PHLuaSetUpContext()

PHUDMeta.__gc = PHUDMetaReleaseObject

function PHObject:isKindOfClass(class)
  if not class then
    return false
  end
  while self and self ~= class do
    self = self.__index
  end
  return self == class
end


