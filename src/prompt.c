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

#ifdef WIN32
// Temp for tests on windows
#include <Windows.h>
#endif

unsigned int initPrompts()
{
	// Set default mode to usermode
	promptMode = PROMPT_USER;

	promptTable[0].action = &promptU;
	promptTable[1].action = &promptE;
	promptTable[2].action = &promptC;
	promptTable[3].action = &promptCIf;
	promptTable[4].action = &promptCFW;
	promptTable[5].action = &promptCRD;
	promptTable[6].action = &promptCACL;

	return 1;
}

void prompt()
{
	(*promptTable[promptMode].action)();
}

// User prompt
void promptU() { printf("%s> ",hostname); }
// Enable prompt
void promptE() { printf("%s# ",hostname); }
// Configure prompt
void promptC() { printf("%s(conf)# ",hostname);}
// Configure prompt for Network Interfaces
void promptCIf() { printf("%s(conf-iface)# ",hostname);}
// Configure prompt for Firewall
void promptCFW() { printf("%s(conf-fw)# ",hostname); }
// Configure prompt for CARP redundancy
void promptCRD() { printf("%s(conf-redundancy)# ",hostname);}
// Configure prompt for ACLs
void promptCACL() { printf("%s(conf-acl)# ",hostname);}

void printError(char* str, ...)
{
	va_list args;
    va_start(args, str);

    setPromptColor(0,RED);
    vprintf(str, args);
    resetPromptColor(0);

    va_end(args);
}

void printSuccess(char* str, ...)
{
	va_list args;
    va_start(args, str);

    setPromptColor(0,GREEN);
    vprintf(str, args);
    resetPromptColor(0);

    va_end(args);
}

void setPromptColor(short stdout_stream, short color)
{
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

    fprintf((stdout_stream? stdout : stderr), "\x1b[%d%sm",UnixColorFG[color],(color>=YELLOW&&color<MAX_COLORS ?";1":""));
}

void resetPromptColor(short stdout_stream)
{
    fprintf(( stdout_stream ? stdout : stderr ), "\x1b[0m");
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
