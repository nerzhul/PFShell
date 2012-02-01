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

#include "command_conf_fw.h"
#include "command_conf_acl.h"
#include "../firewall.h"
#include "../prompt/prompt_msg.h"
#include "../configuration.h"

cmdCallback cfwCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF;
	return cb;
}

cmdCallback cfwCMD_default(char* args)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	if(strlen(args) <= 11)
	{
		cb.message = CMDFW_DEFAULT_ERROR();
	}
	else
	{
		char* defaultcmd[2];
		cutFirstWord(args,defaultcmd);
		if(strcmp(defaultcmd[0],"input-policy") == 0)
			cfwCMD_default_input(defaultcmd[1]);
		else if(strcmp(defaultcmd[0],"output-policy") == 0)
			cfwCMD_default_output(defaultcmd[1]);
		else
		{
			cb.message = CMDFW_DEFAULT_ERROR();
		}
	}
	return cb;
}

cmdCallback cfwCMD_default_input(char* args)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	if(strlen(args) < 4)
	{
		cb.message = CMDFW_DEFAULT_ERROR();
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
			cb.message = CMDFW_DEFAULT_ERROR();
		}
	}
	return cb;
}

cmdCallback cfwCMD_default_output(char* args)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	if(strlen(args) < 4)
	{
		cb.message = CMDFW_DEFAULT_ERROR();
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
			cb.message = CMDFW_DEFAULT_ERROR();
		}
	}
	return cb;
}

cmdCallback cfwCMD_disable(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	if(strlen(_none) > 1) {
		cb.message = CMDFW_DISABLE_ERROR();
		return cb;
	}

	if(askConfirm() == 0) {
		hsystemcmd("/sbin/pfctl -d");
		firewallState = 0;
		cb.message = CMDFW_DISABLE_SUCCESS();
	}
	return cb;
}

cmdCallback cfwCMD_enable(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	if(strlen(_none) > 1) {
		cb.message = CMDFW_ENABLE_ERROR();
		return cb;
	}

	hsystemcmd("/sbin/pfctl -e");
	hsystemcmd("/sbin/pfctl -f /etc/pf.conf.run");
	firewallState = 1;
	CMDFW_ENABLE_SUCCESS();
	return cb;
}

cmdCallback cfwCMD_acl(char* args)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	char* aclname[2];
	cutFirstWord(args,aclname);

	if(strlen(aclname[1]) > 0 || strlen(aclname[0]) < 2)
	{
		cb.message = CMDFW_ACL_ERROR();
		return cb;
	}

	current_acl = aclname[0];
	cb.promptMode = PROMPT_CONF_ACL;
	return cb;
}

cmdCallback cfwCMD_noacl(char* args)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	char* aclname[2];
	cutFirstWord(args,aclname);

	if(strlen(aclname[1]) > 0 || strlen(aclname[0]) < 2)
		return cb;

	removeAccessList(aclname[0]);
	WRITE_RUN();
	return cb;
}

cmdCallback cfwCMD_show_packetfilter(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_FW,""};
	system("/sbin/pfctl -s rule");
	return cb;
}
