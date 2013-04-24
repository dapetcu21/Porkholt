#!/usr/bin/env lua

package.path = package.path..";"..string.gsub(arg[0], "(/?)[^/]*%.lua$", "%1?.lua")
require("path")
require("platforms")
require("bakeapi")

local src = arg[1]
local dst = arg[2]
externals_dir = arg[3]
platform = platforms[arg[4]]

src = path.getabsolute(src)
dst = path.getabsolute(dst)
root_src = src
root_dst = dst

fake_links = platform.fake_links
luacompress = true


local i = 5
while arg[i] ~= nil do
  local a = arg[i]
  if (a == "fakesymlinks") then
    fake_links = true
  elseif (a == "nofakesymlinks") then
    fake_links = false 
  elseif (a == "luacompress") then
    luacompress = true
  elseif (a == "noluacompress") then
    luacompress = false
  elseif (a == "clean") then
    os.execute('rm -r "'..dst..'"')
    return 0
  end
  i = i + 1
end

function check_versions()
  local file = io.popen("/usr/bin/stat --version 2>/dev/null")
  local ver = file:read("*a")
  file:close()
  if string.find(ver,"GNU") then
    gnu_stat = true
  end
  file = io.popen("which gm")
  gm_exe = file:read("*a")
  local n = string.len(gm_exe)
  if (string.sub(gm_exe, -1) == "\n") then
    gm_exe = string.sub(gm_exe, 1, -2)
    n = n - 1
  end
  if (n == 0) then
    gm_exe = "/opt/local/bin/gm" --For OS X With MacPorts
  end
end

check_versions()

modif_cache = {}
function dir_list(dir)
  local file = io.popen("/bin/ls -1AF "..'"'..dir..'"' )
  local files = {}
  for line in file:lines() do
    suf = string.sub(line, -1)
    if (suf == "/") then
      files[string.sub(line, 1, -2)] = "d"
    elseif (suf == "@") then
      files[string.sub(line, 1, -2)] = "l"
    elseif (suf == "*") then
      files[string.sub(line, 1, -2)] = "f"
    elseif ((suf ~= "=") and (suf ~= "%") and (suf ~= "|")) then
      files[line] = "f"
    end
  end
  file:close()
  local s
  if gnu_stat then
    s = '/usr/bin/stat --format="%Y" '
  else
    s = '/usr/bin/stat -f "%m" '
  end
  local b
  for line,t in pairs(files) do
    b = true
    s = s..'"'..dir.."/"..line..'" '
  end
  if not b then
    return files
  end
  local file = io.popen(s)
  local ln = file:lines()
  for line,t in pairs(files) do
    modif_cache[dir.."/"..line] = tonumber(ln()) or 0
  end
  file:close()
  return files
end

function max(a, b)
  if a > b then
    return a
  else
    return b
  end
end

function file_exists(file)
  local v1,v2,v3 = os.execute('/bin/ls "'..file..'" 1>/dev/null 2>/dev/null')
  return ((v1 == true) and (v2 == "exit") and (v3 == 0)) or ((v1 == 0) and (v2 == nil) and (v3 == nil))
end

function dir_make(path)
  --print('Making directory "'..path..'"')
  os.execute('mkdir -p "'..path..'"')
end

function file_modif(file)
  return modif_cache[file] or 0
end

function file_rm(file)
  os.execute('rm -r "'..file..'"')
end

function file_base(f)
  local pos = string.find(f, "%.")
  local ext
  if pos then
    ext = string.sub(f, 1, pos-1)
  else
    ext = pos
  end
  return ext
end

function file_extension(f)
  local pos1, pos2, ext = string.find(f, "%.([^%.][^%.]-)$")
  return ext
end


function copy_file(src, dst, name)
  if (max(file_modif(src), bake_modif) > file_modif(dst)) then
    if name then
      print('Copying file "'..name..'"')
    end
    os.execute('cp -a "'..src..'" "'..dst..'"')
    os.execute('touch "'..dst..'"')
  end
end

function compress_script(src, dst, name)
  if (not luacompress) then
    copy_file(src, dst, name)
  else
    if (max(file_modif(src), bake_modif) > file_modif(dst)) then
      name = name or f
      if jit then
        print('Compiling script "'..name..'" to LuaJIT bytecode')
        local out = assert(io.open(dst, "wb"))
        local f = assert(loadfile(src))
        out:write(string.dump(f, true))
        out:close()
      else
        print('Compressing script "'..name..'"')
        local scriptline = 'cd "'..externals_dir..'/LuaSrcDiet" && lua '..externals_dir..'/LuaSrcDiet/LuaSrcDiet.lua "'..src..'" -o "'..dst..'" --maximum >> /dev/null'
        os.execute(scriptline)
      end
    end
  end
end

function link(destd, prefix, lk, f)
  if fake_links then
    local ns = prefix..f
    local nd = path.getabsolute(root_dst..'/'..prefix..lk)
    local n = string.len(root_dst)+1
    nd = string.gsub(string.sub(nd, n), '^/?', '')
    syml:write(ns..'\n'..nd..'\n\n')
  else
    local cmd = 'cd "'..destd..'" && ln -sf "'..lk..'" "'..f..'"'
    os.execute(cmd)
  end
end

function link_file(srcd, dstd, f, named)
  local fl = io.popen('readlink "'..srcd..'/'..f..'"')
  local lk = fl:lines()()
  fl:close()

  link(dstd, named, lk, f)
end

function scale_for_image(fname, screen, size)
  local d = scaling_descriptors[fname]
  local scale = screen.scale

  if d then
    local dw, dh
    if d.w_ratio ~= nil then
      dw = d.w_ratio * screen.w
    elseif d.h_ratio ~= nil then
      dh = d.h_ratio * screen.h
    elseif d.w_inch ~=nil then
      dw = d.w_inch * screen.ppi
    elseif d.h_inch ~=nil then
      dh = d.h_inch * screen.ppi
    elseif d.w_cm ~=nil then
      dw = d.w_cm * screen.ppi * 0.393701
    elseif d.h_cm ~=nil then
      dh = d.h_cm * screen.ppi * 0.393701
    elseif d.scale ~=nil then
      scale = d.scale
    end

    if dw ~= nil then
      scale = math.floor(dw + 0.5) / size.w
    elseif dh ~= nil then
      scale = math.floor(dh + 0.5) / size.h
    end

    if scale ~= nil and scale > 0.9 then
      scale = 1
    end
  end
  return scale
end

function downscale_png(srcd, dstd, f, stp, named)
  named = named or ""
  if (stp == 'l') then
    local fl = io.popen('readlink "'..srcd.."/"..f..'"')
    local lnk = fl:lines()()
    fl:close()

    for ext,screen in pairs(platform.screens) do
      local lhd = string.gsub(lnk, ".png$", ext..".png")
      local fhd = string.gsub(f, ".png$", ext..".png")
      link(dstd, named, lhd, fhd)
    end
  else
    local fm = max(file_modif(srcd.."/"..f), bake_modif)
    local downscale = false
    for ext,screen in pairs(platform.screens) do
      local fhd = string.gsub(f, ".png$", ext..".png")
      local dmh= file_modif(dstd.."/"..fhd)
      if (fm > dmh) then
        downscale = true
        break
      end
    end
    if downscale then
      local scales = {}
      local img_name = named..f
      print('Downscaling image "'..img_name..'"')
      local size = image_size(srcd..'/'..f)
      for ext,screen in pairs(platform.screens) do
        local fhd = string.gsub(f, ".png$", ext..".png")
        local scale = scale_for_image(img_name, screen, size)
        local ofhd = scales[scale]
        if ofhd then
          link(dstd, named, ofhd, fhd)
        else
          os.execute(string.format('%s convert "%s/%s" -resize %.4f%% "png32:%s/%s"', gm_exe, srcd, f, scale*100, dstd, fhd))
          scales[scale] = fhd
        end
      end
    end
  end
end

function layout_images(width, scale, src, ins, dst)
  local w = 0
  local h = 0
  local x = 0
  local y = 0
  local s = ''
  local exec = ''

  for i = 0, ins.n - 1 do
    local f = ins[i]
    local xx = f.w * scale
    local yy = f.h * scale
    if (x + xx > width) then
      y = h
      x = 0
    end
    if (w < x + xx) then
      w = x + xx
    end
    if (h < y + yy) then
      h = y + yy 
    end
    s = s .. '{ x = ' .. tostring(x) .. ', y = ' .. tostring(y) .. ', width = ' .. tostring(xx) .. ', height = ' .. tostring(yy) .. ' }, '
    exec = exec .. "-draw 'image Copy "..tostring(x)..','..tostring(y)..' '..tostring(xx)..','..tostring(yy)..' "' .. src .. '/' .. f.fname .. '"\' '
    x = x + xx
  end
  exec = gm_exe..' convert -size '..tostring(w)..'x'..tostring(h)..' xc:transparent ' .. exec .. '"png32:'.. dst ..'"'

  return s, exec, w, h
end

function image_size(file)
    local exec = gm_exe..' identify -format "%w %h" "'..file..'"'
    local f = io.popen(exec)
    local o = { w = f:read("*n"), h = f:read("*n") }
    f:close()
    return o
end

function create_map(files, src, dst, screen, name)
  local i = 0
  local file_list = {}
  local ins = {}
  local max_size = { w = 0, h = 0 }
  while files[tostring(i)..'.png'] == 'f' do
    local fn = tostring(i)..'.png'
    local size = image_size(src..'/'..fn)
    max_size.w = math.max(max_size.w, size.w)
    max_size.h = math.max(max_size.h, size.h)
    ins[i] = { fname = fn, w = size.w, h = size.h }
    file_list[fn] = true
    i = i + 1
  end
  ins.n = i
  
  if screen.max_texture == nil then
    local size = math.max(screen.w, screen.h)
    local b = 1
    while (b < size) do
      b = b * 2
    end
    screen.max_texture = b
  end
  
  local max = screen.max_texture
  local scale = scale_for_image(name, screen, max_size)
  local s, cmd, w, h = layout_images(max, scale, src, ins, dst..'/map.png')

  s = 'textureMaps = {\n  { fname = "map.png", ' .. s .. ' }\n}\n'
  os.execute(cmd)

  return s, file_list
end


function animation_dir(src, dst, prefix, screen)
  prefix = prefix or ""
  dir_make(dst)

  local dl = dir_list(dst)
  local fl = dir_list(src)
    
  local dmod = 0
  for f,tp in pairs(dl) do
    local m = file_modif(dst.."/"..f)
    if (dmod < m) then
      dmod = m
    end
  end

  local smod = bake_modif
  for f,tp in pairs(fl) do
    local m = file_modif(src.."/"..f)
    if (smod < m) then
      smod = m
    end
  end

  local valid_files = nil

  if (smod > dmod) then
    print('Creating animation atlas "'..prefix..'"')
    local s
    s, valid_files = create_map(fl, src, dst, screen, prefix)
    local file, err = io.open(src..'/info.lua', 'r')
    if (file) then
      s = s..file:read("*a")
      file:close()
      local of, erro = io.open(dst..'/info.lua', 'w')
      if (of) then
        of:write(s)
        of:close()
      else
        print(erro)
      end
    else
      print(err)
    end
  end

  if valid_files then
    for f,tp in pairs(dl) do
      if not valid_files[f] then
        print('Removing file "'..prefix..'/'..f..'"')
        file_rm(dst.."/"..f)
      end
    end
  else
    for f,tp in pairs(dl) do
      if not ((tp == "f") and ((file_extension(f) == "png") or (f == "info.lua"))) then
        print('Removing file "'..prefix..'/'..f..'"')
        file_rm(dst.."/"..f)
      end
    end
  end
end

function anim_dir(src, dst, prefix)
  for ext, screen in pairs(platform.screens) do
    local fhd = string.gsub(dst, ".png$", ext..".png")
    animation_dir(src, fhd, prefix, screen)
  end
end

bake_modif = 0

function crawl_dir(src, dst, prefix)
  prefix = prefix or ""
  dir_make(dst)
  
  local sl = dir_list(src)
  local dl = dir_list(dst)

  local bmod = bake_modif
  if sl["bakeconfig.lua"] then
    sl["bakeconfig.lua"] = nil
    local f, err = loadfile(src..'/bakeconfig.lua')

    local bbm = file_modif(src..'/bakeconfig.lua')
    if bbm > bmod then
      bmod = bbm
    end

    if not f then
      print(err)
    else
      current_src = src
      current_dst = dst
      current_prefix = prefix
      local status, err = pcall(f)
      if not status then
        print(err)
      end
    end
  end
  
  sl[".DS_Store"] = nil
  sl[".git"] = nil
  sl[".svn"] = nil
  sl["CVS"] = nil
  dl["_symlinks"] = nil

  local bmod_old = bake_modif
  bake_modif = bmod

  for f,tp in pairs(sl) do
    local ext = f:sub(-3)
    if (f:sub(1,1) == '.' and (ext == 'swp' or ext == 'swo')) or f:sub(-1) == '~' then
      sl[f] = nil 
    end
  end

  for f,tp in pairs(dl) do
    local ff = f
    local ext = file_extension(f)
    for ext,screen in pairs(platform.screens) do
      ff = string.gsub(ff, ext..".png$", ".png")
    end
    if (tp == "l") then
      local fl = io.popen('readlink "'..dst..'/'..f..'"')
      local lk = fl:lines()()
      fl:close()
      for ext,screen in pairs(platform.screens) do
        lk = string.gsub(lk, ext..".png$", ".png")
      end
      if lk == ff then
        tp = sl[ff]
      end
    end
    if (tp ~= sl[ff]) then
      print('Removing file "'..prefix..f..'"')
      file_rm(dst.."/"..f)
    end
  end
  
  for f,tp in pairs(sl) do
    local ext = file_extension(f)
    if (tp == "d") then
      if (ext == "png") then
        anim_dir(src..'/'..f, dst..'/'..f, prefix..f)
      else
        crawl_dir(src..'/'..f, dst..'/'..f, prefix..f..'/')
      end
    elseif (tp == "l") then
      if (ext == "png") then
        downscale_png(src, dst, f, tp, prefix)
      else
        link_file(src, dst, f, prefix)
      end
    elseif (tp == "f") then
      if (ext == "png") then
        downscale_png(src, dst, f, tp, prefix)
      elseif (ext == "lua") then
        compress_script(src..'/'..f, dst..'/'..f, prefix..f)
      else
        copy_file(src..'/'..f, dst..'/'..f, prefix..f)
      end
    end
  end

  bake_modif = bmod_old
end

if file_exists(src) then
  if fake_links then
    dir_make(dst)
    syml=io.open(dst..'/_symlinks', 'w')
  end
    
  crawl_dir(src, dst)

  if fake_links then
    syml:close()
  end
else
  if file_exists(dst) then
    print('Removing resource directory')
    file_rm(dst)
  end
end

