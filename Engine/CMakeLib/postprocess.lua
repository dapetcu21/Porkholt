externals_dir = arg[4]
local src = arg[1]
local dst = arg[2]
local action = arg[3]
downscale_hd = (action ~= 'build-nodownscale')
if (action == 'clean') then
  os.execute('rm -r "'..dst..'"')
  return 0
end

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
  file:close();
  local s = '/usr/bin/stat -f "%m" '
  for line,t in pairs(files) do
    s = s..'"'..dir.."/"..line..'" '
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
  local pos = string.find(f, "%.")
  local ext
  if pos then
    ext = string.sub(f, pos+1)
  else
    ext = ""
  end
  return ext
end


function copy_file(src, dst, name)
  if (file_modif(src) > file_modif(dst)) then
    name = name or f
    print('Copying file "'..name..'"')
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
    local fm = file_modif(srcd.."/"..f)
    local dm = file_modif(dstd.."/"..f)
    local dmh= file_modif(dstd.."/"..f..".hd")
    if ((fm > dm) or (fm > dmh)) then
        if (stp == 'l') then
          print('Symlinking image "'..name..'"')
          local fl = io.popen('readlink "'..srcd.."/"..f..'"')
          local link = fl:lines()()
          fl:close()
          local cmd = 'cd "'..dstd..'"'
          cmd = cmd..' && ln -sf "'..link..'"    "'..dstd.."/"..f..'"'
          cmd = cmd..' && ln -sf "'..link..'.hd" "'..dstd.."/"..f..'.hd"'
          os.execute(cmd)
        else
          print('Downscaling image "'..name..'"')
      	  local flags="-quality 5 -channel RGBA -depth 24 -colorspace RGB"
      	  os.execute('convert "'..srcd.."/"..f..'" '..flags..' -resize 50% "png32:'..dstd.."/"..f..'"')
      	  os.execute('convert "'..srcd.."/"..f..'" '..flags..' -resize 25% "png32:'..dstd.."/"..f..'.hd"')
  	    end
    end
  else
    copy_file(srcd.."/"..f, dstd.."/"..f, name)
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
      if (ext == "png.hd") then
        ff = file_base(f)..".png"
      elseif (ext == "png.nmap.hd") then
        ff = file_base(f)..".png.nmap"
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
      crawl_dir(src..'/'..f, dst..'/'..f, prefix..f..'/')
    elseif ((tp == "f") or (tp == "l")) then
      if ((ext == "png") or (ext == "png.nmap")) then
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