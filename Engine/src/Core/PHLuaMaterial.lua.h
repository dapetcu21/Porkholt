static const char * luaEnvironmentInit = "\n\
metastack = {}\n\
\n\
function vec2(a, b) local r = {a, b} setmetatable(r, metastack) return r end\n\
function vec3(a, b, c) local r = {a, b, c} setmetatable(r, metastack) return r end\n\
function vec4(a, b, c , d) local r = {a, b, c, d} setmetatable(r, metastack) return r end\n\
\n\
function metastack.gettype(a)\n\
    local lt = type(a)\n\
    if (lt == \"number\") then\n\
        return \"f\"\n\
    elseif (lt == \"table\") then\n\
        if rawget(a, \"tp\") then\n\
            return a.tp\n\
        end\n\
        local t = nil\n\
        for i=1,4 do\n\
            if type(rawget(a, i)) == \"number\" then\n\
                t = 'v'..i\n\
            else\n\
                break\n\
            end\n\
        end\n\
        if t == 'v1' then\n\
            t = nil\n\
        end\n\
        return t\n\
    end\n\
    return nil\n\
end\n\
\n\
function metastack.isstack(a)\n\
    return (type(a) == \"table\") and (type(a.stack) == \"table\")\n\
end\n\
\n\
fv14 = {f=1, v2=1, v3=1, v4=1}\n\
function metastack.addsub(a, b, op)\n\
    local at = metastack.gettype(a) \n\
    local bt = metastack.gettype(b) \n\
    if (at == bt) and (fv14[at]) then\n\
        return metastack.operate(op..\"_\"..at..\"_\"..bt, a, b)\n\
    end\n\
    return nil\n\
end\n\
\n\
function metastack.__add(a, b) \n\
    return metastack.addsub(a, b, \"plus\")\n\
end\n\
\n\
function metastack.__sub(a, b)\n\
    return metastack.addsub(a, b, \"minus\")\n\
end\n\
\n\
fv14m = {f=1, v2=1, v3=1, v4=1, mat=1}\n\
function metastack.multdiv(a, b, op)\n\
    local at = metastack.gettype(a) \n\
    local bt = metastack.gettype(b) \n\
    if (at == bt) and fv14m[at] then\n\
        return metastack.operate(op..\"_\"..at..\"_\"..bt, a, b)\n\
    elseif (bt == \"f\") and fv14[at] then\n\
        return metastack.operate(op..\"_\"..at..\"_\"..bt, a, b)\n\
    end\n\
    return (op == \"multiply\") or nil and metastack.multdiv(b, a, op)\n\
end\n\
\n\
function metastack.__mul(a, b)\n\
    return metastack.multdiv(a, b, \"multiply\")\n\
end\n\
\n\
function metastack.__div(a, b)\n\
    return metastack.multdiv(a, b, \"divide\")\n\
end\n\
\n\
metastack.props = { f = {}, v2 = {}, v3 = {}, v4 = {}, mat = {} }\n\
\n\
function metastack.props.mat.transposed(t)\n\
    return metastack.operate(\"transposed_mat\", t)\n\
end\n\
\n\
function metastack.props.mat.inverse(t)\n\
    return metastack.operate(\"inverse_mat\", t)\n\
end\n\
\n\
function metastack.__index(t, key)\n\
   local at = metastack.gettype(t)\n\
   local f = metastack.props[at][key]\n\
   if f then\n\
       return f(t)\n\
   else\n\
       return nil\n\
   end\n\
end\n\
\n\
function metastack.operate(op, ...)\n\
    local is = {}\n\
    local any = false\n\
    for i,v in ipairs(arg) do\n\
        is[i] = metastack.isstack(v)\n\
        any = any or is[i]\n\
    end\n\
    if not any then\n\
        return ops[op](unpack(arg))\n\
    else\n\
        local o = { stack = { n = 0 } }\n\
        setmetatable(o, metastack)\n\
        local f = function (x) \n\
                local s = o.stack\n\
                s.n = s.n + 1\n\
                s[s.n] = x\n\
            end\n\
        local add = function(a, as)\n\
            if as then\n\
                for i,v in ipairs(a.stack) do\n\
                    f(v)\n\
                end\n\
            else\n\
                f(a)\n\
            end\n\
       end\n\
       for i,v in ipairs(arg) do\n\
           add(v, is[i])\n\
       end\n\
       f({ op = ops_c[op] })\n\
       o.tp = ops_r[op]\n\
       return o\n\
    end\n\
end\n\
\n\
function metastack.variable(index, t)\n\
    local o = { tp = t, stack = { { var = index }, n=1 } }\n\
    setmetatable(o, metastack)\n\
    return o\n\
end\n\
\n\
-- OPS:\n\
ops = {}\n\
function ops.plus_f_f(a, b) return a+b end\n\
function ops.plus_v2_v2(a, b) return vec2(a[1]+b[1], a[2]+b[2]) end\n\
function ops.plus_v3_v3(a, b) return vec3(a[1]+b[1], a[2]+b[2], a[3]+b[3]) end\n\
function ops.plus_v4_v4(a, b) return vec4(a[1]+b[1], a[2]+b[2], a[3]+b[3], a[4]+b[4]) end\n\
\n\
function ops.minus_f_f(a, b) return a-b end\n\
function ops.minus_v2_v2(a, b) return vec2(a[1]-b[1], a[2]-b[2]) end\n\
function ops.minus_v3_v3(a, b) return vec3(a[1]-b[1], a[2]-b[2], a[3]-b[3]) end\n\
function ops.minus_v4_v4(a, b) return vec4(a[1]-b[1], a[2]-b[2], a[3]-b[3], a[4]-b[4]) end\n\
\n\
function ops.multiply_f_f(a, b) return a*b end\n\
function ops.multiply_v2_f(a, b) return vec2(a[1]*b, a[2]*b) end\n\
function ops.multiply_v2_v2(a, b) return vec2(a[1]*b[1], a[2]*b[2]) end\n\
function ops.multiply_v3_f(a, b) return vec3(a[1]*b, a[2]*b, a[3]*b) end\n\
function ops.multiply_v3_v3(a, b) return vec3(a[1]*b[1], a[2]*b[2], a[3]*b[3]) end\n\
function ops.multiply_v4_f(a, b) return vec4(a[1]*b, a[2]*b, a[3]*b, a[4]*b) end\n\
function ops.multiply_v4_v4(a, b) return vec4(a[1]*b[1], a[2]*b[2], a[3]*b[3], a[4]*b[4]) end\n\
\n\
function ops.divide_f_f(a, b) return a/b end\n\
function ops.divide_v2_f(a, b) return vec2(a[1]/b, a[2]/b) end\n\
function ops.divide_v2_v2(a, b) return vec2(a[1]/b[1], a[2]/b[2]) end\n\
function ops.divide_v3_f(a, b) return vec3(a[1]/b, a[2]/b, a[3]/b) end\n\
function ops.divide_v3_v3(a, b) return vec3(a[1]/b[1], a[2]/b[2], a[3]/b[3]) end\n\
function ops.divide_v4_f(a, b) return vec4(a[1]/b, a[2]/b, a[3]/b, a[4]/b) end\n\
function ops.divide_v4_v4(a, b) return vec4(a[1]/b[1], a[2]/b[2], a[3]/b[3], a[4]/b[4]) end\n\
";
