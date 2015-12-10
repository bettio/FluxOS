#ifndef _BSON_H_
#define _BSON_H_

#include <stdint.h>

#define TYPE_DOUBLE 0x01
#define TYPE_STRING 0x02
#define TYPE_INT32 0x10

#ifdef __cplusplus
extern "C" {
#endif

const void *bsonKeyLookup(const char *key, const void *document, uint8_t *type);
const char *bsonValueToString(const void *valuePtr, uint8_t *len);
int32_t bsonValueToInt32(const void *valuePtr);
void *mapFile(const char *name, int flags, int *fileFD, unsigned int *fileSize);
int bsonCheckValidity(const void *document, unsigned int fileSize);

#ifdef __cplusplus
}
#endif

#endif
