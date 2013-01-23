#!/usr/bin/env lua
function exec(x)
  local file = io.popen(x)
  local s = file:read("*a")
  file:close()
  return s;
end

function strip_n(x)
  if (string.sub(x, -1) == '\n') then
    x = string.sub(x, 1, -2)
  end
  return x
end

self_p = strip_n(exec('readlink "'..arg[0]..'"'))
if (#self_p == 0) then
  self_p = arg[0]
end
script_path = strip_n(exec('dirname "'..self_p..'"'))
engine_path = script_path..'/..'

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
  return files
end

function crawl_dir(src, prefix)
  prefix = prefix or ""


  local sl = dir_list(src)
  sl[".DS_Store"] = nil
  sl[".git"] = nil
  sl[".svn"] = nil
  sl["CVS"] = nil
 
  for f,tp in pairs(sl) do
    local ext = f:sub(-3)
    if (f:sub(1,1) == '.' and (ext == 'swp' or ext == 'swo')) or f:sub(-1) == '~' then
      sl[f] = nil 
    end
  end

  for f,tp in pairs(sl) do
    if (tp == "d") then
      cpp_file:write('PHEmbedDir("', prefix..f, '");\n\n')
      crawl_dir(src..'/'..f, prefix..f..'/')
    elseif (tp == "f") then
      escape_file(src..'/'..f, prefix..f)
    end
  end

end

cpp_file = io.open(engine_path..'/src/Core/PHEmbeddedData.cpp', 'w')
h_file = io.open(engine_path..'/include/Porkholt/Core/PHEmbeddedData.h', 'w')

cpp_file:write(
[[/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#include <Porkholt/Core/PHEmbeddedData.h>

map<PHHashedString, pair<const char*, size_t> > * PHEmbedFiles = NULL;

]])
h_file:write(
[[/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */
#ifndef PHEMBEDDEDDATA_H
#define PHEMBEDDEDDATA_H

#include <Porkholt/Core/PHMain.h>

]])

function escape_file(file, path)
  local name = path:gsub("[^%w]", "_"):gsub("^(%d)", "_%1")

  local f = io.open(file, "rb");
  local s = f:read("*a");
  f:close()
  local nr = #s

  local count = 0
  local number = false
  s = s:gsub('.', function(ch)
    local r;
    local ln = number
    number = false
    if ch == "'" then
      r = "\\'"
      count = count + 2
    elseif ch == '"' then
      r ='\\"'
      count = count + 2
    elseif ch == '?' then
      r = '\\?'
      count = count + 2
    elseif ch == '\\' then
      r = '\\\\'
      count = count + 2
    elseif ch == '\n' then
      r = '\\n\\\n'
      count = 0
    elseif ch == '\a' then
      r = '\\a'
      count = count + 2
    elseif ch == '\0' then
      r = '\\0'
      number = true
      count = count + 2
    elseif ch == '\t' then
      r = '\\t'
      count = count + 2
    elseif ch == '\b' then
      r = '\\b'
      count = count + 2
    elseif ch == '\v' then
      r = '\\v'
      count = count + 2
    elseif ch == '\f' then
      r = '\\f'
      count = count + 2
    elseif ch == '\r' then
      r = '\\r'
      count = count + 2
    elseif ch:find("%d") then
      if ln then
        r = '""'..ch
        count = count + 3
      else
        r = ch
        count = count + 1
      end
    elseif ch:find("[%l%u%p%s]") then
      r = ch
      count = count + 1
    else
      r = string.format('\\%.3o', ch:byte()) 
      count = count + 4
      number = true
    end
    if count >= 80 then
      r = '\\\n'..r
      count = 0
    end
    return r
  end)

  cpp_file:write('PHEmbedFile(', name, ', "', path, '", ', nr, ',\n"', s, '");\n\n')
  h_file:write('extern const char   ', name, '[];\nextern const size_t ', name, '_size;\n\n')
end

cpp_file:write('PHEmbedDir("");\n\n')
crawl_dir(engine_path..'/embedded')
cpp_file:close()
h_file:write([[

extern map<PHHashedString, pair<const char*, size_t> > * PHEmbedFiles;

#define PHEmbedFile_(ident, path, size) \
PH_INITIALIZER(PH_TOKENPASTE(PHEmbedInit_, PH_UNIQUE_TOKEN)) \
{ \
    if (!PHEmbedFiles) \
        PHEmbedFiles = new map<PHHashedString, pair<const char*, size_t> >; \
    PHEmbedFiles->insert(make_pair(PHHashedString(path), make_pair(ident, size))); \
}

#define PHEmbedFile(ident, path, size, contents) \
const char   ident[] = contents; \
const size_t ident ## _size = size; \
PHEmbedFile_(ident, path, size)

#define PHEmbedDir(path) PHEmbedFile_((const char*)NULL, path, 0)

#endif
]])

h_file:close()
