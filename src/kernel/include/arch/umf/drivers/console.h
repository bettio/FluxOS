/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: console.h                                                       *
 *   Date: 28/12/2006                                                      *
 ***************************************************************************/

#ifndef _ARCH_UMM_DRIVERS_CONSOLE_H_
#define _ARCH_UMM_DRIVERS_CONSOLE_H_

#include <kdef.h>
#include <filesystem/vfs.h>

struct CharDevice;
struct VNode;

class Console{
	public:
		static void Init();
		static void ReInit();
		static CharDevice *Device();
		static int Write(CharDevice *cd, const char *buffer, int count);
		static int Read(CharDevice *cd, char *buffer, int count);
		static int write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize);
		static int read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize);
		static int ioctl(VNode *node, int request, long arg);
		static void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset);

	private:
		static CharDevice console_tty;
};

#endif
