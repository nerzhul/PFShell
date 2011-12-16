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

void cfwCMD_disable(char* _none)
{
	if(strlen(_none) > 1) {
		CMDFW_DISABLE_ERROR();
		return;
	}

	if(askConfirm() == 0) {
		hsystemcmd("/sbin/pfctl -d");
		printSuccess("Firewall disabled !\n Don't forget to re-enable it faster !!\n");
	}
}

void cfwCMD_enable(char* _none)
{
	hsystemcmd("/sbin/pfctl -e");
	printSuccess("Firewall enabled.\n");
}

void cfwCMD_acl(char* args)
{
	char* nexttab[2];
	char _nextvar[1024];

	char* name;
	unsigned short _allow;
	unsigned short _direction;
	unsigned short _proto;

	char* _saddr;
	unsigned short _sport;

	char* _daddr;
	unsigned short _dport;


	strcpy(_nextvar,args);
	cutFirstWord(_nextvar,nexttab);
	if(strlen(nexttab[0]) < 2 || strlen(_nextvar) > 1023 || strcmp(nexttab[1],"") == 0)
	{
		CMDFW_ACL_ERROR();
		return;
	}

	name = (char*)malloc(strlen(nexttab[0])*sizeof(char));
	strcpy(name,nexttab[0]);
	strcpy(_nextvar,nexttab[1]);

	cutFirstWord(_nextvar,nexttab);
	if(strcmp(nexttab[0],"deny") != 0 && strcmp(nexttab[0],"allow") != 0 || strcmp(nexttab[1],"") == 0)
	{
		CMDFW_ACL_ERROR();
		return;
	}

	_allow = (strcmp(nexttab[0],"deny") == 0) ? 0 : 1;
	strcpy(_nextvar,nexttab[1]);

	cutFirstWord(_nextvar,nexttab);
	if(strcmp(nexttab[0],"in") != 0 && strcmp(nexttab[0],"out") != 0 || strcmp(nexttab[1],"") == 0)
	{
		CMDFW_ACL_ERROR();
		return;
	}

	_direction = (strcmp(nexttab[0],"in") == 0) ? 0 : 1;
	strcpy(_nextvar,nexttab[1]);

	cutFirstWord(_nextvar,nexttab);
	if(strcmp(nexttab[0],"tcp") != 0 && strcmp(nexttab[0],"udp") != 0 && strcmp(nexttab[0],"icmp") || strcmp(nexttab[1],"") == 0)
	{
		CMDFW_ACL_ERROR();
		return;
	}

	if(strcmp(nexttab[0],"tcp") == 0) _proto = 0;
	else if(strcmp(nexttab[0],"udp") == 0) _proto = 1;
	else _proto = 2;

	strcpy(_nextvar,nexttab[1]);

	cutFirstWord(_nextvar,nexttab);
	if(strcmp(nexttab[0],"any") == 0)
	{
		if(strcmp(nexttab[1],"") == 0)
		{
			CMDFW_ACL_ERROR();
			return;
		}

		_saddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
		strcpy(_saddr,nexttab[0]);
	}
	else
	{
		if(regexp(nexttab[0],"10.0.0.0/8") == 0)
		{
			_saddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
			strcpy(_saddr,nexttab[0]);
		}
		else
		{
			// its a port if we are there
			CMDFW_ACL_ERROR();
			return;
		}
	}

	strcpy(_nextvar,nexttab[1]);
	cutFirstWord(_nextvar,nexttab);

	int port = atoi(nexttab[0]);
	if(port <= 65535 && port > 0)
	{
		_sport = port;
		strcpy(_nextvar,nexttab[1]);
		cutFirstWord(_nextvar,nexttab);
	}
	else _sport = 0;

	if(strcmp(nexttab[0],"any") == 0)
	{
		_daddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
		strcpy(_daddr,nexttab[0]);
	}
	else
	{
		if(regexp(nexttab[0],"10.0.0.0/8") == 0)
		{
			_daddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
			strcpy(_daddr,nexttab[0]);
		}
		else
		{
			CMDFW_ACL_ERROR();
			return;
		}
	}

	if(strcmp(nexttab[1],"") == 0)
		_dport = 0;
	else
	{
		int port2 = atoi(nexttab[1]);
		if(port2 > 65535 && port2 <= 0)
		{
			CMDFW_ACL_ERROR();
			return;
		}
		else
			_dport = port2;
	}

	addACL(name,_direction,_proto,_sport,_dport,_saddr,_daddr,_allow);
	WRITE_RUN();
}

void cfwCMD_show(char* args)
{
	if(strlen(args) < 1)
	{
		CMDFW_SHOW_ERROR();
		return;
	}

	char* toshow[2];
	cutFirstWord(args,toshow);
	if(strcmp(toshow,"acls") == 0)
	{
		// @ TODO
	}
}

void cfwCMD_show_acl(char* args)
{
}

void cfwCMD_edit_packetfilter(char* _none)
{
	system("vi /etc/pf.conf");
	hsystemcmd("/sbin/pfctl -f /etc/pf.conf");
	printSuccess("Firewall reloaded.\n");
}

void cfwCMD_show_packetfilter(char* _none)
{
	system("/sbin/pfctl -s rule");
}
