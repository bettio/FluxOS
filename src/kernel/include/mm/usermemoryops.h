/***************************************************************************
 *   Copyright 2015 by Davide Bettio <davide.bettio@kdemail.net>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************
 *   Name: usermemoryops.h                                                 *
 *   Date: 28/12/2015                                                      *
 ***************************************************************************/

#ifndef USERMEMORYOPS_H_
#define USERMEMORYOPS_H_

#include <cstdlib.h>
#include <stdint.h>

#define userptr

extern "C"
{
    bool canWriteUserMemory(void *ptr, unsigned long size);
    bool canReadUserMemory(const void *ptr, unsigned long size);

    int strndupUser(const char *s, int maxsize, char **newStr);
    int strnlenUser(userptr const char *s, int maxsize);
    int strncpyFromUser(char *dest, userptr const char *src, int size);

    int memcpyToUser(userptr void *dest, const void *src, unsigned long size);
    int memcpyFromUser(void *dest, userptr const void *src, unsigned long size);

    int getFromUser8(uint8_t *value, uint8_t *ptr);
    int getFromUser16(uint16_t *value, userptr uint16_t *ptr);
    int getFromUser32(uint32_t *value, userptr uint32_t *ptr);
    int getFromUser64(uint64_t *value, userptr uint64_t *ptr);

    int putToUser8(uint8_t value, userptr uint8_t *ptr);
    int putToUser16(uint16_t value, userptr uint16_t *ptr);
    int putToUser32(uint32_t value, userptr uint32_t *ptr);
    int putToUser64(uint64_t value, userptr uint64_t *ptr);
}

template<typename T> inline int getFromUser(T *value, T *ptr)
{
    if (sizeof(T) == 1) {
        return getFromUser8((uint8_t *) value, (uint8_t *) ptr);
    } else if (sizeof(T) == 2) {
        return getFromUser16((uint16_t *) value, (uint16_t *) ptr);
    } else if (sizeof(T) == 4) {
        return getFromUser32((uint32_t *) value, (uint32_t *) ptr);
    } else if (sizeof(T) == 8) {
        return getFromUser64((uint64_t *) value, (uint64_t *) ptr);
    } else {
        return memcpyFromUser(value, ptr, sizeof(T));
    }
};

template<typename T> inline int putToUser(T value, T *ptr)
{
    if (sizeof(T) == 1) {
        return putToUser8((uint8_t) value, (uint8_t *) ptr);
    } else if (sizeof(T) == 2) {
        return putToUser16((uint16_t) value, (uint16_t *) ptr);
    } else if (sizeof(T) == 4) {
        return putToUser32((uint32_t) value, (uint32_t *) ptr);
    } else if (sizeof(T) == 8) {
        return putToUser64((uint64_t) value, (uint64_t *) ptr);
    } else {
        return memcpyToUser(ptr, &value, sizeof(T));
    }
};

class UserString
{
  public:
    inline UserString(userptr const char *user, int maxBufLen)
    {
        m_buf = NULL;
        m_len = strnlenUser(user, maxBufLen);
        int ret = 0;
        if (m_len < 0) {
            return;
        } else if (m_len < 64) {
            ret = strncpyFromUser(m_smallBuf, user, 64);
        } else {
            int dupSize = (m_len < maxBufLen) ? m_len : maxBufLen;
            ret = strndupUser(user, dupSize, &m_buf);
        }
        if (ret < 0) {
            m_len = ret;
        }
    }

    inline bool isValid()
    {
        return m_len >= 0;
    }

    inline int len()
    {
        return m_len;
    }

    inline int errorCode()
    {
        return m_len;
    }

    char *buf()
    {
        return m_buf ? m_buf : m_smallBuf;
    }

  private:
    char *m_buf;
    int m_len;
    char m_smallBuf[64];
};

#endif
