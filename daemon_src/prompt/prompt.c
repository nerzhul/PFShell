/*
* Copyright (c) 2011-2012, Frost Sapphire Studios
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Frost Sapphire Studios nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "prompt.h"
#include "configuration.h"

char* printError(char* str, ...)
{
	char buffer[4094] = "";
	char buffer2[4094] = "";
	va_list args;
	va_start(args, str);

	strcpy(buffer,setPromptColor(0,RED));
	sprintf(buffer2,str, args);
	strcat(buffer,buffer2);
	strcat(buffer,resetPromptColor(0));

	va_end(args);
}

char* printSuccess(char* str, ...)
{
	char buffer[4094] = "";
	char buffer2[4094] = "";
	va_list args;
	va_start(args, str);

	strcpy(buffer,setPromptColor(0,GREEN));
	sprintf(buffer2,str, args);
	strcat(buffer,buffer2);
	strcat(buffer,resetPromptColor(0));

	va_end(args);
}

char* setPromptColor(short stdout_stream, short color)
{
	char buffer[100] = "";
	enum ANSITextAttr
	{
		TA_NORMAL=0,
		TA_BOLD=1,
		TA_BLINK=5,
		TA_REVERSE=7
		};

	enum ANSIFgTextAttr
	{
		FG_BLACK=30, FG_RED,  FG_GREEN, FG_BROWN, FG_BLUE,
		FG_MAGENTA,  FG_CYAN, FG_WHITE, FG_YELLOW
	};

	enum ANSIBgTextAttr
	{
		BG_BLACK=40, BG_RED,  BG_GREEN, BG_BROWN, BG_BLUE,
		BG_MAGENTA,  BG_CYAN, BG_WHITE
	};

	static unsigned int UnixColorFG[MAX_COLORS] =
	{
		FG_BLACK,                                           // BLACK
		FG_RED,                                             // RED
		FG_GREEN,                                           // GREEN
		FG_BROWN,                                           // BROWN
		FG_BLUE,                                            // BLUE
		FG_MAGENTA,                                         // MAGENTA
		FG_CYAN,                                            // CYAN
		FG_WHITE,                                           // WHITE
		FG_YELLOW,                                          // YELLOW
		FG_RED,                                             // LRED
		FG_GREEN,                                           // LGREEN
		FG_BLUE,                                            // LBLUE
		FG_MAGENTA,                                         // LMAGENTA
		FG_CYAN,                                            // LCYAN
		FG_WHITE                                            // LWHITE
	};

	sprintf(buffer,"\x1b[%dm",UnixColorFG[color]);
	return buffer;
}

char* resetPromptColor(short stdout_stream)
{
	return "\x1b[0m";
}

unsigned short askConfirm() {
	printf("Are you sure [y|n] ? ");
	char c = getchar();
	while(c != 'y' && c != 'n') {
		printError("Bad response, 'y' or 'n' expected !\n");
		c = getchar();
	}

	getchar();

	if(c == 'n') {
		printf("Aborted.\n");
		return 1;
	}

	return 0;
}
