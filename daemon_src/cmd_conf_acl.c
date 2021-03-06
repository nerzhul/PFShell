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

#include "cmd_conf_acl.h"
#include "configuration.h"
#include "firewall.h"
#include "prompt_msg.h"

cmdCallback caclCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ACL,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF_FW;
	return cb;
}

cmdCallback caclCMD_allow_acl(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ACL,""};
	if(readACL(args,1,0) == 0)
	{
		WRITE_RUN();
	}
	else
		cb.message = CMDACL_ERROR();
	return cb;
}

cmdCallback caclCMD_deny_acl(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ACL,""};
	if(readACL(args,0,0) == 0)
	{
		WRITE_RUN();
	}
	else
		cb.message = CMDACL_ERROR();
	return cb;
}

cmdCallback caclCMD_noallow_acl(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ACL,""};
	if(readACL(args,1,1) == 0)
	{
		WRITE_RUN();
	}

	return cb;
}

cmdCallback caclCMD_nodeny_acl(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ACL,""};
	if(readACL(args,0,1) == 0)
	{
		WRITE_RUN();
	}

	return cb;
}

cmdCallback caclCMD_help(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ACL,""};

	if(strlen(_none) > 0)
		return cb;

	char buffer[3072];
	bzero(buffer,3072);
	uint8_t i;
	for(i=0;i<MAX_CONF_ACL_CMD;i++)
	{
		char buffer2[256];
		bzero(buffer2,256);
		sprintf(buffer2,"%s%s\n",confACLCmd[i].name,confACLCmd[i].help);
		strcat(buffer,buffer2);
	}

	cb.message = (char*)malloc((strlen(buffer)+1)*sizeof(char));
	strcpy(cb.message,buffer);
	return cb;
}
