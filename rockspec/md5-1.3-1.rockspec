package = "MD5"
version = "1.3-1"
source = {
   url = "https://github.com/keplerproject/md5/archive/1.3.tar.gz",
   md5 = "17b773591141091a4104d5183aad134b",
   dir = "md5-1.3",
}
description = {
   summary = "Checksum library",
   detailed = [[MD5 offers checksum facilities for Lua 5.X: a hash (digest) function, a pair crypt/decrypt based on MD5 and CFB, and a pair crypt/decrypt based on DES with 56-bit keys.]],
   license = "MIT/X11",
   homepage = "http://keplerproject.github.io/md5/",
}
dependencies = {
   "lua >= 5.0"
}
build = {
   type = "builtin",
   modules = {
      md5 = "src/md5.lua",
      ["md5.core"] = {
         sources = { "src/compat-5.2.c", "src/md5.c", "src/md5lib.c", },
         incdirs = { "src/", },
      },
      des56 = {
         sources = { "src/compat-5.2.c", "src/des56.c", "src/ldes56.c", },
         incdirs = { "src/", },
      },
   },
   copy_directories = { "doc", "tests", },
}
