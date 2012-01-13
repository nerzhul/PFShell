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

#include <stdio.h>
#include <signal.h>
#include "unix_signal.h"
#include "prompt.h"

void sigint_handler(int param)
{
	if(promptMode > PROMPT_USER)
		promptMode = PROMPT_ENABLE;
	else
		promptMode = PROMPT_USER;
	putchar('\n');
	prompt();
	fflush(stdout);
}

void sigtstp_handler(int param)
{
	if(promptMode > PROMPT_USER)
		promptMode = PROMPT_ENABLE;
	else
		promptMode = PROMPT_USER;
	putchar('\n');
	prompt();
	fflush(stdout);
}

void sigterm_handler(int param)
{
	if(promptMode > PROMPT_USER)
		promptMode = PROMPT_ENABLE;
	else
		promptMode = PROMPT_USER;
	putchar('\n');
	prompt();
	fflush(stdout);
}

unsigned short initSignals()
{
	signal(SIGINT,sigint_handler);
	signal(SIGTERM,sigterm_handler);
	signal(SIGTSTP,sigtstp_handler);

	return 0;
}