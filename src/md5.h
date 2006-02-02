/**
*  $Id: md5.h,v 1.1 2006/02/02 21:11:12 tomas Exp $
*  Cryptographic module for Lua.
*  @author  Roberto Ierusalimschy
*/


#ifndef md5_h
#define md5_h

#include <lua.h>


#define HASHSIZE       16

void md5 (const char *message, long len, char *output);
int luaopen_md5 (lua_State *L);


#endif
