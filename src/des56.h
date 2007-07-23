#ifndef _FASTDESH_
#define _FASTDESH_

typedef long keysched[32];

typedef struct {
	unsigned char b[8];
} chunk;

#define DT_OK 0
#define DT_FILE_NOT_FOUND 1
#define DT_NOT_ENOUGH_MEMORY 2
#define DT_ERROR_RUNNIG_LUA_SCRIPT 3


//char* decypherText(char* fileName, int* returnCode, long* filesize, chunk c);
//int cypherText (char* inputFilename, char* outputFilename, chunk key);

void fsetkey(chunk *key, keysched ks);
void fencrypt(char *block, int decrypt, keysched ks);

#endif
