/***************************************************************************
 *   Copyright 2010 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: gccattributes.h                                                 *
 *   Date: 08/2010                                                         *
 ***************************************************************************/

#ifndef _GCCATTRIBUTES_H
#define _GCCATTRIBUTES_H

#define ALLOC_FUNCTION \
          __attribute__((malloc))

#define ALLOC_SIZE(s0) \
          __attribute__((alloc_size(s0)))

#define ALLOC_SIZES(s0, s1) \
          __attribute__((alloc_size(s0,s1)))

#define ALWAYS_INLINE \
          __attribute__((always_inline)

#define COLD_SPOT \
          __attribute__ ((cold))

#define CONST_FUNCTION \
          __attribute__ ((__const__))

#define DEPRECATED \
          __attribute__ ((deprecated))

#define FORMAT_STRING(formatType, formatParam, params) \
          __attribute__ ((format (formatType, formatParam, params)))

#define HOT_SPOT \
          __attribute__ ((hot))

#define MUST_CHECK \
          __attribute__((warn_unused_result))

#define NON_NULL_ARGS \
          __attribute__((nonnull))

#define NON_NULL_ARG(a) \
          __attribute__((nonnull(a)))

#endif
