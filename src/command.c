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
#include <regex.h>
#include "command.h"
#include "command_user.h"
#include "command_enable.h"
#include "command_conf.h"
#include "command_conf_if.h"
#include "command_conf_fw.h"
#include "command_conf_rd.h"
#include "prompt.h"

unsigned short initCmds()
{
	//	User Mode Commands
	userCmd[0].name = "exit";
	userCmd[0].handler = &uCMD_exit;
	userCmd[1].name = "enable";
	userCmd[1].handler = &uCMD_enable;
	userCmd[2].name = "help";
	userCmd[2].handler = &uCMD_help;
	userCmd[3].name = "show";
	userCmd[3].handler = &uCMD_show;

	// Enable Mode Commands
	enableCmd[0].name = "exit";
	enableCmd[0].handler = &eCMD_exit;
	enableCmd[1].name = "configure";
	enableCmd[1].handler = &eCMD_configure;
	enableCmd[2].name = "save";
	enableCmd[2].handler = &eCMD_save;
	enableCmd[3].name = "show";
	enableCmd[3].handler = &eCMD_show;

	// Enable - Configure Commands
	confCmd[0].name = "exit";
	confCmd[0].handler = &cCMD_exit;
	confCmd[1].name = "firewall";
	confCmd[1].handler = &cCMD_firewall;
	confCmd[2].name = "hostname";
	confCmd[2].handler = &cCMD_hostname;
	confCmd[3].name = "interface";
	confCmd[3].handler = &cCMD_interface;

	// Enable - Configure - Interface Commands
	confIfCmd[0].name = "exit";
	confIfCmd[0].handler = &cifCMD_exit;
	confIfCmd[1].name = "shutdown";
	confIfCmd[1].handler = &cifCMD_shutdown;
	confIfCmd[2].name = "no shutdown";
	// @ TODO
	confIfCmd[3].name = "ip";
	confIfCmd[3].handler = &cifCMD_ip;

	// Enable - Configure - Interface Inverted commands
	noconfIfCmd[0].name = "shutdown";
	noconfIfCmd[0].handler = cifCMD_noshutdown;

	// Enable - Configure - Firewall Commands
	confFWCmd[0].name = "exit";
	confFWCmd[0].handler = &cfwCMD_exit;
	confFWCmd[1].name = "default";
	confFWCmd[1].handler = &cfwCMD_default;
	confFWCmd[2].name = "enable";
	confFWCmd[2].handler = &cfwCMD_enable;
	confFWCmd[3].name = "disable";
	confFWCmd[3].handler = &cfwCMD_disable;
	confFWCmd[4].name = "acl";
	confFWCmd[4].handler = &cfwCMD_acl;
	// TEMP Command
	confFWCmd[5].name = "edit";
	confFWCmd[5].handler = &cfwCMD_edit_packetfilter;

	// Enable - Configure - Redundancy Commands
	confRDCmd[0].name = "exit";
	confRDCmd[0].handler = &crdCMD_exit;

	masterCmd[0] = userCmd;
	masterCmd[1] = enableCmd;
	masterCmd[2] = confCmd;
	masterCmd[3] = confIfCmd;
	masterCmd[4] = confFWCmd;
	masterCmd[5] = confRDCmd;

	masternoCmd[0] = nouserCmd;
	masternoCmd[1] = noenableCmd;
	masternoCmd[2] = noconfCmd;
	masternoCmd[3] = noconfIfCmd;
	masternoCmd[4] = noconfFWCmd;
	masternoCmd[5] = noconfRDCmd;

	return 1;
}

// read only 1024 chars for this moment
// maybe we read less chars in the future
char* readCmd()
{
	char buffer[1024];
	char* cmd;
	char tmpchar;
	short offset = 0;
	do
	{
		tmpchar = getchar();
		buffer[offset] = tmpchar;
		++offset;
	// @ TODO: handle \t
	} while(offset < 1023 && tmpchar != '\n' && tmpchar != '\0');

	cmd = (char*) malloc((offset)*sizeof(char));

	--offset;
	cmd[offset] = '\0';
	--offset;

	while(offset >= 0)
	{
		cmd[offset] = buffer[offset];
		--offset;
	}
	return cmd;
}

void handleCmd(char* _fullcmd)
{
	unsigned int i;
	i = 0;
	char* cmd[2];
	cutFirstWord(_fullcmd,cmd);
	if(strcmp(cmd[0],"no") == 0)
	{
		char* nocmd[2];
		cutFirstWord(cmd[1],nocmd);
		while(i < MAX_NO_CMDS[promptMode])
		{
			if(strcmp(nocmd[0],masternoCmd[promptMode][i].name) == 0)
			{
				(*masternoCmd[promptMode][i].handler)(nocmd[1]);
				// Bad thing but improve performance code
				return;
			}
			++i;
		}
	}
	else
	{
		while(i < MAX_CMDS[promptMode])
		{
			if(strcmp(cmd[0],masterCmd[promptMode][i].name) == 0)
			{
				(*masterCmd[promptMode][i].handler)(cmd[1]);
				// Bad thing but improve performance code
				return;
			}
			++i;
		}
	}
	printError("Unknown command\n");
}

void cutFirstWord(char* string,char** result)
{
	char firstWord[1024];
	char followWords[1024];

	int offset = 0;
	int offset2 = 0;
	short first_written = 0;

	while(offset <= strlen(string))
	{
		if(!first_written)
		{
			if(string[offset] == ' ' || string[offset] == '\n')
			{
				first_written = 1;
				firstWord[offset] = '\0';
			}
			else
				firstWord[offset] = string[offset];
		}
		else
		{
			followWords[offset2] = string[offset];
			++offset2;
		}

		++offset;
	}

	if(first_written)
	{
		followWords[offset] = '\0';

		result[0] = (char*) malloc(offset*sizeof(char));
		result[1] = (char*) malloc(offset2*sizeof(char));

		offset = strlen(firstWord);
		while(offset >= 0)
		{
			result[0][offset] = firstWord[offset];
			--offset;
		}

		offset = strlen(followWords);
		while(offset >= 0)
		{
			result[1][offset] = followWords[offset];
			--offset;
		}
	}
	else
	{
		result[0] = string;
		result[1] = "";
	}
}

int execSystemCommand(char* cmd, char* output)
{
	FILE *fp;
	char path[1035];

	fp = popen(cmd, "r");
	if (fp == NULL) {
		printError("Failed to run command %s\n",cmd);
		return -1;
	}

	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		strcat(output,path);
	}

	pclose(fp);
	return 0;
}

void hsystemcmd(char* cmd)
{
	char cmd2[1024];
	strcpy(cmd2,cmd);
	strcat(cmd2," > /dev/null 2>&1");
	system(cmd2);
}

unsigned short regexp(char* str, char* pattern)
{
	int err,match;
	regex_t preg;
	err = regcomp(&preg, pattern, REG_EXTENDED);
	if(err == 0)
	{
		match = regexec(&preg, str, 0, NULL, 0);
		regfree (&preg);
		if (match == 0) return 0;
	}
	return 1;
}
