#include <stdlib.h>
#include <string.h>

#include "des56.h"

#include "lua.h"
#include "lauxlib.h"

#include "compat-5.2.h"
#include "ldes56.h"

static int des56_decrypt( lua_State *L )
{
  keysched KS;
  int i;
  size_t cypherlen;
  const char *cypherText =
    luaL_checklstring( L, 1, &cypherlen );
  size_t keylen;
  const char *key =
    luaL_checklstring( L, 2, &keylen );

  char* plainText =
    (char *) malloc( (cypherlen + 1) * sizeof(char));
  if(plainText == NULL) {
    lua_pushstring(L, "Error decrypting: memory allocation failed.");
    lua_error(L);
  }

  if (key && keylen >= 8)
  {
    char k[8];
    memcpy(k, key, 8);
    fsetkey(k, &KS);
  } else {
    lua_pushstring(L, "Error decrypting: invalid key.");
    lua_error(L);
  }

  memcpy(plainText, cypherText, cypherlen);
  for (i=0; i<(int)cypherlen; i+=8)
  {
    fencrypt(&(plainText[i]), 1, &KS);
  }

  lua_pushlstring(L, plainText, cypherlen - plainText[cypherlen-1]);

  free( plainText );
  return 1;
}

static int des56_crypt( lua_State *L )
{
  char *cipherText;
  keysched KS;
  int i;
  size_t plainlen;
  const char *plainText = luaL_checklstring( L, 1, &plainlen );
  size_t keylen;
  const char *key = luaL_checklstring( L, 2, &keylen );
  int pad = (8 - plainlen % 8);

  cipherText = (char *) malloc( (plainlen+pad) * sizeof(char));
  if(cipherText == NULL) {
    lua_pushstring(L, "Error encrypting: memory allocation failed.");
    lua_error(L);
  }

  if (key && keylen >= 8)
  {
    char k[8];
	memcpy(k, key, 8);
    fsetkey(k, &KS);
  } else {
    lua_pushstring(L, "Error encrypting: invalid key.");
    lua_error(L);
  }

  memcpy(cipherText, plainText, plainlen);
  memset(&(cipherText[plainlen]), pad, pad);
  for (i=0; i<(int)plainlen+1; i+=8)
  {
    fencrypt(&(cipherText[i]), 0, &KS);
  }

  lua_pushlstring( L, cipherText, plainlen+pad);
  free( cipherText );
  return 1;
}

/*
** Assumes the table is on top of the stack.
*/
static void set_info (lua_State *L) {
	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, "Copyright (C) 2007-2019 PUC-Rio");
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, "DES 56 cryptographic facilities for Lua");
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, "DES56 1.3");
	lua_settable (L, -3);
}

static const struct luaL_Reg des56lib[] = {
  {"crypt", des56_crypt},
  {"decrypt", des56_decrypt},
  {NULL, NULL},
};

int luaopen_des56 (lua_State *L) {
  lua_newtable(L);
  luaL_setfuncs(L, des56lib, 0);
  set_info (L);
  return 1;
}
