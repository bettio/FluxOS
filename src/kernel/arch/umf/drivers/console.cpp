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
 *   Name: console.cpp                                                     *
 *   Date: 28/12/2006                                                      *
 ***************************************************************************/

#include <drivers/chardevicemanager.h>
#include <arch/umf/drivers/console.h>
#include <arch/umf/core/hostsyscalls.h>
#include <cstdlib.h>
#include <cstring.h>

CharDevice Console::console_tty = 
{
	0,
	4,
	1,
	Read,
	Write,
	read,
	write,
	ioctl,
	mmap,
	"tty1",
};

void Console::Init()
{
	return;
}

void Console::ReInit()
{
	CharDevice *tty = new CharDevice;
	//TODO: Warning: unchecked malloc
	tty->int_cookie = 0;
	tty->Major = 4;
	tty->Minor = 1;
	tty->name = "tty1";
	tty->Read = Read;
	tty->Write = Write;
	tty->read = read;
	tty->write = write;
	tty->ioctl = ioctl;

	CharDeviceManager::Register(tty);
}

CharDevice *Console::Device()
{
	return &console_tty;
}

int Console::Write(CharDevice *cd, const char *buffer, int count)
{
	return HostSysCalls::write(1, buffer, count);
}

int Console::Read(CharDevice *cd, char *buffer, int count)
{
	return HostSysCalls::read(0, buffer, count);
}

int Console::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize)
{
	return HostSysCalls::write(1, buffer, bufsize);
}

int Console::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
	return HostSysCalls::read(0, buffer, bufsize);
}

int Console::ioctl(VNode *node, int request, long arg)
{
	return -EINVAL;
}

void *Console::mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
	return (void *) -ENODEV;
}
