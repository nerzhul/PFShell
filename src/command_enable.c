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

#include "configuration.h"
#include "command_enable.h"
#include "prompt.h"

void eCMD_configure(char* args) {
	if(strlen(args) > 0)
	{
		if(strcmp(args,"firewall") == 0)
		{
			promptMode = PROMPT_CONF_FW;
		}
		else
			CMDEN_CONFIGURE_ERROR();
	}
	else
		promptMode = PROMPT_CONF;
}

void eCMD_exit(char* _none) {
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   exit\n");
	else
		promptMode = PROMPT_USER;
}

void eCMD_show(char* args)
{
	if(strlen(args) <= 1)
	{
		CMDEN_SHOW_ERROR();
	}
	else
	{
		char* showcmd[2];
		cutFirstWord(args,showcmd);
		if(strcmp(showcmd[0],"version") == 0)
		{
				printf("PFShell version %s\n",VERSION);
		}
		else if(strcmp(showcmd[0],"interfaces") == 0)
		{
			char output[10240] = "";
#ifdef FREEBSD
			execSystemCommand("for IF in $(/sbin/ifconfig | grep HWaddr | awk '{print $1}'); do /sbin/ifconfig $IF; done;",output);
#else
			execSystemCommand("for IF in $(/sbin/ifconfig | grep BROADCAST | awk '{print $1}' | awk -F':' '{print $1}'); do /sbin/ifconfig $IF; done;",output);
#endif
			printf("Network Interfaces:\n%s",output);
		}
		else if(strcmp(showcmd[0],"acls") == 0)
		{
			printf("------------- Current ACLs -------------\n");
			unsigned short found = 0;
			acl* cursor = access_lists;
			while(cursor != NULL)
			{
				printf("Access-List %s\n",cursor->name);
				access_control* cursor2 = cursor->ac;
				while(cursor2 != NULL)
				{
					found = 1;
					putchar(' ');
					if(cursor2->_allow == 0)
						printf("deny ");
					else
						printf("allow ");

					if(cursor2->_direction == 0)
						printf("in ");
					else
						printf("out ");

					if(cursor2->_proto == 0)
						printf("tcp ");
					else if(cursor2->_proto == 1)
						printf("udp ");
					else
						printf("icmp ");

					printf("%s ",cursor2->_saddr);
					if(cursor2->_sport > 0)
						printf("%d ",cursor2->_sport);

					printf("%s ",cursor2->_daddr);
					if(cursor2->_dport > 0)
						printf("%d",cursor2->_dport);
					printf("\n");
					cursor2 = cursor2->next;
				}
				putchar('\n');
				cursor = cursor->next;
			}
			if(found == 0) printf("No Access-Lists found !\n");
		}
		else
		{
			CMDEN_SHOW_ERROR();
		}
	}
}

void eCMD_save(char* _none)
{
	system("cp /opt/PFShell/running-config /opt/PFShell/startup-config");
	printSuccess("Save done with success !\n");
}
