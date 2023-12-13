package.cpath = package.cpath .. ";/opt/local/share/luarocks/lib/lua/5.3/?.so"

local des56 = require 'des56'

if #arg < 3 then
  error("usage: des.lua encrypt|decrypt <msg> <key>")
end

local cmd = arg[1]
local msg = arg[2]
local key = arg[3]

local r = nil

function string.to_hex(str)
    return (str:gsub('.', function (c)
        return string.format('%02x', string.byte(c))
    end))
end

function string.from_hex(str)
    return (str:gsub('..', function (cc)
        return string.char(tonumber(cc, 16))
    end))
end

if cmd == 'encrypt' then
  r = des56.crypt(msg:from_hex(), key:from_hex())
elseif cmd == 'decrypt' then
  r = des56.decrypt(msg:from_hex(), key:from_hex())
else
  error("usage: des.lua encrypt|decrypt <msg> <key>")
end

print(r:to_hex())
