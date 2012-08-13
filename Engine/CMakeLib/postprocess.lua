externals_dir = arg[4]
local src = arg[1]
local dst = arg[2]
local action = arg[3]
downscale_hd = (action ~= 'build-nodownscale')
if (action == 'clean') then
  os.execute('rm -r "'..dst..'"')
  return 0
end

function check_versions()
    local file = io.popen("/usr/bin/stat --version 2>/dev/null")
    local ver = file:read("*a")
    file:close()
    if string.find(ver,"GNU") then
        gnu_stat = true
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

function dir_make(path)
  os.execute('mkdir -p "'..path..'"')
end

function file_modif(file)
  local c = modif_cache[file]
--  if (c) then 
--    return c
--  end
--  local fl = io.popen('/bin/test -r "'..file..'" && /usr/bin/stat -f "%m" "'..file..'"')
--  local line = fl:lines()()
--  fl:close()
--  c = tonumber(line) or 0
--  modif_cache[file] = c
--  return c
  return c or 0
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
  if (file_modif(src) > file_modif(dst)) then
    if name then
        print('Copying file "'..name..'"')
    end
    os.execute('cp -a "'..src..'" "'..dst..'"')
  end
end

function compress_script(src, dst, name)
  if (file_modif(src) > file_modif(dst)) then
    name = name or f
    print('Compressing script "'..name..'"')
    local scriptline = 'cd "'..externals_dir..'/LuaSrcDiet" && '..externals_dir.."/lua/src/lua".." "..externals_dir..'/LuaSrcDiet/LuaSrcDiet.lua "'..src..'" -o "'..dst..'" --maximum >> /dev/null';
  	os.execute(scriptline)
  end
end

function downscale_png(srcd, dstd, f, stp, name)
  name = name or f
  if (downscale_hd) then
    local fhd = string.gsub(f, ".png$", ".hd.png");
    local fm = file_modif(srcd.."/"..f)
    local dm = file_modif(dstd.."/"..f)
    local dmh= file_modif(dstd.."/"..fhd)
    if ((fm > dm) or (fm > dmh)) then
        if (stp == 'l') then
          print('Symlinking image "'..name..'"')
          local fl = io.popen('readlink "'..srcd.."/"..f..'"')
          local link = fl:lines()()
          fl:close()
          local lihd = string.gsub(link, ".png$", ".hd.png");
          local cmd = 'cd "'..dstd..'"'
          cmd = cmd..' && ln -sf "'..link..'" "'..dstd.."/"..f..'"'
          cmd = cmd..' && ln -sf "'..lihd..'" "'..dstd.."/"..fhd..'"'
          os.execute(cmd)
        else
          print('Downscaling image "'..name..'"')
      	  os.execute('gm convert "'..srcd.."/"..f..'" -resize 25% "png32:'..dstd.."/"..f..'"')
      	  os.execute('gm convert "'..srcd.."/"..f..'" -resize 50% "png32:'..dstd.."/"..fhd..'"')
  	    end
    end
  else
    copy_file(srcd.."/"..f, dstd.."/"..f, name)
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
  exec = 'gm convert -size '..tostring(w)..'x'..tostring(h)..' xc:transparent ' .. exec .. '"png32:'.. dst ..'"'

  return s, exec, w, h
end

function create_map(files, src, dst, hd)
  local i = 0
  local of = {}
  local ins = {}
  while files[tostring(i)..'.png'] == 'f' do
    local fn = tostring(i)..'.png'
    local exec = 'gm identify -format "%w %h" "'..src..'/'..fn..'"';
    local f = io.popen(exec)
    local x = f:read("*n")
    local y = f:read("*n")
    f:close()
    ins[i] = { fname = fn, w = x, h = y }
    i = i + 1
  end
  ins.n = i;
  
  local max, scale
  if downscale_hd then
    if hd then
      max = 2048
      scale = 0.5
    else
      max = 1024
      scale = 0.25
    end
  else
    max = 4096
    scale = 1
  end

  local s, cmd, w, h = layout_images(max, scale, src, ins, dst..'/map.png')

  s = 'textureMaps = {\n  { fname = "map.png", ' .. s .. ' }\n}\n'
  os.execute(cmd)

  return s, of
end


function animation_dir(src, dst, prefix, hd)
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

  local smod = 0
  for f,tp in pairs(fl) do
    local m = file_modif(src.."/"..f)
    if (smod < m) then
      smod = m
    end
  end

  local valid_files = nil

  if (smod > dmod) then
    print('Creating animation atlas "'..prefix..'"')
    local s, valid_files = create_map(fl, src, dst, hd)
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
        print('Removing file "'..prefix..f..'"')
        file_rm(dst.."/"..f)
      end
    end
  else
    for f,tp in pairs(dl) do
      if not ((tp == "f") and ((file_extension(f) == "png") or (f == "info.lua"))) then
        print('Removing file "'..prefix..f..'"')
        file_rm(dst.."/"..f)
      end
    end
  end
end

function anim_dir(src, dst, prefix)
  if (downscale_hd) then
    local fhd = string.gsub(dst, ".png$", ".hd.png")
    animation_dir(src, dst, prefix, false)
    animation_dir(src, fhd, prefix, true)
  else
    animation_dir(src, dst, prefix)
  end
end

function crawl_dir(src, dst, prefix)
  prefix = prefix or ""
  dir_make(dst)
  
  local sl = dir_list(src)
  local dl = dir_list(dst)
  
  sl[".DS_Store"]  = nil
  sl[".git"]  = nil
  sl[".svn"]  = nil
  sl["CVS"]  = nil
  
  for f,tp in pairs(dl) do
    local ff = f;
    local ext = file_extension(f)
    if (downscale_hd) then
      ff = string.gsub(ff, ".hd.png$", ".png");
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
        anim_dir(src..'/'..f, dst..'/'..f, prefix..f..'/')
      else
        crawl_dir(src..'/'..f, dst..'/'..f, prefix..f..'/')
      end
    elseif ((tp == "f") or (tp == "l")) then
      if (ext == "png") then
        downscale_png(src, dst, f, tp, prefix..f)
      elseif (ext == "lua") then
        if (tp == "l") then
          copy_file(src..'/'..f, dst..'/'..f, prefix..f)
        else
          compress_script(src..'/'..f, dst..'/'..f, prefix..f)
        end
      else
        copy_file(src..'/'..f, dst..'/'..f, prefix..f)
      end
    end
  end
end

crawl_dir(src, dst)
