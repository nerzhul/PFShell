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

#include <stdlib.h>
#include <ncurses.h>
#include "command_user.h"
#include "../prompt/prompt_msg.h"

cmdCallback uCMD_enable(char* _none)
{
	cmdCallback cb = {PROMPT_USER,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDUSER_ENABLE_ERROR();
		return cb;
	}
	//  @ TODO
	printf("Password: \n");

	cb.promptMode = PROMPT_ENABLE;
	return cb;
}

cmdCallback uCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_USER,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
		return cb;
	}

	printf("Bye !\n");
	return cb;
}

cmdCallback uCMD_help(char* _none)
{
	cmdCallback cb = {PROMPT_USER,""};
	cb.message = "enable - grant you administrative privileges\nexit   - leave the current terminal\nhelp   - show this help\nshow   - show some informations\n";
	return cb;
}

cmdCallback uCMD_show(char* args)
{
	cmdCallback cb = {PROMPT_USER,""};
	if(strlen(args) <= 1)
	{
		cb.message = CMDUSER_SHOW_ERROR();
	}
	else
	{
		char* showcmd[2];
		cutFirstWord(args,showcmd);
		if(strcmp(showcmd[0],"version") == 0)
		{
				sprintf(cb.message,"PFShell version %s\n",VERSION);
		}
		else
		{
			cb.message = CMDUSER_SHOW_ERROR();
		}
	}
	return cb;
}
