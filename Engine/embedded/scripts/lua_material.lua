
metastack = {}

function vec2(a, b) local r = {a, b} setmetatable(r, metastack) return r end
function vec3(a, b, c) local r = {a, b, c} setmetatable(r, metastack) return r end
function vec4(a, b, c , d) local r = {a, b, c, d} setmetatable(r, metastack) return r end

function metastack.gettype(a)
    local lt = type(a)
    if (lt == "number") then
        return "f"
    elseif (lt == "table") then
        if rawget(a, "tp") then
            return a.tp
        end
        local t = nil
        for i=1,4 do
            if type(rawget(a, i)) == "number" then
                t = 'v'..i
            else
                break
            end
        end
        if t == 'v1' then
            t = nil
        end
        return t
    end
    return nil
end

function metastack.isstack(a)
    return (type(a) == "table") and (type(a.stack) == "table")
end

fv14 = {f=1, v2=1, v3=1, v4=1}
function metastack.addsub(a, b, op)
    local at = metastack.gettype(a) 
    local bt = metastack.gettype(b) 
    if (at == bt) and (fv14[at]) then
        return metastack.operate(op.."_"..at.."_"..bt, a, b)
    end
    return nil
end

function metastack.__add(a, b) 
    return metastack.addsub(a, b, "plus")
end

function metastack.__sub(a, b)
    return metastack.addsub(a, b, "minus")
end

fv14m = {f=1, v2=1, v3=1, v4=1, mat=1}
function metastack.multdiv(a, b, op)
    local at = metastack.gettype(a) 
    local bt = metastack.gettype(b) 
    if (at == bt) and fv14m[at] then
        return metastack.operate(op.."_"..at.."_"..bt, a, b)
    elseif (bt == "f") and fv14[at] then
        return metastack.operate(op.."_"..at.."_"..bt, a, b)
    end
    return (op == "multiply") or nil and metastack.multdiv(b, a, op)
end

function metastack.__mul(a, b)
    return metastack.multdiv(a, b, "multiply")
end

function metastack.__div(a, b)
    return metastack.multdiv(a, b, "divide")
end

metastack.props = { f = {}, v2 = {}, v3 = {}, v4 = {}, mat = {} }

function metastack.props.mat.transposed(t)
    return metastack.operate("transposed_mat", t)
end

function metastack.props.mat.inverse(t)
    return metastack.operate("inverse_mat", t)
end

function metastack.__index(t, key)
   local at = metastack.gettype(t)
   local f = metastack.props[at][key]
   if f then
       return f(t)
   else
       return nil
   end
end

function metastack.operate(op, ...)
    local is = {}
    local any = false
    for i = 1, select("#",...) do
        local v = select(i,...)
        is[i] = metastack.isstack(v)
        any = any or is[i]
    end
    if not any then
        return ops[op](...)
    else
        local o = { stack = { n = 0 } }
        setmetatable(o, metastack)
        local f = function (x) 
                local s = o.stack
                s.n = s.n + 1
                s[s.n] = x
            end
        local add = function(a, as)
            if as then
                for i,v in ipairs(a.stack) do
                    f(v)
                end
            else
                f(a)
            end
       end
       for i = 1, select("#",...) do
           add(select(i,...), is[i])
       end
       f({ op = ops_c[op] })
       o.tp = ops_r[op]
       return o
    end
end

function metastack.variable(index, t)
    local o = { tp = t, stack = { { var = index }, n=1 } }
    setmetatable(o, metastack)
    return o
end

-- OPS:
ops = {}
function ops.plus_f_f(a, b) return a+b end
function ops.plus_v2_v2(a, b) return vec2(a[1]+b[1], a[2]+b[2]) end
function ops.plus_v3_v3(a, b) return vec3(a[1]+b[1], a[2]+b[2], a[3]+b[3]) end
function ops.plus_v4_v4(a, b) return vec4(a[1]+b[1], a[2]+b[2], a[3]+b[3], a[4]+b[4]) end

function ops.minus_f_f(a, b) return a-b end
function ops.minus_v2_v2(a, b) return vec2(a[1]-b[1], a[2]-b[2]) end
function ops.minus_v3_v3(a, b) return vec3(a[1]-b[1], a[2]-b[2], a[3]-b[3]) end
function ops.minus_v4_v4(a, b) return vec4(a[1]-b[1], a[2]-b[2], a[3]-b[3], a[4]-b[4]) end

function ops.multiply_f_f(a, b) return a*b end
function ops.multiply_v2_f(a, b) return vec2(a[1]*b, a[2]*b) end
function ops.multiply_v2_v2(a, b) return vec2(a[1]*b[1], a[2]*b[2]) end
function ops.multiply_v3_f(a, b) return vec3(a[1]*b, a[2]*b, a[3]*b) end
function ops.multiply_v3_v3(a, b) return vec3(a[1]*b[1], a[2]*b[2], a[3]*b[3]) end
function ops.multiply_v4_f(a, b) return vec4(a[1]*b, a[2]*b, a[3]*b, a[4]*b) end
function ops.multiply_v4_v4(a, b) return vec4(a[1]*b[1], a[2]*b[2], a[3]*b[3], a[4]*b[4]) end

function ops.divide_f_f(a, b) return a/b end
function ops.divide_v2_f(a, b) return vec2(a[1]/b, a[2]/b) end
function ops.divide_v2_v2(a, b) return vec2(a[1]/b[1], a[2]/b[2]) end
function ops.divide_v3_f(a, b) return vec3(a[1]/b, a[2]/b, a[3]/b) end
function ops.divide_v3_v3(a, b) return vec3(a[1]/b[1], a[2]/b[2], a[3]/b[3]) end
function ops.divide_v4_f(a, b) return vec4(a[1]/b, a[2]/b, a[3]/b, a[4]/b) end
function ops.divide_v4_v4(a, b) return vec4(a[1]/b[1], a[2]/b[2], a[3]/b[3], a[4]/b[4]) end
