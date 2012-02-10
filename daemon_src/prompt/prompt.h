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

#ifndef __PROMPT_H_
#define __PROMPT_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define PROMPT_USER		0
#define PROMPT_ENABLE		1
#define PROMPT_CONF		2
#define PROMPT_CONF_IF		3
#define PROMPT_CONF_FW		4
#define PROMPT_CONF_RD		5
#define PROMPT_CONF_ACL 	6
#define PROMPT_CONF_ROUTER 	7
#define MAX_PROMPTS		8


#define BLACK 0
#define RED 1
#define GREEN 2
#define BROWN 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define GREY 7
#define YELLOW 8
#define LRED 9
#define LGREEN 10
#define LBLUE 11
#define LMAGENTA 12
#define LCYAN 13
#define WHITE 14
#define MAX_COLORS 15

char* printError(char* str, ...);
char* printSuccess(char* str, ...);

char* setPromptColor(short stdout_stream, short color);
char* resetPromptColor(short stdout_stream);

unsigned short askConfirm();

#endif
