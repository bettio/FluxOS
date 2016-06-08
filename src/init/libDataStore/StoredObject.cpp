#include "StoredObject.h"
#include "bson.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

class StoredObject::Private
{
    public:
        void *bsonObject;
        unsigned long bsonObjectSize;
        int bsonObjectFd;
};


StoredObject::~StoredObject()
{
    if (isValid()) {
       munmap(d->bsonObject, d->bsonObjectSize);
       close(d->bsonObjectFd);
    }
}

bool StoredObject::isValid() const
{
    return (d != NULL) && (d->bsonObject != NULL);
}

bool StoredObject::contains(const char *key) const
{
    return bsonKeyLookup(key, d->bsonObject, NULL) != NULL;
}

const char *StoredObject::value(const char *key, const char *defaultString) const
{
    uint8_t type;
    const void *valuePtr = bsonKeyLookup(key, d->bsonObject, &type);

    if ((valuePtr != NULL) && (type == TYPE_STRING)) {
        return bsonValueToString(valuePtr, NULL);
    } else {
        return defaultString;
    }
}

long int StoredObject::value(const char *key, long int defaultValue) const
{
    uint8_t type;
    const void *valuePtr = bsonKeyLookup(key, d->bsonObject, &type);

    if ((valuePtr != NULL) && (type == TYPE_INT32)) {
        return bsonValueToInt32(valuePtr);
    } else {
        return defaultValue;
    }
}

StoredObject StoredObject::loadObject(const char *fileName)
{
    StoredObject storedObj;

    unsigned int fileSize;
    int fd;
    char *bsonData = (char *) mapFile(fileName, O_RDONLY | O_CLOEXEC, &fd, &fileSize);
    if (!bsonData) {
        storedObj.d = NULL;
        return storedObj;
    }

    storedObj.d = new StoredObject::Private;
    if (storedObj.d) {
        if (bsonCheckValidity(bsonData, fileSize)) {
            storedObj.d->bsonObject = bsonData;
            storedObj.d->bsonObjectSize = fileSize;
            storedObj.d->bsonObjectFd = fd;
        } else {
            //delete storedObj.d;
            free(storedObj.d);
            storedObj.d = NULL;
            munmap((void *) bsonData, fileSize);
        }
    }

    return storedObj;
}
