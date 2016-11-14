/***************************************************************************
 *   Copyright 2005 by Davide Bettio <davide.bettio@kdemail.net>           *
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
 *   Name: vt.cpp                                                          *
 *   Date: 07/10/2007                                                      *
 ***************************************************************************/

#include <drivers/vt.h>

#include <errors.h>
#include <drivers/consoledevice.h>
#include <drivers/chardevice.h>
#include <drivers/termios.h>
#include <filesystem/ioctl.h>

#define MAX_PARAMS 32

enum state
{
	Normal,
	Escape,
	CSI	
} State = Normal;

unsigned int params[MAX_PARAMS];
int nParam;

unsigned int SavedX;
unsigned int SavedY;

#define DEFAULT_FORE_COLOR White
#define DEFAULT_BACK_COLOR Black

ConsoleDevice *Vt::console;

volatile char keysBuffer[256];
volatile int kbPos;
unsigned int readPos;
int inputAreaX; //HACK to avoid too much chars with backspace

CharDevice Vt::ttyDev =
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

void Vt::init(ConsoleDevice *dev)
{
    console = dev;
}

void Vt::ReInit()
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

CharDevice *Vt::Device()
{
    return &ttyDev;
}

int Vt::Write(CharDevice *cd, const char *buffer, int count)
{
    for (int i = 0; i < count; i++){
        Vt::putc(buffer[i]);
    }

    return count;
}

int Vt::Read(CharDevice *cd, char *buffer, int count)
{
    if (count == 0){
        return 0;
    }

    inputAreaX = console->x(); //HACK

    int buffCounter = 0;
    while (1){
        while (kbPos == readPos);
        while (readPos < kbPos){
            char tmp = keysBuffer[readPos & 0xFF];
            readPos++;

            if (tmp == '\b'){
                if (buffCounter > 0){
		    buffCounter--;
		}
		continue;
	    }

            if ((buffCounter == count)){
	        inputAreaX = -1; //HACK
                return buffCounter;
            }

            buffer[buffCounter] = tmp;
            buffCounter++;

	    if (tmp == '\n'){
	        inputAreaX = -1; //HACK
                return buffCounter;
	    }
        }
    }
}

int Vt::write(VNode *node, uint64_t pos, const char *buffer, unsigned int bufsize, WriteOpFlags flags)
{
    return Write((CharDevice *) 0, buffer, bufsize);
}

int Vt::read(VNode *node, uint64_t pos, char *buffer, unsigned int bufsize)
{
    return Read((CharDevice *) 0, buffer, bufsize);
}

int Vt::ioctl(VNode *node, int request, long arg)
{
    switch (request){
        case TCGETS: {
            termios *t = (termios *) arg;
            t->c_iflag = 0;
            t->c_oflag = OPOST;
            t->c_cflag = 0; 
            t->c_lflag = ECHO | ICANON | ISIG;
            t->c_line = 0;
            for (int i = 0; i < NCCS; i++) t->c_cc[i] = 0;
            t->c_ispeed = 0;
            t->c_ospeed = 0;
            return 0;	
        }
        default : {
            return -EIOCTLNOTSUPPORTED;
        }
     }
}

void *Vt::mmap(VNode *node, void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    return 0;
}

void Vt::notifyKeyPress(int code)
{
    keysBuffer[kbPos & 0xFF] = code;
    kbPos++;

    if ((code == '\b')){
        //HACK
	if (console->x() - 1 >= inputAreaX){
            console->setX(console->x() - 1);
            console->print(' ');
            console->setX(console->x() - 1);
	}
    }else{
        console->print(code);
    }

}

void Vt::putc(char c)
{
	unsigned int cY;
	unsigned int cX;
	int tmp;

	switch(State){
		case Normal:
			if (c == '\x1B'){
				State = Escape;
			}else{
				console->print(c);
			}

			break;

		case Escape:
			switch(c){
				case '[':
					State = CSI;

					//I efficiently clean parameters list
					for (int i = 0; i <= nParam; i++) params[i] = 0;

					//I reset the params counter
					nParam = 0;

					break;

				default:
					State = Normal;
			}

			break;

		case CSI:
			switch(c){
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					params[nParam] *= 10;
					params[nParam] += c - '0';

					break;

				case ';':
					if (nParam < MAX_PARAMS){
						nParam++;
						params[nParam] = 0;
					}else{
						State = Normal;
					}

					break;

				//Cursor Up
				case 'A':
					cY = console->y();

					if (params[0] == 0) params[0] = 1;

					if (params[0] < cY){
						console->setY(cY - params[0]);
					}

					State = Normal;

					break;

				//Cursor Down
				//TODO: Check compatibility
				case 'B':
					cY = console->y();

					if (params[0] == 0) params[0] = 1;

					console->setY(cY + params[0]);

					State = Normal;

					break;

				//Cursor Forward
				case 'C':
					cX = console->x();

					if (params[0] == 0) params[0] = 1;

					console->setX(cX + params[0]);

					State = Normal;

					break;

				//Cursor Backward
				case 'D':
					cX = console->x();

					if (params[0] == 0) params[0] = 1;

					if (params[0] < cX){
						console->setX(cX - params[0]);
					}

					State = Normal;

					break;

				//Cursor Next Line
				//TODO: Sembrano essere ignorati
				case 'E':
					if (params[0] == 0) params[0] = 1;

					///printf("Next Line %i\n", params[0]);

					break;

				//Cursor Preceding Line
				//TODO: Sembrano essere ignorati
				case 'F':
					if (params[0] == 0) params[0] = 1;

					///printf("Preceding Line %i\n", params[0]);

					break;

				//Unsave cursor
				case 'u':
					console->gotoXY(SavedX, SavedY);

					State = Normal;

					break;

				//Save cursor
				case 's':
					SavedX = console->x();
					SavedY = console->y();

					State = Normal;

					break;

				//Attributes
				case 'm':
					for (int i = 0; i <= nParam; i++){
						switch(params[i]){
							case 30:
								console->setTextColor(Black);

								break;
							case 31:
								console->setTextColor(Red);
								
								break;

							case 32:
								console->setTextColor(Green);
								
								break;

							//I don't know why but only xterm show 33 as yellow
							case 33:
								console->setTextColor(Brown);
								
								break;

							case 34:
								console->setTextColor(Blue);
								
								break;

							case 35:
								console->setTextColor(Magenta);
								
								break;

							case 36:
								console->setTextColor(Cyan);
								
								break;

							case 37:
								console->setTextColor(White);
								
								break;

							case 39:
								console->setTextColor(DEFAULT_FORE_COLOR);
								
								break;

							case 40:
								console->setBackColor(Black);

								break;
							case 41:
								console->setBackColor(Red);
								
								break;

							case 42:
								console->setBackColor(Green);
								
								break;

							//I don't know why but only xterm show 43 as yellow
							case 43:
								console->setBackColor(Brown);
								
								break;

							case 44:
								console->setBackColor(Blue);
								
								break;

							case 45:
								console->setBackColor(Magenta);
								
								break;

							case 46:
								console->setBackColor(Cyan);
								
								break;

							case 47:
								console->setBackColor(White);
								
								break;

							case 49:
								console->setBackColor(DEFAULT_BACK_COLOR);
								
								break;
						}
					}

					State = Normal;

					break;

				//Home
				case 'H':
					if (params[0] == 0) params[0] = 1;
					if (params[1] == 0) params[1] = 1;

					console->gotoXY(params[1], params[0]);

					State = Normal;

					break;

				//Erase display
				case 'J':
					switch(params[0]){
						//Below
						case 0:
							cX = console->x();
							cY = console->y();

							tmp = (console->height() - cY) * console->width() + console->width() - cX;

							for (int i = 0; i < tmp; i++) console->print(' ');

							console->gotoXY(cX, cY);

							break;

						//Above
						case 1:
							cX = console->x();
							cY = console->y();

							console->gotoXY(1, 1);

							tmp = (cY - 1) * console->width() + cX;

							for (int i = 0; i < tmp; i++) console->print(' ');

							console->gotoXY(cX, cY);

							break;

						//All
						case 2:
							console->clear();

							break;
					}

					State = Normal;

					break;

				//Erase line
				case 'K':
					switch(params[0]){
						//Right
						case 0:
							cX = console->x();

							tmp = console->width() - cX;
					
							for (int i = 0; i <= tmp; i++) console->print(' ');

							console->setX(cX);

							break;

						//Left
						case 1:
							cX = console->x();
					
							console->setX(1);
	
							for (unsigned int i = 0; i < cX; i++) console->print(' ');

							console->setX(cX);

							break;

						//All
						case 2:
							cX = console->x();
					
							console->setX(1);
	
							tmp = console->width();

							for (int i = 0; i < tmp; i++) console->print(' ');

							console->setX(cX);

						}
		
					State = Normal;

					break;

				//Erase chars
				case 'X':
					///printf("Erase chars: %i\n", params[0]);

					State = Normal;

					break;

				//TODO
				case '?':
					///****unsupported*****

					break;

				default:

					///****unmanaged*****

					State = Normal;
				}

			break;
	}
}
