/***************************************************************************
 *   Copyright 2007 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: task.cpp                                                        *
 *   Date: 27/11/2007                                                      *
 ***************************************************************************/

#include <task/task.h>
#include <arch/umf/core/hostsyscalls.h>

#include <core/printk.h>
#include <cstdlib.h>
#include <core/system.h>

#include <lib/koof/intkeymap.h>

#include <arch/umf/linux-x86_64/syscall.h>

extern int cpid;
extern IntKeyMap<int> *Pids;
extern int currentTaskPid;

void SyscallsLoop(pid_t child, const char *name)
{
		long orig_eax;
		long params[5];
		int status;
		bool called = false;

		HostSysCalls::waitpid(-1, &status, 0);

		WIFEXITED(status){
			printk("Error: Process has exited too early (%s:%s)\n", __FILE__,  __FUNCTION__);

			return;
		}

		orig_eax = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * ORIG_EAX), NULL);

		HostSysCalls::ptrace(PTRACE_SYSCALL, child, NULL, NULL);

		if (orig_eax != 59){
			printk("Error: Unexpected first syscall: %i. (%s:%s)\n", orig_eax, __FILE__,  __FUNCTION__);

			return;
		}

        if (name) HostSysCalls::unlink(name);

		while(1){
			HostSysCalls::waitpid(-1, &status, 0);
			WIFEXITED(status) break;

			if (called == false){
				called = true;

				orig_eax = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * ORIG_EAX), NULL);
				params[0] = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * EDI), NULL);
				params[1] = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * ESI), NULL);
				params[2] = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * EDX), NULL);
				params[3] = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * R10), NULL);
				params[4] = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * R8), NULL);

				//printk("eax %i, ebx 0x%x, ecx 0x%x, edx 0x%x\n", orig_eax, params[0], params[1], params[2]);

				if ((orig_eax != 60) && (orig_eax != 12) && (orig_eax != 57) && (orig_eax != 7) && (orig_eax != 59) && (orig_eax != 243)
				     && (orig_eax != 192) && (orig_eax != 11)){
					HostSysCalls::ptrace(PTRACE_POKEUSER, child, (void *) (8 * ORIG_EAX), (void *) 39 /* getpid */);
				}
				
				if (orig_eax == 60){
                    Task::TaskDescriptorTable[(*Pids)[HostSysCalls::getpid()]].Status = TERMINATED;
                }
			}else{
				cpid = child;
				currentTaskPid = (*Pids)[HostSysCalls::getpid()];

				if ((orig_eax != 60) && (orig_eax != 12) && (orig_eax != 57) && (orig_eax != 7) && (orig_eax != 59) && (orig_eax != 243)&& (orig_eax != 192)&& (orig_eax != 11)){
					uint64_t tmpEax = HostSysCalls::ptrace(PTRACE_PEEKUSER, child, (void *) (8 * EAX), NULL);

					int syscRet = (((uint64_t) orig_eax) < 256) ? SysCall::SysCallTable[orig_eax](params[0], params[1], params[2], params[3], params[4]) : -1;

					HostSysCalls::ptrace(PTRACE_POKEUSER, child, (void *) (8 * EAX), (void *) syscRet);
				}

				called = false;
			}

			int ret = HostSysCalls::ptrace(PTRACE_SYSCALL, child, NULL, NULL);

			if (ret < 0) while(1); //FIXME
		}
}
