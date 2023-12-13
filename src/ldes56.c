#include <stdlib.h>
#include <string.h>

#include "des56.h"

#include "lua.h"
#include "lauxlib.h"

#include "compat-5.2.h"
#include "ldes56.h"

static char *des56_alloc(lua_State *L, size_t len)
{
  char *p = (char *) malloc( len * sizeof(char));
  if(p == NULL) {
    lua_pushstring(L, "des56_alloc: memory allocation failed");
    lua_error(L);
   }
  return p;
}

static int des56_set_key(lua_State *L, const char *key, size_t len, keysched *KS)
{
  char k[8];
  if (key && len >= 8)
  {
    memcpy(k, key, 8);
    fsetkey(k, KS);
  } else {
    lua_pushstring(L, "des56_set_key: invalid key");
    lua_error(L);
    return 0;
  }
  return 1;
}

static int des56_decrypt( lua_State *L )
{
  keysched KS;
  int i;
  size_t cipherLen;
  const char *cipherText =
    luaL_checklstring( L, 1, &cipherLen );
  size_t keyLen;
  const char *key =
    luaL_checklstring( L, 2, &keyLen );

  char* plainText = des56_alloc(L, cipherLen);
  if(plainText == NULL)
    return 0;
  if (des56_set_key(L, key, keyLen, &KS) == 0)
    return 0;

  // copy the cipherText into the decryption buffer
  memcpy(plainText, cipherText, cipherLen);

  // loop over the 8-byte blocks and encrypt them
  for (i=0; i<(int)cipherLen; i+=8)
  {
    fencrypt(&(plainText[i]), 1, &KS);
  }

  // PKCS#5 padding: last byte in the plainText is the pad length, remove those bytes
  lua_pushlstring(L, plainText, cipherLen - plainText[cipherLen-1]);
  free( plainText );

  return 1;
}

static int des56_crypt( lua_State *L )
{
  keysched KS;
  int i;
  size_t plainLen;
  const char *plainText = luaL_checklstring( L, 1, &plainLen );
  size_t keyLen;
  const char *key = luaL_checklstring( L, 2, &keyLen );
  int padLen = (8 - plainLen % 8);

  char* cipherText = des56_alloc(L, plainLen + padLen);
  if(cipherText == NULL)
    return 0;
  if (des56_set_key(L, key, keyLen, &KS) == 0)
    return 0;

  // copy the plainText into the encryption buffer
  memcpy(cipherText, plainText, plainLen);

  // PKCS#5 padding: add padLen bytes with padLen value (last byte matters)
  memset(&(cipherText[plainLen]), padLen, padLen);

  // loop over the 8-byte blocks and decrypt them
  for (i=0; i<(int)plainLen+1; i+=8)
  {
    fencrypt(&(cipherText[i]), 0, &KS);
  }

  // return the encrypted padded string
  lua_pushlstring( L, cipherText, plainLen + padLen);
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
