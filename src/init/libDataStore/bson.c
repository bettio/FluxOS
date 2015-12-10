#include "bson.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

//HACK
int strnlen(const char *c, int n)
{
    return strlen(c);
}

inline uint32_t readUInt32(const void *u)
{
    const char *b = (const char *) u;
    return (b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24));
}

const void *bsonKeyLookup(const char *key, const void *document, uint8_t *type)
{
    const char *docBytes = (const char *) document;
    uint32_t docLen = readUInt32(document);

    //CHECK LEN

    unsigned int offset = 4;
    while (offset + 1 < docLen) {
       uint8_t elementType = (uint8_t) docBytes[offset];
       int keyLen = strnlen(docBytes + offset + 1, docLen - offset);

       if (!strncmp(key, docBytes + offset + 1, docLen - offset)) {
           if (type) {
               *type = elementType;
           }
           return (void *) (docBytes + offset + 1 + keyLen + 1);
       }

       //offset <- type (uint8_t) + key (const char *) + '\0' (char)
       offset += 1 + keyLen + 1;

       switch (elementType) {
           case TYPE_STRING: {
               uint32_t stringLen = readUInt32(docBytes + offset);
               offset += stringLen + 4;
           }
       }
    }

    return NULL;
}

const char *bsonValueToString(const void *valuePtr, uint8_t *len)
{
    const char *valueBytes = (const char *) valuePtr;
    uint32_t stringLen = readUInt32(valueBytes);

    if (len) {
        *len = stringLen;
    }

    return valueBytes + 4;
}

int32_t bsonValueToInt32(const void *valuePtr)
{
    return readUInt32(valuePtr);
}

void *mapFile(const char *name, int flags, int *fileFD, unsigned int *fileSize)
{
    int fd = open(name, flags);
    if (fileFD) {
        *fileFD = fd;
    }
    if (fd < 0) {
        return NULL;
    }

    struct stat fileStats;
    fstat(fd, &fileStats);

    if (fileSize) {
        *fileSize = fileStats.st_size;
    }

    return mmap(NULL, fileStats.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
}

int bsonCheckValidity(const void *document, unsigned int fileSize)
{
    const char *docBytes = (const char *) document;
    uint32_t docLen = readUInt32(document);
    int offset;

    if (fileSize < 4 + 1 + 2 + 1) {
        return 0;
    }

    if (docLen > fileSize) {
        return 0;
    }

    offset = 4;
    switch (docBytes[offset]) {
       case TYPE_STRING:
       break;

       default:
           return 0;
    }

    return 1;
}
