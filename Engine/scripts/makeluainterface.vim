'<,'>s/\s*\(.\{-})\).*/\1/
'<,'>s/\([^(,)]*\) \w*\([,)]\s*\)/(\1)\2/g
'<,'>s/\(.\{-}\) \(\w*\)(\(.*\))/PHLuaMethod (PHClass, \2, \1, \3)/
'<,'>s/, )/)/
'<,'>s/PHLuaMethod \((\w*, \w*\), void\(.*\)/PHLuaMethodV\1\2

 
