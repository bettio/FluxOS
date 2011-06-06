/***************************************************************************
 *   Copyright 2006 by Davide Bettio <davide.bettio@kdemail.net>           *
 *   Copyright 2006 by Marco Pagliaricci                                   *
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
 *   Name: cstdlib.cpp                                                     *
 *   Date: 05/02/2006                                                      *
 ***************************************************************************/

#include <cstring.h>
#include <cstdlib.h>
#include <cstring.h>

#include <core/printk.h>
#include <cmath.h>

#include <arch.h>

	int abs(int num)
	{
		if (num < 0) return -num;

		return num;
	}

	long abs(long num)
	{
		if (num < 0) return -num;

		return num;
	}

extern "C"{
    /**
     * @author: Marco Pagliaricci
     */
	int atoi(const char *s)
	{
		int base = 10, f = 0, retn = 0, i = (strlen(s) - 1);

		if (*s == '-'){
			f = 1;
			++s;
		}

		if (*s == '0'){
			++s;
			--i;
			if (*s == 'x'){
				base = 16;
				--i;
				++s;
			}else if ((*s > '0') && (*s <= '9')){
				base = 8;
			}
		}

		while (*s){
			if ((*s >= '0') && (*s <= '9')){
				retn = retn + (uintpow(base,i) * ((*s) - '0'));

			}else if ((*s >= 'A') && (*s <= 'F')) {
				retn = retn + (uintpow(base,i) * ((*s) - ('A' - 10)));

			}else{
				retn /= base;
			}

			s++;
			i--;
		}

		if (f == 1) {
			return -retn;
		}else{
			return retn;
		}
	}

    size_t itoaz(long long n, char *s, unsigned int b)
    {
        size_t i = 0;
        long long num = (n > 0) ? n : -n;
        do{
            int digit = num % b;
            s[i] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            i++;
            num /= b;
        }while (num != 0);
        
        if (n < 0){
            s[i] = '-';
            i++;
        }
        
        for (int j = 0, k = i - 1; j < k; j++, k--){
            char tmp = s[k];
            s[k] = s[j];
            s[j] = tmp;
        }
        
        s[i] = '\0';
        
        return i;
    }

    size_t uitoaz(unsigned long long n, char *s, unsigned int b)
    {
        size_t i = 0;
        unsigned long long num = n;
        do{
            int digit = num % b;
            s[i] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            i++;
            num /= b;
        }while (num != 0);

        for (int j = 0, k = i - 1; j < k; j++, k--){
            char tmp = s[k];
            s[k] = s[j];
            s[j] = tmp;
        }
        
        s[i] = '\0';
        
        return i;
    }

	long atol(const char *str)
	{

		return 0;
	}

	long labs(long num)
	{
		if (num < 0) return -num;

		return num;
	}
	
	void abort(void)
	{
        printk("ABORT\n");
        while(1);
    }
}
