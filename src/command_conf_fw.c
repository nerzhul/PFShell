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

#include "command_conf_fw.h"
#include "prompt.h"
#include "command.h"
#include "configuration.h"

void cfwCMD_exit(char* _none)
{
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   exit\n");
	else
		promptMode = PROMPT_CONF;
}

void cfwCMD_default(char* args)
{
	if(strlen(args) <= 11)
	{
		CMDFW_DEFAULT_ERROR();
	}
	else
	{
		char* defaultcmd[2];
		cutFirstWord(args,defaultcmd);
		// @TODO: create table with handlers
		if(strcmp(defaultcmd[0],"forward-policy") == 0)
			cfwCMD_default_forward(defaultcmd[1]);
		else if(strcmp(defaultcmd[0],"input-policy") == 0)
			cfwCMD_default_input(defaultcmd[1]);
		else if(strcmp(defaultcmd[0],"output-policy") == 0)
			cfwCMD_default_output(defaultcmd[1]);
		else
		{
			CMDFW_DEFAULT_ERROR();
		}
	}
}

void cfwCMD_default_forward(char* args)
{
	if(strlen(args) < 4)
	{
		CMDFW_DEFAULT_ERROR();
	}
	else
	{
		char* policy[2];
		cutFirstWord(args,policy);
		if(strcmp(policy[0],"deny") == 0)
		{
			pfpolicies[2] = 0;
			WRITE_RUN();
		}
		else if(strcmp(policy[0],"allow") == 0)
		{
			pfpolicies[2] = 1;
			WRITE_RUN();
		}
		else
		{
			CMDFW_DEFAULT_ERROR();
		}
	}
}

void cfwCMD_default_input(char* args)
{
	if(strlen(args) < 4)
	{
		CMDFW_DEFAULT_ERROR();
	}
	else
	{
		char* policy[2];
		cutFirstWord(args,policy);
		if(strcmp(policy[0],"deny") == 0)
		{
			pfpolicies[0] = 0;
			WRITE_RUN();
		}
		else if(strcmp(policy[0],"allow") == 0)
		{
			pfpolicies[0] = 1;
			WRITE_RUN();
		}
		else
		{
			CMDFW_DEFAULT_ERROR();
		}
	}
}

void cfwCMD_default_output(char* args)
{
	if(strlen(args) < 4)
	{
		CMDFW_DEFAULT_ERROR();
	}
	else
	{
		char* policy[2];
		cutFirstWord(args,policy);
		if(strcmp(policy[0],"deny") == 0)
		{
			pfpolicies[1] = 0;
			WRITE_RUN();
		}
		else if(strcmp(policy[0],"allow") == 0)
		{
			pfpolicies[1] = 1;
			WRITE_RUN();
		}
		else
		{
			CMDFW_DEFAULT_ERROR();
		}
	}
}

void cfwCMD_disable(char* _none) {
	if(strlen(_none) > 1) {
		CMDFW_DISABLE_ERROR();
		return;
	}
	// @ TODO: verify if PF is enabled
	if(askConfirm() == 0) {
		system("/sbin/pfctl -d > /dev/null 2>&1");
		printSuccess("Firewall disabled !\n Don't forget to re-enable it faster !!\n");
	}
}

void cfwCMD_enable(char* _none) {
	hsystemcmd("/sbin/pfctl -e > /dev/null 2>&1");
	printSuccess("Firewall enabled.\n");
}

void cfwCMD_edit_packetfilter(char* _none)
{
	system("vi /etc/pf.conf");
	system("/sbin/pfctl -d > /dev/null 2>&1");
	system("/sbin/pfctl -e > /dev/null 2>&1");
	printSuccess("Firewall reloaded.\n");
}

void cfwCMD_show_packetfilter(char* _none)
{
	system("/sbin/pfctl -s rule");
}
