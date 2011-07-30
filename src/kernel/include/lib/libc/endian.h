
#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <arch.h>

#define BSWAP16(value) (((uint16_t) (value << 8)) | (((uint16_t) value) >> 8))

#ifdef LITTLE_ENDIAN
inline uint16_t BIG_TO_HOST_16(uint16_t value)
{
    return BSWAP16(value);
}

inline uint32_t BIG_TO_HOST_32(uint32_t value)
{
    return __builtin_bswap32(value);
}

inline uint64_t BIG_TO_HOST_64(uint64_t value)
{
    return __builtin_bswap64(value);
}
#else
inline uint16_t BIG_TO_HOST_16(uint16_t value)
{
    return value;
}

inline uint32_t BIG_TO_HOST_32(uint32_t value)
{
    return value;
}

inline uint64_t BIG_TO_HOST_64(uint64_t value)
{
    return value;
}
#endif

#ifdef LITTLE_ENDIAN
inline uint16_t LITTLE_TO_HOST_16(uint16_t value)
{
    return value;
}

inline uint32_t LITTLE_TO_HOST_32(uint32_t value)
{
    return value;
}

inline uint64_t LITTLE_TO_HOST_64(uint64_t value)
{
    return value;
}
#else
inline uint16_t LITTLE_TO_HOST_16(uint16_t value)
{
    return BSWAP16(value);
}

inline uint32_t LITTLE_TO_HOST_32(uint32_t value)
{
    return __builtin_bswap32(value);
}

inline uint64_t LITTLE_TO_HOST_64(uint64_t value)
{
    return __builtin_bswap64(value);
}
#endif

#ifdef LITTLE_ENDIAN
inline uint16_t HOST_TO_BIG_16(uint16_t value)
{
    return BSWAP16(value);
}

inline uint32_t HOST_TO_BIG_32(uint32_t value)
{
    return __builtin_bswap32(value);
}

inline uint64_t HOST_TO_BIG_64(uint64_t value)
{
    return __builtin_bswap64(value);
}
#else
inline uint16_t HOST_TO_BIG_16(uint16_t value)
{
    return value;
}

inline uint32_t HOST_TO_BIG_32(uint32_t value)
{
    return value;
}

inline uint64_t HOST_TO_BIG_64(uint64_t value)
{
    return value;
}
#endif

#ifdef LITTLE_ENDIAN
inline uint16_t HOST_TO_LITTLE_16(uint16_t value)
{
    return value;
}

inline uint32_t HOST_TO_LITTLE_32(uint32_t value)
{
    return value;
}

inline uint64_t HOST_TO_LITTLE_64(uint64_t value)
{
    return value;
}
#else
inline uint16_t HOST_TO_LITTLE_16(uint16_t value)
{
    return BSWAP16(value);
}

inline uint32_t HOST_TO_LITTLE_32(uint32_t value)
{
    return __builtin_bswap32(value);
}

inline uint64_t HOST_TO_LITTLE_64(uint64_t value)
{
    return __builtin_bswap64(value);
}
#endif

#endif
