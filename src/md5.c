/**
*  $Id: md5.c,v 1.2 2008/03/24 20:59:12 mascarenhas Exp $
*  Hash function MD5
*  @author  Marcela Ozorio Suarez, Roberto I.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <lua.h>
#include <lauxlib.h>

#if ! defined (LUA_VERSION_NUM) || LUA_VERSION_NUM < 501
#include "compat-5.1.h"
#endif


#include "md5.h"


#define WORD 32
#define MASK 0xFFFFFFFF
#if __STDC_VERSION__ >= 199901L
#include <stdint.h>
typedef uint32_t WORD32;
#else
typedef unsigned int WORD32;
#endif


/**
*  md5 hash function.
*  @param message: aribtary string.
*  @param len: message length.
*  @param output: buffer to receive the hash value. Its size must be
*  (at least) HASHSIZE.
*/
void md5 (const char *message, long len, char *output);


/*
** Realiza a rotacao no sentido horario dos bits da variavel 'D' do tipo WORD32.
** Os bits sao deslocados de 'num' posicoes
*/
#define rotate(D, num)  (D<<num) | (D>>(WORD-num))

/*Macros que definem operacoes relizadas pelo algoritmo  md5 */
#define F(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~(z))))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~(z))))


/*vetor de numeros utilizados pelo algoritmo md5 para embaralhar bits */
static const WORD32 T[64]={
                     0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                     0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                     0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                     0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                     0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                     0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                     0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                     0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                     0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                     0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                     0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                     0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                     0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                     0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                     0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                     0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};


static void word32tobytes (const WORD32 *input, char *output) {
  int j = 0;
  while (j<4*4) {
    WORD32 v = *input++;
    output[j++] = (char)(v & 0xff); v >>= 8;
    output[j++] = (char)(v & 0xff); v >>= 8;
    output[j++] = (char)(v & 0xff); v >>= 8;
    output[j++] = (char)(v & 0xff);
  }
}


static void inic_digest(WORD32 *d) {
  d[0] = 0x67452301;
  d[1] = 0xEFCDAB89;
  d[2] = 0x98BADCFE;
  d[3] = 0x10325476;
}


/*funcao que implemeta os quatro passos principais do algoritmo MD5 */
static void digest(const WORD32 *m, WORD32 *d) {
  int j;
  /*MD5 PASSO1 */
  for (j=0; j<4*4; j+=4) {
    d[0] = d[0]+ F(d[1], d[2], d[3])+ m[j] + T[j];       d[0]=rotate(d[0], 7);
    d[0]+=d[1];
    d[3] = d[3]+ F(d[0], d[1], d[2])+ m[(j)+1] + T[j+1]; d[3]=rotate(d[3], 12);
    d[3]+=d[0];
    d[2] = d[2]+ F(d[3], d[0], d[1])+ m[(j)+2] + T[j+2]; d[2]=rotate(d[2], 17);
    d[2]+=d[3];
    d[1] = d[1]+ F(d[2], d[3], d[0])+ m[(j)+3] + T[j+3]; d[1]=rotate(d[1], 22);
    d[1]+=d[2];
  }
  /*MD5 PASSO2 */
  for (j=0; j<4*4; j+=4) {
    d[0] = d[0]+ G(d[1], d[2], d[3])+ m[(5*j+1)&0x0f] + T[(j-1)+17];
    d[0] = rotate(d[0],5);
    d[0]+=d[1];
    d[3] = d[3]+ G(d[0], d[1], d[2])+ m[((5*(j+1)+1)&0x0f)] + T[(j+0)+17];
    d[3] = rotate(d[3], 9);
    d[3]+=d[0];
    d[2] = d[2]+ G(d[3], d[0], d[1])+ m[((5*(j+2)+1)&0x0f)] + T[(j+1)+17];
    d[2] = rotate(d[2], 14);
    d[2]+=d[3];
    d[1] = d[1]+ G(d[2], d[3], d[0])+ m[((5*(j+3)+1)&0x0f)] + T[(j+2)+17];
    d[1] = rotate(d[1], 20);
    d[1]+=d[2];
  }
  /*MD5 PASSO3 */
  for (j=0; j<4*4; j+=4) {
    d[0] = d[0]+ H(d[1], d[2], d[3])+ m[(3*j+5)&0x0f] + T[(j-1)+33];
    d[0] = rotate(d[0], 4);
    d[0]+=d[1];
    d[3] = d[3]+ H(d[0], d[1], d[2])+ m[(3*(j+1)+5)&0x0f] + T[(j+0)+33];
    d[3] = rotate(d[3], 11);
    d[3]+=d[0];
    d[2] = d[2]+ H(d[3], d[0], d[1])+ m[(3*(j+2)+5)&0x0f] + T[(j+1)+33];
    d[2] = rotate(d[2], 16);
    d[2]+=d[3];
    d[1] = d[1]+ H(d[2], d[3], d[0])+ m[(3*(j+3)+5)&0x0f] + T[(j+2)+33];
    d[1] = rotate(d[1], 23);
    d[1]+=d[2];
  }
  /*MD5 PASSO4 */
  for (j=0; j<4*4; j+=4) {
    d[0] = d[0]+ I(d[1], d[2], d[3])+ m[(7*j)&0x0f] + T[(j-1)+49];
    d[0] = rotate(d[0], 6);
    d[0]+=d[1];
    d[3] = d[3]+ I(d[0], d[1], d[2])+ m[(7*(j+1))&0x0f] + T[(j+0)+49];
    d[3] = rotate(d[3], 10);
    d[3]+=d[0];
    d[2] = d[2]+ I(d[3], d[0], d[1])+ m[(7*(j+2))&0x0f] + T[(j+1)+49];
    d[2] = rotate(d[2], 15);
    d[2]+=d[3];
    d[1] = d[1]+ I(d[2], d[3], d[0])+ m[(7*(j+3))&0x0f] + T[(j+2)+49];
    d[1] = rotate(d[1], 21);
    d[1]+=d[2];
  }
}


static void bytestoword32 (WORD32 *x, const char *pt) {
  int i;
  for (i=0; i<16; i++) {
    int j=i*4;
    x[i] = (((WORD32)(unsigned char)pt[j+3] << 8 |
           (WORD32)(unsigned char)pt[j+2]) << 8 |
           (WORD32)(unsigned char)pt[j+1]) << 8 |
           (WORD32)(unsigned char)pt[j];
  }

}


static void put_length(WORD32 *x, long len) {
  /* in bits! */
  x[14] = (WORD32)((len<<3) & MASK);
  x[15] = (WORD32)(len>>(32-3) & 0x7);
}


/*
** returned status:
*  0 - normal message (full 64 bytes)
*  1 - enough room for 0x80, but not for message length (two 4-byte words)
*  2 - enough room for 0x80 plus message length (at least 9 bytes free)
*/
static int converte (WORD32 *x, const char *pt, int num, int old_status) {
  int new_status = 0;
  char buff[64];
  if (num<64) {
    memcpy(buff, pt, num);  /* to avoid changing original string */
    memset(buff+num, 0, 64-num);
    if (old_status == 0)
      buff[num] = '\200';
    new_status = 1;
    pt = buff;
  }
  bytestoword32(x, pt);
  if (num <= (64 - 9))
    new_status = 2;
  return new_status;
}


void md5 (const char *message, long len, char *output) {
  WORD32 d[4];
  int status = 0;
  long i = 0;
  inic_digest(d);
  while (status != 2) {
    WORD32 d_old[4];
    WORD32 wbuff[16];
    int numbytes = (len-i >= 64) ? 64 : len-i;
    /*salva os valores do vetor digest*/
    d_old[0]=d[0]; d_old[1]=d[1]; d_old[2]=d[2]; d_old[3]=d[3];
    status = converte(wbuff, message+i, numbytes, status);
    if (status == 2) put_length(wbuff, len);
    digest(wbuff, d);
    d[0]+=d_old[0]; d[1]+=d_old[1]; d[2]+=d_old[2]; d[3]+=d_old[3];
    i += numbytes;
  }
  word32tobytes(d, output);
}


/**
*  Hash function. Returns a hash for a given string.
*  @param message: arbitrary binary string.
*  @return  A 128-bit hash string.
*/
static int lmd5 (lua_State *L) {
  WORD32 d[4];
  int status = 0;
  long i = 0;
  char buff[HASHSIZE + 16]; /* md5 len \0 */
  size_t len, sl = 0, addlen = 0;
  const char *smsg = NULL;
  const char *message = luaL_checklstring(L, 1, &len);
  if (lua_gettop(L) >= 2)
    smsg = luaL_checklstring(L, 2, &sl);
  if (!sl)
    inic_digest(d);
  else if (sscanf(smsg, "%08x%08x%08x%08x %d",
        &d[0], &d[1], &d[2], &d[3], &addlen) != 5)
    luaL_error(L, "bad md5 state");
  while (status != 2) {
    WORD32 d_old[4];
    WORD32 wbuff[16];
    int numbytes = (len-i >= 64) ? 64 : len-i;
    if (smsg && numbytes == 0 && len != 0)
      break;
    /*salva os valores do vetor digest*/
    d_old[0]=d[0]; d_old[1]=d[1]; d_old[2]=d[2]; d_old[3]=d[3];
    status = converte(wbuff, message+i, numbytes, status);
    if (status == 2) put_length(wbuff, addlen+len);
    digest(wbuff, d);
    d[0]+=d_old[0]; d[1]+=d_old[1]; d[2]+=d_old[2]; d[3]+=d_old[3];
    i += numbytes;
  }
  if (status == 2) {
    word32tobytes(d, buff);
    lua_pushlstring(L, buff, HASHSIZE);
  }
  else {
    sprintf(buff, "%08x%08x%08x%08x %d", d[0], d[1], d[2], d[3], addlen+len);
    lua_pushstring(L, buff);
  }
  return 1;
}


static int tohex(lua_State *L) {
  size_t i, l;
  const char *str = luaL_checklstring(L, 1, &l);
  luaL_Buffer b;
  char hex[3] = {};
  luaL_buffinit(L, &b);
  for (i = 0; i < l; ++i) {
    sprintf(hex, "%02x", str[i] & 0xff);
    luaL_addlstring(&b, hex, 2);
  }
  luaL_pushresult(&b);
  return 1;
}


static int lmd5hexa(lua_State *L) {
    size_t len;
    const char *ret = NULL;
    int multiret = lmd5(L);
    ret = luaL_checklstring(L, -1, &len);
    if (len != HASHSIZE)
        return multiret;
    lua_insert(L, 1);
    lua_settop(L, 1);
    return tohex(L);
}


/**
*  X-Or. Does a bit-a-bit exclusive-or of two strings.
*  @param s1: arbitrary binary string.
*  @param s2: arbitrary binary string with same length as s1.
*  @return  a binary string with same length as s1 and s2,
*   where each bit is the exclusive-or of the corresponding bits in s1-s2.
*/
static int ex_or (lua_State *L) {
  size_t l1, l2;
  const char *s1 = luaL_checklstring(L, 1, &l1);
  const char *s2 = luaL_checklstring(L, 2, &l2);
  luaL_Buffer b;
  luaL_argcheck( L, l1 == l2, 2, "lengths must be equal" );
  luaL_buffinit(L, &b);
  while (l1--) luaL_putchar(&b, (*s1++)^(*s2++));
  luaL_pushresult(&b);
  return 1;
}


static void checkseed (lua_State *L) {
  if (lua_isnone(L, 3)) {  /* no seed? */
    time_t tm = time(NULL);  /* for `random' seed */
    lua_pushlstring(L, (char *)&tm, sizeof(tm));
  }
}


#define MAXKEY	256
#define BLOCKSIZE	HASHSIZE


static int initblock (lua_State *L, const char *seed, int lseed, char *block) {
  size_t lkey;
  const char *key = luaL_checklstring(L, 2, &lkey);
  if (lkey > MAXKEY)
    luaL_error(L, "key too long (> %d)", MAXKEY);
  memset(block, 0, BLOCKSIZE);
  memcpy(block, seed, lseed);
  memcpy(block+BLOCKSIZE, key, lkey);
  return (int)lkey+BLOCKSIZE;
}


static void codestream (lua_State *L, const char *msg, size_t lmsg,
                                      char *block, int lblock) {
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while (lmsg > 0) {
    char code[BLOCKSIZE];
    int i;
    md5(block, lblock, code);
    for (i=0; i<BLOCKSIZE && lmsg > 0; i++, lmsg--)
      code[i] ^= *msg++;
    luaL_addlstring(&b, code, i); 
    memcpy(block, code, i); /* update seed */
  }
  luaL_pushresult(&b);
}


static void decodestream (lua_State *L, const char *cypher, size_t lcypher,
                          char *block, int lblock) {
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  while (lcypher > 0) {
    char code[BLOCKSIZE];
    int i;
    md5(block, lblock, code);  /* update seed */
    for (i=0; i<BLOCKSIZE && lcypher > 0; i++, lcypher--)
      code[i] ^= *cypher++;
    luaL_addlstring(&b, code, i); 
    memcpy(block, cypher-i, i);
  }
  luaL_pushresult(&b);
}


/**
*  Encrypts a string. Uses the hash function md5 in CFB (Cipher-feedback
*  mode).
*  @param message: arbitrary binary string to be encrypted.
*  @param key: arbitrary binary string to be used as a key.
*  @param [seed]: optional arbitrary binary string to be used as a seed.
*  if no seed is provided, the function uses the result of
*  <code>time()</code> as a seed.  
*  @return  The cyphertext (as a binary string).
*/
static int crypt (lua_State *L) {
  size_t lmsg;
  const char *msg = luaL_checklstring(L, 1, &lmsg);
  size_t lseed;
  const char *seed;
  int lblock;
  char block[BLOCKSIZE+MAXKEY];
  checkseed(L);
  seed = luaL_checklstring(L, 3, &lseed);
  if (lseed > BLOCKSIZE)
    luaL_error(L, "seed too long (> %d)", BLOCKSIZE);
  /* put seed and seed length at the beginning of result */
  block[0] = (char)lseed;
  memcpy(block+1, seed, lseed);
  lua_pushlstring(L, block, lseed+1);  /* to concat with result */
  lblock = initblock(L, seed, lseed, block);
  codestream(L, msg, lmsg, block, lblock);
  lua_concat(L, 2);
  return 1;
}


/**
*  Decrypts a string. For any message, key, and seed, we have that
*  <code>decrypt(crypt(msg, key, seed), key) == msg</code>.
*  @param cyphertext: message to be decrypted (this must be the result of
   a previous call to <code>crypt</code>.
*  @param key: arbitrary binary string to be used as a key.
*  @return  The plaintext.
*/
static int decrypt (lua_State *L) {
  size_t lcyphertext;
  const char *cyphertext = luaL_checklstring(L, 1, &lcyphertext);
  size_t lseed = cyphertext[0];
  const char *seed = cyphertext+1;
  int lblock;
  char block[BLOCKSIZE+MAXKEY];
  luaL_argcheck(L, lcyphertext >= lseed+1 && lseed <= BLOCKSIZE, 1,
                 "invalid cyphered string");
  cyphertext += lseed+1;
  lcyphertext -= lseed+1;
  lblock = initblock(L, seed, lseed, block);
  decodestream(L, cyphertext, lcyphertext, block, lblock);
  return 1;
}


/*
** Assumes the table is on top of the stack.
*/
static void set_info (lua_State *L) {
	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, "Copyright (C) 2003 PUC-Rio");
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, "Basic cryptographic facilities");
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, "MD5 1.1.2");
	lua_settable (L, -3);
	lua_pushliteral (L, "HASHSIZE");
	lua_pushinteger (L, HASHSIZE);
	lua_settable (L, -3);
}


static struct luaL_reg md5lib[] = {
  {"sum", lmd5},
  {"sumhexa", lmd5hexa},
  {"tohex", tohex},
  {"exor", ex_or},
  {"crypt", crypt},
  {"decrypt", decrypt},
  {NULL, NULL}
};


LUALIB_API int luaopen_md5 (lua_State *L) {
  luaL_openlib(L, "md5", md5lib, 0);
  set_info (L);
  return 1;
}

