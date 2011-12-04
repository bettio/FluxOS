/***************************************************************************
 *   Copyright 2011 by Davide Bettio <davide.bettio@kdemail.net>           *
 *   Copyright Silvio Abruzzo                                              *
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
 *   Name: keytraslation.cpp                                               *
 *   Date: 31/12/2004                                                      *
 ***************************************************************************
 *   I don't have have time to map each key, so I've took this file from   *
 *   Silvio's kernel. I don't really like this code but it saved me a lot  *
 *   of time. Anyway I will replace it shortly.                            *
 *   Sorry for the italian layout. Hope to fix it as soon as possible      *
 *   Anyway I think this source has been mostly generated with some tool   *
 ***************************************************************************/

#include <arch/ia32/io.h>

static uint8_t led_status;

static uint32_t keymap[104][11];
/*    1    ascii code*/
/*    2    shift*/
/*    3    ctrl*/
/*    4    alt*/
/*    5    shift+alt*/
/*    6    shift+ctrl*/
/*    7    ctrl+alt*/
/*    8    shift+ctrl+alt*/
/*    9    caps-lock*/
/*    10    shift+caps-lock*/

struct key_opt
{
    bool shift;
    bool ctrl;
    bool alt;
    bool caps_lock;
};

static key_opt my_opt;

void setleds(uint8_t led_status)
{
    while (inportb(0x64) & 2);
    outportb(0x60, 0xED);
    while (inportb(0x64) & 2);
    outportb(0x60, led_status);
}

void InitMap()
{
    my_opt.shift = false;
    my_opt.ctrl = false;
    my_opt.alt = false;
    my_opt.caps_lock = false;
    led_status = 0;
    setleds(0);

    keymap[1][1] = 0x1b;
    keymap[1][2] = 0x1b;
    keymap[1][3] = 0x1b;
    keymap[1][4] = 0x1b;
    keymap[1][5] = 0x1b;
    keymap[1][6] = 0x1b;
    keymap[1][7] = 0x1b;
    keymap[1][8] = 0x1b;
    keymap[1][9] = 0x1b;
    keymap[1][10] = 0x1b;

    keymap[2][1] = 49;
    keymap[2][2] = 33;
    keymap[2][3] = 49;
    keymap[2][4] = 49;
    keymap[2][5] = 33;
    keymap[2][6] = 33;
    keymap[2][7] = 49;
    keymap[2][8] = 33;
    keymap[2][9] = 49;
    keymap[2][10] = 33;

    keymap[3][1] = 50;
    keymap[3][2] = 34;
    keymap[3][3] = 50;
    keymap[3][4] = 50;
    keymap[3][5] = 34;
    keymap[3][6] = 34;
    keymap[3][7] = 50;
    keymap[3][8] = 34;
    keymap[3][9] = 50;
    keymap[3][10] = 34;

    keymap[4][1] = 51;
    keymap[4][2] = 156;
    keymap[4][3] = 51;
    keymap[4][4] = 51;
    keymap[4][5] = 156;
    keymap[4][6] = 156;
    keymap[4][7] = 51;
    keymap[4][8] = 156;
    keymap[4][9] = 51;
    keymap[4][10] = 156;

    keymap[5][1] = 52;
    keymap[5][2] = 36;
    keymap[5][3] = 52;
    keymap[5][4] = 52;
    keymap[5][5] = 36;
    keymap[5][6] = 36;
    keymap[5][7] = 52;
    keymap[5][8] = 36;
    keymap[5][9] = 52;
    keymap[5][10] = 36;

    keymap[6][1] = 53;
    keymap[6][2] = 37;
    keymap[6][3] = 53;
    keymap[6][4] = 53;
    keymap[6][5] = 37;
    keymap[6][6] = 37;
    keymap[6][7] = 53;
    keymap[6][8] = 37;
    keymap[6][9] = 53;
    keymap[6][10] = 37;

    keymap[7][1] = 54;
    keymap[7][2] = 38;
    keymap[7][3] = 54;
    keymap[7][4] = 54;
    keymap[7][5] = 38;
    keymap[7][6] = 38;
    keymap[7][7] = 54;
    keymap[7][8] = 38;
    keymap[7][9] = 54;
    keymap[7][10] = 38;

    keymap[8][1] = 55;
    keymap[8][2] = 47;
    keymap[8][3] = 55;
    keymap[8][4] = 55;
    keymap[8][5] = 47;
    keymap[8][6] = 47;
    keymap[8][7] = 55;
    keymap[8][8] = 47;
    keymap[8][9] = 55;
    keymap[8][10] = 47;

    keymap[9][1] = 56;
    keymap[9][2] = 40;
    keymap[9][3] = 56;
    keymap[9][4] = 56;
    keymap[9][5] = 40;
    keymap[9][6] = 40;
    keymap[9][7] = 56;
    keymap[9][8] = 40;
    keymap[9][9] = 56;
    keymap[9][10] = 40;

    keymap[10][1] = 57;
    keymap[10][2] = 41;
    keymap[10][3] = 57;
    keymap[10][4] = 57;
    keymap[10][5] = 41;
    keymap[10][6] = 41;
    keymap[10][7] = 57;
    keymap[10][8] = 41;
    keymap[10][9] = 57;
    keymap[10][10] = 41;

    keymap[11][1] = 48;
    keymap[11][2] = 61;
    keymap[11][3] = 48;
    keymap[11][4] = 48;
    keymap[11][5] = 61;
    keymap[11][6] = 61;
    keymap[11][7] = 48;
    keymap[11][8] = 61;
    keymap[11][9] = 48;
    keymap[11][10] = 61;

    keymap[12][1] = 39;
    keymap[12][2] = 63;
    keymap[12][3] = 0;
    keymap[12][4] = 39;
    keymap[12][5] = 0;
    keymap[12][6] = 0;
    keymap[12][7] = 0;
    keymap[12][8] = 0;
    keymap[12][9] = 39;
    keymap[12][10] = 63;

    keymap[13][1] = 141;
    keymap[13][2] = 94;
    keymap[13][3] = 0;
    keymap[13][4] = 0;
    keymap[13][5] = 0;
    keymap[13][6] = 0;
    keymap[13][7] = 0;
    keymap[13][8] = 0;
    keymap[13][9] = 141;
    keymap[13][10] = 94;

    keymap[14][1] = 8;
    keymap[14][2] = 8;
    keymap[14][3] = 8;
    keymap[14][4] = 8;
    keymap[14][5] = 8;
    keymap[14][6] = 8;
    keymap[14][7] = 8;
    keymap[14][8] = 8;
    keymap[14][9] = 8;
    keymap[14][10] = 8;

    keymap[15][1] = 9;
    keymap[15][2] = 9;
    keymap[15][3] = 9;
    keymap[15][4] = 9;
    keymap[15][5] = 9;
    keymap[15][6] = 9;
    keymap[15][7] = 9;
    keymap[15][8] = 9;
    keymap[15][9] = 9;
    keymap[15][10] = 9;

    keymap[16][1] = 113;
    keymap[16][2] = 81;
    keymap[16][3] = 113;
    keymap[16][4] = 113;
    keymap[16][5] = 81;
    keymap[16][6] = 81;
    keymap[16][7] = 113;
    keymap[16][8] = 81;
    keymap[16][9] = 81;
    keymap[16][10] = 113;

    keymap[17][1] = 119;
    keymap[17][2] = 87;
    keymap[17][3] = 119;
    keymap[17][4] = 119;
    keymap[17][5] = 87;
    keymap[17][6] = 87;
    keymap[17][7] = 119;
    keymap[17][8] = 87;
    keymap[17][9] = 87;
    keymap[17][10] = 119;

    keymap[18][1] = 101;
    keymap[18][2] = 69;
    keymap[18][3] = 101;
    keymap[18][4] = 101;
    keymap[18][5] = 69;
    keymap[18][6] = 69;
    keymap[18][7] = 101;
    keymap[18][8] = 69;
    keymap[18][9] = 69;
    keymap[18][10] = 101;

    keymap[19][1] = 114;
    keymap[19][2] = 82;
    keymap[19][3] = 114;
    keymap[19][4] = 114;
    keymap[19][5] = 82;
    keymap[19][6] = 82;
    keymap[19][7] = 114;
    keymap[19][8] = 82;
    keymap[19][9] = 82;
    keymap[19][10] = 114;

    keymap[20][1] = 116;
    keymap[20][2] = 84;
    keymap[20][3] = 116;
    keymap[20][4] = 116;
    keymap[20][5] = 84;
    keymap[20][6] = 84;
    keymap[20][7] = 116;
    keymap[20][8] = 84;
    keymap[20][9] = 84;
    keymap[20][10] = 116;

    keymap[21][1] = 121;
    keymap[21][2] = 89;
    keymap[21][3] = 121;
    keymap[21][4] = 121;
    keymap[21][5] = 89;
    keymap[21][6] = 89;
    keymap[21][7] = 121;
    keymap[21][8] = 89;
    keymap[21][9] = 89;
    keymap[21][10] = 121;

    keymap[22][1] = 117;
    keymap[22][2] = 85;
    keymap[22][3] = 117;
    keymap[22][4] = 117;
    keymap[22][5] = 85;
    keymap[22][6] = 85;
    keymap[22][7] = 117;
    keymap[22][8] = 85;
    keymap[22][9] = 85;
    keymap[22][10] = 117;

    keymap[23][1] = 105;
    keymap[23][2] = 73;
    keymap[23][3] = 105;
    keymap[23][4] = 105;
    keymap[23][5] = 73;
    keymap[23][6] = 73;
    keymap[23][7] = 105;
    keymap[23][8] = 73;
    keymap[23][9] = 73;
    keymap[23][10] = 105;

    keymap[24][1] = 111;
    keymap[24][2] = 79;
    keymap[24][3] = 111;
    keymap[24][4] = 111;
    keymap[24][5] = 79;
    keymap[24][6] = 79;
    keymap[24][7] = 111;
    keymap[24][8] = 79;
    keymap[24][9] = 79;
    keymap[24][10] = 111;

    keymap[25][1] = 112;
    keymap[25][2] = 80;
    keymap[25][3] = 112;
    keymap[25][4] = 112;
    keymap[25][5] = 80;
    keymap[25][6] = 80;
    keymap[25][7] = 112;
    keymap[25][8] = 80;
    keymap[25][9] = 80;
    keymap[25][10] = 112;

    keymap[26][1] = 138;
    keymap[26][2] = 130;
    keymap[26][3] = 0;
    keymap[26][4] = 91;
    keymap[26][5] = 0;
    keymap[26][6] = 0;
    keymap[26][7] = 0;
    keymap[26][8] = 0;
    keymap[26][9] = 138;
    keymap[26][10] = 130;

    keymap[27][1] = 43;
    keymap[27][2] = 42;
    keymap[27][3] = 0;
    keymap[27][4] = 93;
    keymap[27][5] = 0;
    keymap[27][6] = 0;
    keymap[27][7] = 0;
    keymap[27][8] = 0;
    keymap[27][9] = 43;
    keymap[27][10] = 42;

    keymap[28][1] = 10;
    keymap[28][2] = 10;
    keymap[28][3] = 10;
    keymap[28][4] = 10;
    keymap[28][5] = 10;
    keymap[28][6] = 10;
    keymap[28][7] = 10;
    keymap[28][8] = 10;
    keymap[28][9] = 10;
    keymap[28][10] = 10;

    keymap[29][1] = 0;
    keymap[29][2] = 0;
    keymap[29][3] = 0;
    keymap[29][4] = 0;
    keymap[29][5] = 0;
    keymap[29][6] = 0;
    keymap[29][7] = 0;
    keymap[29][8] = 0;
    keymap[29][9] = 0;
    keymap[29][10] = 0;

    keymap[30][1] = 97;
    keymap[30][2] = 65;
    keymap[30][3] = 97;
    keymap[30][4] = 97;
    keymap[30][5] = 65;
    keymap[30][6] = 65;
    keymap[30][7] = 97;
    keymap[30][8] = 65;
    keymap[30][9] = 65;
    keymap[30][10] = 97;

    keymap[31][1] = 115;
    keymap[31][2] = 83;
    keymap[31][3] = 115;
    keymap[31][4] = 115;
    keymap[31][5] = 83;
    keymap[31][6] = 83;
    keymap[31][7] = 115;
    keymap[31][8] = 83;
    keymap[31][9] = 83;
    keymap[31][10] = 115;

    keymap[32][1] = 100;
    keymap[32][2] = 68;
    keymap[32][3] = 100;
    keymap[32][4] = 100;
    keymap[32][5] = 68;
    keymap[32][6] = 68;
    keymap[32][7] = 100;
    keymap[32][8] = 68;
    keymap[32][9] = 68;
    keymap[32][10] = 100;

    keymap[33][1] = 102;
    keymap[33][2] = 70;
    keymap[33][3] = 102;
    keymap[33][4] = 102;
    keymap[33][5] = 70;
    keymap[33][6] = 70;
    keymap[33][7] = 102;
    keymap[33][8] = 70;
    keymap[33][9] = 70;
    keymap[33][10] = 102;

    keymap[34][1] = 103;
    keymap[34][2] = 71;
    keymap[34][3] = 103;
    keymap[34][4] = 103;
    keymap[34][5] = 71;
    keymap[34][6] = 71;
    keymap[34][7] = 103;
    keymap[34][8] = 71;
    keymap[34][9] = 71;
    keymap[34][10] = 103;

    keymap[35][1] = 104;
    keymap[35][2] = 72;
    keymap[35][3] = 104;
    keymap[35][4] = 104;
    keymap[35][5] = 72;
    keymap[35][6] = 72;
    keymap[35][7] = 104;
    keymap[35][8] = 72;
    keymap[35][9] = 72;
    keymap[35][10] = 104;

    keymap[36][1] = 106;
    keymap[36][2] = 74;
    keymap[36][3] = 106;
    keymap[36][4] = 106;
    keymap[36][5] = 74;
    keymap[36][6] = 74;
    keymap[36][7] = 106;
    keymap[36][8] = 74;
    keymap[36][9] = 74;
    keymap[36][10] = 106;

    keymap[37][1] = 107;
    keymap[37][2] = 75;
    keymap[37][3] = 107;
    keymap[37][4] = 107;
    keymap[37][5] = 75;
    keymap[37][6] = 75;
    keymap[37][7] = 107;
    keymap[37][8] = 75;
    keymap[37][9] = 75;
    keymap[37][10] = 107;

    keymap[38][1] = 108;
    keymap[38][2] = 76;
    keymap[38][3] = 108;
    keymap[38][4] = 108;
    keymap[38][5] = 76;
    keymap[38][6] = 76;
    keymap[38][7] = 108;
    keymap[38][8] = 76;
    keymap[38][9] = 76;
    keymap[38][10] = 108;

    keymap[39][1] = 149;
    keymap[39][2] = 135;
    keymap[39][3] = 0;
    keymap[39][4] = 64;
    keymap[39][5] = 0;
    keymap[39][6] = 0;
    keymap[39][7] = 0;
    keymap[39][8] = 0;
    keymap[39][9] = 149;
    keymap[39][10] = 135;

    keymap[40][1] = 133;
    keymap[40][2] = 167;
    keymap[40][3] = 0;
    keymap[40][4] = 35;
    keymap[40][5] = 0;
    keymap[40][6] = 0;
    keymap[40][7] = 0;
    keymap[40][8] = 0;
    keymap[40][9] = 133;
    keymap[40][10] = 167;

    keymap[41][1] = 92;
    keymap[41][2] = '|';
    keymap[41][3] = 0;
    keymap[41][4] = 0;
    keymap[41][5] = 0;
    keymap[41][6] = 0;
    keymap[41][7] = 0;
    keymap[41][8] = 0;
    keymap[41][9] = 92;
    keymap[41][10] = '|';

    keymap[42][1] = 0;
    keymap[42][2] = 0;
    keymap[42][3] = 0;
    keymap[42][4] = 0;
    keymap[42][5] = 0;
    keymap[42][6] = 0;
    keymap[42][7] = 0;
    keymap[42][8] = 0;
    keymap[42][9] = 0;
    keymap[42][10] = 0;

    keymap[43][1] = 151;
    keymap[43][2] = 15;
    keymap[43][3] = 0;
    keymap[43][4] = 0;
    keymap[43][5] = 0;
    keymap[43][6] = 0;
    keymap[43][7] = 0;
    keymap[43][8] = 0;
    keymap[43][9] = 151;
    keymap[43][10] = 15;

    keymap[44][1] = 122;
    keymap[44][2] = 90;
    keymap[44][3] = 122;
    keymap[44][4] = 122;
    keymap[44][5] = 90;
    keymap[44][6] = 90;
    keymap[44][7] = 122;
    keymap[44][8] = 90;
    keymap[44][9] = 90;
    keymap[44][10] = 122;

    keymap[45][1] = 120;
    keymap[45][2] = 88;
    keymap[45][3] = 120;
    keymap[45][4] = 120;
    keymap[45][5] = 88;
    keymap[45][6] = 88;
    keymap[45][7] = 120;
    keymap[45][8] = 88;
    keymap[45][9] = 88;
    keymap[45][10] = 120;

    keymap[46][1] = 99;
    keymap[46][2] = 67;
    keymap[46][3] = 99;
    keymap[46][4] = 99;
    keymap[46][5] = 67;
    keymap[46][6] = 67;
    keymap[46][7] = 99;
    keymap[46][8] = 67;
    keymap[46][9] = 67;
    keymap[46][10] = 99;

    keymap[47][1] = 118;
    keymap[47][2] = 86;
    keymap[47][3] = 118;
    keymap[47][4] = 118;
    keymap[47][5] = 86;
    keymap[47][6] = 86;
    keymap[47][7] = 118;
    keymap[47][8] = 86;
    keymap[47][9] = 86;
    keymap[47][10] = 118;

    keymap[48][1] = 98;
    keymap[48][2] = 66;
    keymap[48][3] = 98;
    keymap[48][4] = 98;
    keymap[48][5] = 66;
    keymap[48][6] = 66;
    keymap[48][7] = 98;
    keymap[48][8] = 66;
    keymap[48][9] = 66;
    keymap[48][10] = 98;

    keymap[49][1] = 110;
    keymap[49][2] = 78;
    keymap[49][3] = 110;
    keymap[49][4] = 110;
    keymap[49][5] = 78;
    keymap[49][6] = 78;
    keymap[49][7] = 110;
    keymap[49][8] = 78;
    keymap[49][9] = 78;
    keymap[49][10] = 110;

    keymap[50][1] = 109;
    keymap[50][2] = 77;
    keymap[50][3] = 109;
    keymap[50][4] = 109;
    keymap[50][5] = 77;
    keymap[50][6] = 77;
    keymap[50][7] = 109;
    keymap[50][8] = 77;
    keymap[50][9] = 77;
    keymap[50][10] = 109;

    keymap[51][1] = ',';
    keymap[51][2] = ';';
    keymap[51][3] = 0;
    keymap[51][4] = 0;
    keymap[51][5] = 0;
    keymap[51][6] = 0;
    keymap[51][7] = 0;
    keymap[51][8] = 0;
    keymap[51][9] = '.';
    keymap[51][10] = ';';

    keymap[52][1] = '.';
    keymap[52][2] = ':';
    keymap[52][3] = 0;
    keymap[52][4] = 0;
    keymap[52][5] = 0;
    keymap[52][6] = 0;
    keymap[52][7] = 0;
    keymap[52][8] = 0;
    keymap[52][9] = '.';
    keymap[52][10] = ':';

    keymap[53][1] = '-';
    keymap[53][2] = '_';
    keymap[53][3] = 0;
    keymap[53][4] = 0;
    keymap[53][5] = 0;
    keymap[53][6] = 0;
    keymap[53][7] = 0;
    keymap[53][8] = 0;
    keymap[53][9] = '-';
    keymap[53][10] = '_';

    keymap[54][1] = 0;
    keymap[54][2] = 0;
    keymap[54][3] = 0;
    keymap[54][4] = 0;
    keymap[54][5] = 0;
    keymap[54][6] = 0;
    keymap[54][7] = 0;
    keymap[54][8] = 0;
    keymap[54][9] = 0;
    keymap[54][10] = 0;

    keymap[55][1] = 42;
    keymap[55][2] = 42;
    keymap[55][3] = 0;
    keymap[55][4] = 42;
    keymap[55][5] = 0;
    keymap[55][6] = 0;
    keymap[55][7] = 0;
    keymap[55][8] = 0;
    keymap[55][9] = 42;
    keymap[55][10] = 42;

    keymap[56][1] = 0;
    keymap[56][2] = 0;
    keymap[56][3] = 0;
    keymap[56][4] = 0;
    keymap[56][5] = 0;
    keymap[56][6] = 0;
    keymap[56][7] = 0;
    keymap[56][8] = 0;
    keymap[56][9] = 0;
    keymap[56][10] = 0;

    keymap[57][1] = 32;
    keymap[57][2] = 32;
    keymap[57][3] = 32;
    keymap[57][4] = 32;
    keymap[57][5] = 32;
    keymap[57][6] = 32;
    keymap[57][7] = 32;
    keymap[57][8] = 32;
    keymap[57][9] = 32;
    keymap[57][10] = 32;

    keymap[58][1] = 0;
    keymap[58][2] = 0;
    keymap[58][3] = 0;
    keymap[58][4] = 0;
    keymap[58][5] = 0;
    keymap[58][6] = 0;
    keymap[58][7] = 0;
    keymap[58][8] = 0;
    keymap[58][9] = 0;
    keymap[58][10] = 0;

    keymap[59][1] = 0;
    keymap[59][2] = 0;
    keymap[59][3] = 0;
    keymap[59][4] = 0;
    keymap[59][5] = 0;
    keymap[59][6] = 0;
    keymap[59][7] = 0;
    keymap[59][8] = 0;
    keymap[59][9] = 0;
    keymap[59][10] = 0;

    keymap[60][1] = 0;
    keymap[60][2] = 0;
    keymap[60][3] = 0;
    keymap[60][4] = 0;
    keymap[60][5] = 0;
    keymap[60][6] = 0;
    keymap[60][7] = 0;
    keymap[60][8] = 0;
    keymap[60][9] = 0;
    keymap[60][10] = 0;

    keymap[61][1] = 0;
    keymap[61][2] = 0;
    keymap[61][3] = 0;
    keymap[61][4] = 0;
    keymap[61][5] = 0;
    keymap[61][6] = 0;
    keymap[61][7] = 0;
    keymap[61][8] = 0;
    keymap[61][9] = 0;
    keymap[61][10] = 0;

    keymap[62][1] = 0;
    keymap[62][2] = 0;
    keymap[62][3] = 0;
    keymap[62][4] = 0;
    keymap[62][5] = 0;
    keymap[62][6] = 0;
    keymap[62][7] = 0;
    keymap[62][8] = 0;
    keymap[62][9] = 0;
    keymap[62][10] = 0;

    keymap[63][1] = 0;
    keymap[63][2] = 0;
    keymap[63][3] = 0;
    keymap[63][4] = 0;
    keymap[63][5] = 0;
    keymap[63][6] = 0;
    keymap[63][7] = 0;
    keymap[63][8] = 0;
    keymap[63][9] = 0;
    keymap[63][10] = 0;

    keymap[64][1] = 0;
    keymap[64][2] = 0;
    keymap[64][3] = 0;
    keymap[64][4] = 0;
    keymap[64][5] = 0;
    keymap[64][6] = 0;
    keymap[64][7] = 0;
    keymap[64][8] = 0;
    keymap[64][9] = 0;
    keymap[64][10] = 0;

    keymap[65][1] = 0;
    keymap[65][2] = 0;
    keymap[65][3] = 0;
    keymap[65][4] = 0;
    keymap[65][5] = 0;
    keymap[65][6] = 0;
    keymap[65][7] = 0;
    keymap[65][8] = 0;
    keymap[65][9] = 0;
    keymap[65][10] = 0;

    keymap[66][1] = 0;
    keymap[66][2] = 0;
    keymap[66][3] = 0;
    keymap[66][4] = 0;
    keymap[66][5] = 0;
    keymap[66][6] = 0;
    keymap[66][7] = 0;
    keymap[66][8] = 0;
    keymap[66][9] = 0;
    keymap[66][10] = 0;

    keymap[67][1] = 0;
    keymap[67][2] = 0;
    keymap[67][3] = 0;
    keymap[67][4] = 0;
    keymap[67][5] = 0;
    keymap[67][6] = 0;
    keymap[67][7] = 0;
    keymap[67][8] = 0;
    keymap[67][9] = 0;
    keymap[67][10] = 0;

    keymap[68][1] = 0;
    keymap[68][2] = 0;
    keymap[68][3] = 0;
    keymap[68][4] = 0;
    keymap[68][5] = 0;
    keymap[68][6] = 0;
    keymap[68][7] = 0;
    keymap[68][8] = 0;
    keymap[68][9] = 0;
    keymap[68][10] = 0;

    keymap[69][1] = 0;
    keymap[69][2] = 0;
    keymap[69][3] = 0;
    keymap[69][4] = 0;
    keymap[69][5] = 0;
    keymap[69][6] = 0;
    keymap[69][7] = 0;
    keymap[69][8] = 0;
    keymap[69][9] = 0;
    keymap[69][10] = 0;

    keymap[70][1] = 0;
    keymap[70][2] = 0;
    keymap[70][3] = 0;
    keymap[70][4] = 0;
    keymap[70][5] = 0;
    keymap[70][6] = 0;
    keymap[70][7] = 0;
    keymap[70][8] = 0;
    keymap[70][9] = 0;
    keymap[70][10] = 0;

    keymap[71][1] = 55;
    keymap[71][2] = 55;
    keymap[71][3] = 55;
    keymap[71][4] = 55;
    keymap[71][5] = 55;
    keymap[71][6] = 55;
    keymap[71][7] = 55;
    keymap[71][8] = 55;
    keymap[71][9] = 55;
    keymap[71][10] = 55;

    keymap[72][1] = 56;
    keymap[72][2] = 56;
    keymap[72][3] = 56;
    keymap[72][4] = 56;
    keymap[72][5] = 56;
    keymap[72][6] = 56;
    keymap[72][7] = 56;
    keymap[72][8] = 56;
    keymap[72][9] = 56;
    keymap[72][10] = 56;

    keymap[73][1] = 57;
    keymap[73][2] = 57;
    keymap[73][3] = 57;
    keymap[73][4] = 57;
    keymap[73][5] = 57;
    keymap[73][6] = 57;
    keymap[73][7] = 57;
    keymap[73][8] = 57;
    keymap[73][9] = 57;
    keymap[73][10] = 57;

    keymap[74][1] = 45;
    keymap[74][2] = 45;
    keymap[74][3] = 45;
    keymap[74][4] = 45;
    keymap[74][5] = 45;
    keymap[74][6] = 45;
    keymap[74][7] = 45;
    keymap[74][8] = 45;
    keymap[74][9] = 45;
    keymap[74][10] = 45;

    keymap[75][1] = 52;
    keymap[75][2] = 52;
    keymap[75][3] = 52;
    keymap[75][4] = 52;
    keymap[75][5] = 52;
    keymap[75][6] = 52;
    keymap[75][7] = 52;
    keymap[75][8] = 52;
    keymap[75][9] = 52;
    keymap[75][10] = 52;

    keymap[76][1] = 53;
    keymap[76][2] = 53;
    keymap[76][3] = 53;
    keymap[76][4] = 53;
    keymap[76][5] = 53;
    keymap[76][6] = 53;
    keymap[76][7] = 53;
    keymap[76][8] = 53;
    keymap[76][9] = 53;
    keymap[76][10] = 53;

    keymap[77][1] = 54;
    keymap[77][2] = 54;
    keymap[77][3] = 54;
    keymap[77][4] = 54;
    keymap[77][5] = 54;
    keymap[77][6] = 54;
    keymap[77][7] = 54;
    keymap[77][8] = 54;
    keymap[77][9] = 54;
    keymap[77][10] = 54;

    keymap[78][1] = 43;
    keymap[78][2] = 43;
    keymap[78][3] = 43;
    keymap[78][4] = 43;
    keymap[78][5] = 43;
    keymap[78][6] = 43;
    keymap[78][7] = 43;
    keymap[78][8] = 43;
    keymap[78][9] = 43;
    keymap[78][10] = 43;

    keymap[79][1] = 49;
    keymap[79][2] = 49;
    keymap[79][3] = 49;
    keymap[79][4] = 49;
    keymap[79][5] = 49;
    keymap[79][6] = 49;
    keymap[79][7] = 49;
    keymap[79][8] = 49;
    keymap[79][9] = 49;
    keymap[79][10] = 49;

    keymap[80][1] = 50;
    keymap[80][2] = 50;
    keymap[80][3] = 50;
    keymap[80][4] = 50;
    keymap[80][5] = 50;
    keymap[80][6] = 50;
    keymap[80][7] = 50;
    keymap[80][8] = 50;
    keymap[80][9] = 50;
    keymap[80][10] = 50;

    keymap[81][1] = 51;
    keymap[81][2] = 51;
    keymap[81][3] = 51;
    keymap[81][4] = 51;
    keymap[81][5] = 51;
    keymap[81][6] = 51;
    keymap[81][7] = 51;
    keymap[81][8] = 51;
    keymap[81][9] = 51;
    keymap[81][10] = 51;

    keymap[82][1] = 48;
    keymap[82][2] = 48;
    keymap[82][3] = 48;
    keymap[82][4] = 48;
    keymap[82][5] = 48;
    keymap[82][6] = 48;
    keymap[82][7] = 48;
    keymap[82][8] = 48;
    keymap[82][9] = 48;
    keymap[82][10] = 48;

    keymap[83][1] = 46;
    keymap[83][2] = 46;
    keymap[83][3] = 46;
    keymap[83][4] = 46;
    keymap[83][5] = 46;
    keymap[83][6] = 46;
    keymap[83][7] = 46;
    keymap[83][8] = 46;
    keymap[83][9] = 46;
    keymap[83][10] = 46;

    keymap[84][1] = 0;
    keymap[84][2] = 0;
    keymap[84][3] = 0;
    keymap[84][4] = 0;
    keymap[84][5] = 0;
    keymap[84][6] = 0;
    keymap[84][7] = 0;
    keymap[84][8] = 0;
    keymap[84][9] = 0;
    keymap[84][10] = 0;

    keymap[85][1] = 0;
    keymap[85][2] = 0;
    keymap[85][3] = 0;
    keymap[85][4] = 0;
    keymap[85][5] = 0;
    keymap[85][6] = 0;
    keymap[85][7] = 0;
    keymap[85][8] = 0;
    keymap[85][9] = 0;
    keymap[85][10] = 0;

    keymap[86][1] = 60;
    keymap[86][2] = 62;
    keymap[86][3] = 0;
    keymap[86][4] = 60;
    keymap[86][5] = 0;
    keymap[86][6] = 0;
    keymap[86][7] = 0;
    keymap[86][8] = 0;
    keymap[86][9] = 60;
    keymap[86][10] = 62;

    keymap[87][1] = 0;
    keymap[87][2] = 0;
    keymap[87][3] = 0;
    keymap[87][4] = 0;
    keymap[87][5] = 0;
    keymap[87][6] = 0;
    keymap[87][7] = 0;
    keymap[87][8] = 0;
    keymap[87][9] = 0;
    keymap[87][10] = 0;

    keymap[88][1] = 0;
    keymap[88][2] = 0;
    keymap[88][3] = 0;
    keymap[88][4] = 0;
    keymap[88][5] = 0;
    keymap[88][6] = 0;
    keymap[88][7] = 0;
    keymap[88][8] = 0;
    keymap[88][9] = 0;
    keymap[88][10] = 0;

    keymap[89][1] = 0;
    keymap[89][2] = 0;
    keymap[89][3] = 0;
    keymap[89][4] = 0;
    keymap[89][5] = 0;
    keymap[89][6] = 0;
    keymap[89][7] = 0;
    keymap[89][8] = 0;
    keymap[89][9] = 0;
    keymap[89][10] = 0;

    keymap[90][1] = 0;
    keymap[90][2] = 0;
    keymap[90][3] = 0;
    keymap[90][4] = 0;
    keymap[90][5] = 0;
    keymap[90][6] = 0;
    keymap[90][7] = 0;
    keymap[90][8] = 0;
    keymap[90][9] = 0;
    keymap[90][10] = 0;

    keymap[91][1] = 0;
    keymap[91][2] = 0;
    keymap[91][3] = 0;
    keymap[91][4] = 0;
    keymap[91][5] = 0;
    keymap[91][6] = 0;
    keymap[91][7] = 0;
    keymap[91][8] = 0;
    keymap[91][9] = 0;
    keymap[91][10] = 0;

    keymap[92][1] = 0;
    keymap[92][2] = 0;
    keymap[92][3] = 0;
    keymap[92][4] = 0;
    keymap[92][5] = 0;
    keymap[92][6] = 0;
    keymap[92][7] = 0;
    keymap[92][8] = 0;
    keymap[92][9] = 0;
    keymap[92][10] = 0;

    keymap[93][1] = 0;
    keymap[93][2] = 0;
    keymap[93][3] = 0;
    keymap[93][4] = 0;
    keymap[93][5] = 0;
    keymap[93][6] = 0;
    keymap[93][7] = 0;
    keymap[93][8] = 0;
    keymap[93][9] = 0;
    keymap[93][10] = 0;

    keymap[94][1] = 0;
    keymap[94][2] = 0;
    keymap[94][3] = 0;
    keymap[94][4] = 0;
    keymap[94][5] = 0;
    keymap[94][6] = 0;
    keymap[94][7] = 0;
    keymap[94][8] = 0;
    keymap[94][9] = 0;
    keymap[94][10] = 0;

    keymap[95][1] = 0;
    keymap[95][2] = 0;
    keymap[95][3] = 0;
    keymap[95][4] = 0;
    keymap[95][5] = 0;
    keymap[95][6] = 0;
    keymap[95][7] = 0;
    keymap[95][8] = 0;
    keymap[95][9] = 0;
    keymap[95][10] = 0;

    keymap[96][1] = 0;
    keymap[96][2] = 0;
    keymap[96][3] = 0;
    keymap[96][4] = 0;
    keymap[96][5] = 0;
    keymap[96][6] = 0;
    keymap[96][7] = 0;
    keymap[96][8] = 0;
    keymap[96][9] = 0;
    keymap[96][10] = 0;

    keymap[97][1] = 0;
    keymap[97][2] = 0;
    keymap[97][3] = 0;
    keymap[97][4] = 0;
    keymap[97][5] = 0;
    keymap[97][6] = 0;
    keymap[97][7] = 0;
    keymap[97][8] = 0;
    keymap[97][9] = 0;
    keymap[97][10] = 0;

    keymap[98][1] = 0;
    keymap[98][2] = 0;
    keymap[98][3] = 0;
    keymap[98][4] = 0;
    keymap[98][5] = 0;
    keymap[98][6] = 0;
    keymap[98][7] = 0;
    keymap[98][8] = 0;
    keymap[98][9] = 0;
    keymap[98][10] = 0;

    keymap[99][1] = 0;
    keymap[99][2] = 0;
    keymap[99][3] = 0;
    keymap[99][4] = 0;
    keymap[99][5] = 0;
    keymap[99][6] = 0;
    keymap[99][7] = 0;
    keymap[99][8] = 0;
    keymap[99][9] = 0;
    keymap[99][10] = 0;

    keymap[100][1] = 0;
    keymap[100][2] = 0;
    keymap[100][3] = 0;
    keymap[100][4] = 0;
    keymap[100][5] = 0;
    keymap[100][6] = 0;
    keymap[100][7] = 0;
    keymap[100][8] = 0;
    keymap[100][9] = 0;
    keymap[100][10] = 0;

    keymap[101][1] = 0;
    keymap[101][2] = 0;
    keymap[101][3] = 0;
    keymap[101][4] = 0;
    keymap[101][5] = 0;
    keymap[101][6] = 0;
    keymap[101][7] = 0;
    keymap[101][8] = 0;
    keymap[101][9] = 0;
    keymap[101][10] = 0;

    keymap[102][1] = 0;
    keymap[102][2] = 0;
    keymap[102][3] = 0;
    keymap[102][4] = 0;
    keymap[102][5] = 0;
    keymap[102][6] = 0;
    keymap[102][7] = 0;
    keymap[102][8] = 0;
    keymap[102][9] = 0;
    keymap[102][10] = 0;

    keymap[103][1] = 0;
    keymap[103][2] = 0;
    keymap[103][3] = 0;
    keymap[103][4] = 0;
    keymap[103][5] = 0;
    keymap[103][6] = 0;
    keymap[103][7] = 0;
    keymap[103][8] = 0;
    keymap[103][9] = 0;
    keymap[103][10] = 0;
}

#define NO_VALID_SCANCODE 0
#define ARROW_UP 0
#define ARROW_DOWN 0
#define ARROW_LEFT 0
#define ARROW_RIGHT 0

int scan2Ascii(int car)
{
    switch (car) {
    case NO_VALID_SCANCODE:
        return NO_VALID_SCANCODE;
    case 42:        //shift press
    case 54:
        my_opt.shift = true;
        return NO_VALID_SCANCODE;
    case 170:        //shift up
    case 182:
        my_opt.shift = false;
        return NO_VALID_SCANCODE;
    case 29:        //ctrl press
    case 57373:
        my_opt.ctrl = true;
        return NO_VALID_SCANCODE;
    case 157:        //ctrl up
    case 57501:
        my_opt.ctrl = false;
        return NO_VALID_SCANCODE;
    case 56:        //alt press
    case 57400:
        my_opt.alt = true;
        return NO_VALID_SCANCODE;
    case 184:        //alt up
    case 57528:
        my_opt.alt = false;
        return NO_VALID_SCANCODE;
    case 58:        //caps press
        my_opt.caps_lock = !my_opt.caps_lock;
        led_status ^= 4;
        setleds(led_status);
        return NO_VALID_SCANCODE;
    case 69:        //block num
        led_status ^= 2;
        setleds(led_status);
        return NO_VALID_SCANCODE;
    case 70:        //block scrorr
        led_status ^= 1;
        setleds(led_status);
        return NO_VALID_SCANCODE;
    case 71:
    case 72:
    case 73:
    case 75:
    case 76:
    case 77:
    case 79:
    case 80:
    case 81:
    case 82:
        if (!(led_status & 2))
            return NO_VALID_SCANCODE;
    case 57416:
        return ARROW_UP;
    case 57421:
        return ARROW_RIGHT;
    case 57419:
        return ARROW_LEFT;
    case 57424:
        return ARROW_DOWN;
    }
    uint8_t type = 1;
    if (my_opt.shift){
        if (my_opt.ctrl){
            if (my_opt.alt){
                type = 8;
            }else{
                type = 6;
            }
        }else{
            if (my_opt.alt){
                type = 5;
            } else {
                if (my_opt.caps_lock){
                    type = 10;
                }else{
                    type = 2;
                }
            }
        }
    }else{
        if (my_opt.ctrl){
            if (my_opt.alt){
                type = 7;
            }else{
                type = 3;
            }
        } else {
            if (my_opt.alt){
                type = 4;
            }else{
                if (my_opt.caps_lock){
                    type = 9;
                }
            }
        }
    }
    if (car < 104) return keymap[car][type];

    return NO_VALID_SCANCODE;
}
