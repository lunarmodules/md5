----------------------------------------------------------------------------
-- $Id: md5.lua,v 1.3 2006/08/21 17:54:36 carregal Exp $
----------------------------------------------------------------------------

local core = require"md5.core"
local string = require"string"

module ("md5")

_COPYRIGHT   = "Copyright (C) 2006 PUC-Rio"
_DESCRIPTION = "MD5 offers basic cryptographic facilities for Lua 5.0: a hash (digest) function, an a pair crypt/decrypt."
_VERSION     = "MD5 1.0.1"
----------------------------------------------------------------------------
-- @param k String with original message.
-- @return String with the md5 hash value converted to hexadecimal digits

function sumhexa (k)
  k = core.sum(k)
  return (string.gsub(k, ".", function (c)
           return string.format("%02x", string.byte(c))
         end))
end
