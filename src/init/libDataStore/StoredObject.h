#ifndef _STOREDOBJECT_H_
#define _STOREDOBJECT_H_

class StoredObject
{
    public:
        bool isValid() const;

        bool contains(const char *key) const;

        const char *value(const char *key, const char *defaultString) const;
        long int value(const char *key, long int defaultValue) const;

        static StoredObject loadObject(const char *fileName);
        static StoredObject loadObject(int fd);

    private:
        class Private;
        Private *d;
};

#endif
