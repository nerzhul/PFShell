/* 
* Copyright (c) 2011, Frost Sapphire Studios
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
#include <string.h>
#include "command.h"
#include "prompt.h"

unsigned short initCmds()
{
	//	User Mode Commands
	userCmd[0].name = "exit";
	userCmd[1].handler = &uCMD_exit;
	userCmd[1].name = "enable";
	userCmd[1].handler = &uCMD_enable;

	// Enable Mode Commands
	enableCmd[0].name = "exit";
	//
	enableCmd[1].name = "configure";
	//
	enableCmd[2].name = "save";
	//

	// Enable - Configure Commands
	confCmd[0].name = "exit";

	// Enable - Configure - Interface Commands
	confIfCmd[0].name = "exit";
	//
	confIfCmd[1].name = "shutdown";
	//
	confIfCmd[2].name = "no shutdown";

	// Enable - Configure - Firewall Commands
	confFWCmd[0].name = "exit";

	// Enable - Configure - Redundancy Commands
	confRDCmd[0].name = "exit";

	return 1;
}

// read only 1024 chars for this moment
// maybe we read less chars in the future
char* readCmd()
{
	char cmd[1024];
	scanf("%s",cmd);
	return cmd;
}

void handleCmd(char* _cmd)
{
	// replace it by a table for the future
	switch(promptMode)
	{
		case PROMPT_USER:
		{
			unsigned int i;
			i = 0;

			while(i < MAX_USER_CMD)
			{
				if(strcmp(_cmd,userCmd[i].name) == 0)
				{
					// Todo: cut the string
					(*userCmd[i].handler)("");
					// Bad thing but improve performance code
					break;
				}
				++i;
			}
			break;
		}
		case PROMPT_ENABLE:
		{
			unsigned int i;
			i = 0;

			while(i < MAX_ENABLE_CMD)
			{
				if(strcmp(_cmd,enableCmd[i].name) == 0)
				{
					// Todo: cut the string
					(*enableCmd[i].handler)("");
					// Bad thing but improve performance code
					break;
				}
				++i;
			}
			break;
		}
		case PROMPT_CONF:
		{
			unsigned int i;
			i = 0;

			while(i < MAX_CONF_CMD)
			{
				if(strcmp(_cmd,confCmd[i].name) == 0)
				{
					// Todo: cut the string
					(*confCmd[i].handler)("");
					// Bad thing but improve performance code
					break;
				}
				++i;
			}
			break;
		}
		case PROMPT_CONF_IF:
		{
			unsigned int i;
			i = 0;

			while(i < MAX_CONF_IF_CMD)
			{
				if(strcmp(_cmd,confIfCmd[i].name) == 0)
				{
					// Todo: cut the string
					(*confIfCmd[i].handler)("");
					// Bad thing but improve performance code
					break;
				}
				++i;
			}
			break;
		}
		case PROMPT_CONF_FW:
		{
			unsigned int i;
			i = 0;

			while(i < MAX_CONF_FW_CMD)
			{
				if(strcmp(_cmd,confFWCmd[i].name) == 0)
				{
					// Todo: cut the string
					(*confFWCmd[i].handler)("");
					// Bad thing but improve performance code
					break;
				}
				++i;
			}
			break;
		}
		case PROMPT_CONF_RD:
		{
			unsigned int i;
			i = 0;

			while(i < MAX_CONF_RD_CMD)
			{
				if(strcmp(_cmd,confRDCmd[i].name) == 0)
				{
					// Todo: cut the string
					(*confRDCmd[i].handler)("");
					// Bad thing but improve performance code
					break;
				}
				++i;
			}
			break;
		}
		default:
			printf("[FATAL ERROR] Unhandled prompt appears %u\n",promptMode);
			exit(0);
			break;
	}
}