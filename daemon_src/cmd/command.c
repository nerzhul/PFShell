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
#include <string.h>
#include <regex.h>
#include "command.h"
#include "command_user.h"
#include "command_enable.h"
#include "command_conf.h"
#include "command_conf_acl.h"
#include "command_conf_if.h"
#include "command_conf_fw.h"
#include "command_conf_rd.h"
#include "command_conf_router.h"
#include "../prompt/prompt_msg.h"

unsigned short initCmds()
{
	// User Mode Commands
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
	confCmd[4].name = "ip";
	confCmd[4].handler = &cCMD_ip;
	confCmd[5].name = "router";
	confCmd[5].handler = &cCMD_router;

	// Enable - Configure Inverted Commands
	noconfCmd[0].name = "hostname";
	noconfCmd[0].handler = &cCMD_nohostname;
	noconfCmd[1].name = "ip";
	noconfCmd[1].handler = &cCMD_noip;

	// Enable - Configure - Interface Commands
	confIfCmd[0].name = "exit";
	confIfCmd[0].handler = &cifCMD_exit;
	confIfCmd[1].name = "shutdown";
	confIfCmd[1].handler = &cifCMD_shutdown;
	confIfCmd[2].name = "ip";
	confIfCmd[2].handler = &cifCMD_ip;
	confIfCmd[3].name = "description";
	confIfCmd[3].handler = &cifCMD_description;

	// Enable - Configure - Interface Inverted commands
	noconfIfCmd[0].name = "shutdown";
	noconfIfCmd[0].handler = &cifCMD_noshutdown;
	noconfIfCmd[1].name = "ip";
	noconfIfCmd[1].handler = &cifCMD_noip;
	noconfIfCmd[2].name = "description";
	noconfIfCmd[2].handler = &cifCMD_nodescription;

	// Enable - Configure - Firewall Commands
	confFWCmd[0].name = "exit";
	confFWCmd[0].handler = &cfwCMD_exit;
	confFWCmd[1].name = "default";
	confFWCmd[1].handler = &cfwCMD_default;
	confFWCmd[2].name = "enable";
	confFWCmd[2].handler = &cfwCMD_enable;
	confFWCmd[3].name = "disable";
	confFWCmd[3].handler = &cfwCMD_disable;
	confFWCmd[4].name = "access-list";
	confFWCmd[4].handler = &cfwCMD_acl;

	// Enable - Configure - Firewall Inverted Commands
	noconfFWCmd[0].name = "access-list";
	noconfFWCmd[0].handler = &cfwCMD_noacl;

	// Enable - Configure - Firewall - ACL Commands
	confACLCmd[0].name = "allow";
	confACLCmd[0].handler = &caclCMD_allow_acl;
	confACLCmd[1].name = "deny";
	confACLCmd[1].handler = &caclCMD_deny_acl;
	confACLCmd[2].name = "exit";
	confACLCmd[2].handler = &caclCMD_exit;

	// Enable - Configure - Firewall No ACL Commands
	noconfACLCmd[0].name = "allow";
	noconfACLCmd[0].handler = &caclCMD_noallow_acl;
	noconfACLCmd[1].name = "deny";
	noconfACLCmd[1].handler = &caclCMD_nodeny_acl;

	// Enable - Configure - Redundancy Commands
	confRDCmd[0].name = "exit";
	confRDCmd[0].handler = &crdCMD_exit;

	// Enable - Configure - Router Commands
	confRouterCmd[0].name = "exit";
	confRouterCmd[0].handler = &crouterCMD_exit;
	confRouterCmd[1].name = "redistribute";
	confRouterCmd[1].handler = crouterCMD_RIP_redistrib;
	confRouterCmd[2].name = "default-information";
	confRouterCmd[2].handler = crouterCMD_RIP_defaultinformation;
	confRouterCmd[3].name = "split-horizon";
	confRouterCmd[3].handler = crouterCMD_RIP_splithorizon;
	confRouterCmd[4].name = "passive-interface";
	confRouterCmd[4].handler = crouterCMD_RIP_passive;
	confRouterCmd[5].name = "cost";
	confRouterCmd[5].handler = crouterCMD_RIP_cost;

	// Enable - Configure - Router No Commands
	noconfRouterCmd[0].name = "redistribute";
	noconfRouterCmd[0].handler = crouterCMD_RIP_noredistrib;
	noconfRouterCmd[1].name = "default-information";
	noconfRouterCmd[1].handler = crouterCMD_RIP_nodefaultinformation;
	noconfRouterCmd[2].name = "split-horizon";
	noconfRouterCmd[2].handler = crouterCMD_RIP_nosplithorizon;
	noconfRouterCmd[3].name = "passive-interface";
	noconfRouterCmd[3].handler = crouterCMD_RIP_nopassive;
	noconfRouterCmd[4].name = "cost";
	noconfRouterCmd[4].handler = crouterCMD_RIP_cost; // Same as no cost

	masterCmd[0] = userCmd;
	masterCmd[1] = enableCmd;
	masterCmd[2] = confCmd;
	masterCmd[3] = confIfCmd;
	masterCmd[4] = confFWCmd;
	masterCmd[5] = confRDCmd;
	masterCmd[6] = confACLCmd;
	masterCmd[7] = confRouterCmd;

	masternoCmd[0] = nouserCmd;
	masternoCmd[1] = noenableCmd;
	masternoCmd[2] = noconfCmd;
	masternoCmd[3] = noconfIfCmd;
	masternoCmd[4] = noconfFWCmd;
	masternoCmd[5] = noconfRDCmd;
	masternoCmd[6] = noconfACLCmd;
	masternoCmd[7] = noconfRouterCmd;

	return 0;
}

cmdCallback handleCmd(char* _fullcmd, unsigned short promptMode)
{
	unsigned int i;
	i = 0;
	char* cmd[2];

	// For load configuration mode
	if(strcmp(_fullcmd,"!") == 0)
		_fullcmd = "exit";

	cutFirstWord(_fullcmd,cmd);

	if(strcmp(cmd[0],"no") == 0)
	{
		char* nocmd[2];
		cutFirstWord(cmd[1],nocmd);
		while(i < MAX_NO_CMDS[promptMode])
		{
			if(strcmp(nocmd[0],masternoCmd[promptMode][i].name) == 0)
			{
				return (*masternoCmd[promptMode][i].handler)(nocmd[1]);
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
				return (*masterCmd[promptMode][i].handler)(cmd[1]);
			}
			++i;
		}
	}

	cmdCallback cb = {promptMode,""};
	if(strlen(_fullcmd) > 0)
		cb.message = printError("Command Unknown !\n");

	return cb;
}

void cutFirstWord(char* string,char** result)
{
	char firstWord[1024] = "";
	char followWords[1024] = "";

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

void cutByChar(char* string,char** result,char cutter)
{
	char firstWord[1024] = "";
	char followWords[1024] = "";

	int offset = 0;
	int offset2 = 0;
	short first_written = 0;

	while(offset <= strlen(string))
	{
		if(!first_written)
		{
			if(string[offset] == cutter)
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


void hsystemcmd(char* cmd)
{
	char cmd2[1024] = "";
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
