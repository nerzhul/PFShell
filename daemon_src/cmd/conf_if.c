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
#include "command.h"
#include "conf_if.h"
#include "../firewall.h"
#include "../configuration.h"
#include "../interface.h"
#include "../prompt/prompt_msg.h"
#include "../iputils.h"

cmdCallback cifCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
	{
		cb.promptMode = PROMPT_CONF;
		current_iface = "";
	}
	return cb;
}

cmdCallback cifCMD_ip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	char* iface[2];
	cutFirstWord(args,iface);
	if(strcmp(iface[0],"address") == 0)
		return cifCMD_ip_address(iface[1]);
	else if(strcmp(iface[0],"access-group") == 0)
		return cifCMD_access_list(iface[1]);
	else if(strcmp(iface[0],"ospf") == 0)
		return cifCMD_ip_ospf(iface[1]);
	else if(strcmp(iface[0],"rip") == 0)
		return cifCMD_ip_rip(iface[1]);
	else
		cb.message = CMDIF_IP_ERROR();
	return cb;
}

cmdCallback cifCMD_ip_rip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keyword[2];
	cutFirstWord(args,keyword);

	if(strcmp(keyword[0],"cost") == 0)
	{
		char* cost[2];
		cutFirstWord(keyword[1],cost);

		if(strlen(cost[1]) == 0)
		{
			int tmpcost = atoi(cost[0]);

			if(tmpcost > 0 && tmpcost < 17)
			{
				setInterfaceRIPCost(current_iface,tmpcost);
				WRITE_RUN();
				WRITE_RIPD();
			}
			else
				cb.message = CMDIF_IP_RIP_COST_ERROR();
		}
		else
			cb.message = CMDIF_IP_RIP_COST_ERROR();
	}
	else
		cb.message = CMDIF_IP_RIP_ERROR();
	return cb;
}

cmdCallback cifCMD_noip_rip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keyword[2];
	cutFirstWord(args,keyword);

	if(strcmp(keyword[0],"cost") == 0)
	{
		char* cost[2];
		cutFirstWord(keyword[1],cost);

		if(strlen(cost[1]) == 0)
		{
			int tmpcost = atoi(cost[0]);

			if(tmpcost == getInterfaceRIPCost(current_iface))
			{
				setInterfaceRIPCost(current_iface,RIP_DEFAULT_COST);
				WRITE_RUN();
				WRITE_RIPD();
			}
		}
		else
			cb.message = CMDIF_IP_RIP_COST_ERROR();
	}
	else
		cb.message = CMDIF_IP_RIP_ERROR();

	return cb;
}

cmdCallback cifCMD_ip_ospf(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keyword[2];
	cutFirstWord(args,keyword);

	if(strcmp(keyword[0],"cost") == 0)
	{
		char* cost[2];
		cutFirstWord(keyword[1],cost);

		if(strlen(cost[1]) == 0)
		{
			int tmpcost = atoi(cost[0]);

			if(tmpcost > 0 && tmpcost < 65536)
			{
				setInterfaceOSPFCost(current_iface,tmpcost);
				WRITE_RUN();
				WRITE_OSPFD();
			}
			else
				cb.message = CMDIF_IP_OSPF_COST_ERROR();
		}
		else
			cb.message = CMDIF_IP_OSPF_COST_ERROR();
	}
	else
		cb.message = CMDIF_IP_OSPF_ERROR();
	return cb;
}

cmdCallback cifCMD_noip_ospf(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keyword[2];
	cutFirstWord(args,keyword);

	if(strcmp(keyword[0],"cost") == 0)
	{
		char* cost[2];
		cutFirstWord(keyword[1],cost);

		if(strlen(cost[1]) == 0)
		{
			int tmpcost = atoi(cost[0]);

			if(tmpcost == getInterfaceOSPFCost(current_iface))
			{
				setInterfaceOSPFCost(current_iface,OSPF_DEFAULT_COST);
				WRITE_RUN();
				WRITE_OSPFD();
			}
		}
		else
			cb.message = CMDIF_IP_OSPF_COST_ERROR();
	}
	else
		cb.message = CMDIF_IP_OSPF_ERROR();

	return cb;
}

cmdCallback cifCMD_ip_address(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(args) < 4)
	{
		cb.message = CMDIF_IPADDR_ERROR();
		return cb;

	}

	if(strcmp(args,"DHCP") == 0)
	{
		if(getInterfaceState(current_iface) == 1)
		{
			char buffer[1024] = "";
			strcpy(buffer,"dhclient ");
			strcat(buffer,current_iface);
			hsystemcmd(buffer);
		}

		if(setInterfaceIP(current_iface,"DHCP") != 0)
		{
			cb.message = CMDIF_FATAL_ERROR();
			return cb;
		}
	}
	else
	{
		char* ipmask[2];
		cutFirstWord(args,ipmask);
		if(strlen(ipmask[1]) > 1)
		{
			if(regexp(ipmask[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
			{
				if(is_valid_mask(ipmask[1]) == 0)
				{
					char buffer[1024] = "";
					char ipbuffer[100] = "";

					strcpy(buffer,"ifconfig ");
					strcat(buffer,current_iface);
					strcat(buffer," ");

					strcpy(ipbuffer,ipmask[0]);
					strcat(ipbuffer," ");
					strcat(ipbuffer,ipmask[1]);
					strcat(buffer,ipbuffer);

					hsystemcmd(buffer);

					if(setInterfaceIP(current_iface,ipbuffer) != 0)
					{
						cb.message = CMDIF_FATAL_ERROR();
						return cb;
					}

				}
				else
				{
					cb.message = CMDIF_IPADDR_ERROR();
					return cb;
				}
			}
			else
			{
				cb.message = CMDIF_IPADDR_ERROR();
				return cb;
			}
		}
		else if(regexp(ipmask[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])((/([0-9]|[1-2][0-9]|3[0-2]))?)$") == 0)
		{
			char buffer[1024] = "";
			strcpy(buffer,"ifconfig ");
			strcat(buffer,current_iface);
			strcat(buffer," ");
			strcat(buffer,ipmask[0]);
			hsystemcmd(buffer);

			if(setInterfaceIP(current_iface,ipmask[0]) != 0)
			{
				cb.message = CMDIF_FATAL_ERROR();
				return cb;
			}
		}
		else
		{
			cb.message = CMDIF_IPADDR_ERROR();
			return cb;
		}
	}
	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_noip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	char* iface[2];
	cutFirstWord(args,iface);
	/*if(strcmp(iface[0],"address") == 0)
		return ;
	else */if(strcmp(iface[0],"access-group") == 0)
		return cifCMD_noaccess_list(iface[1]);
	else if(strcmp(iface[0],"ospf") == 0)
		return cifCMD_noip_ospf(iface[1]);
	else if(strcmp(iface[0],"rip") == 0)
		return cifCMD_noip_rip(iface[1]);
	return cb;

}

cmdCallback cifCMD_shutdown(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDIF_SHUTDOWN_ERROR();
		return cb;
	}

	char buffer[1024] = "";
	strcpy(buffer,"ifconfig ");
	strcat(buffer,current_iface);
	strcat(buffer," down");
	system(buffer);

	if(setInterfaceState(current_iface,0) != 0)
	{
		cb.message = CMDIF_FATAL_ERROR();
		return cb;
	}

	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_noshutdown(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDIF_NOSHUTDOWN_ERROR();
		return cb;
	}

	char buffer[1024] = "";
	strcpy(buffer,"ifconfig ");
	strcat(buffer,current_iface);
	strcat(buffer," up");
	system(buffer);

	char* ifconf = getInterfaceIP(current_iface);
	if(strcmp(ifconf,"DHCP") == 0)
	{
		char buffer[1024] = "";
		strcpy(buffer,"dhclient ");
		strcat(buffer,current_iface);
		hsystemcmd(buffer);
	}
	else if(strlen(ifconf) > 0)
	{
		strcpy(buffer,"ifconfig ");
		strcat(buffer,current_iface);
		strcat(buffer," ");
		strcat(buffer,ifconf);
		hsystemcmd(buffer);
	}

	if(setInterfaceState(current_iface,1) != 0)
	{
		cb.message = CMDIF_FATAL_ERROR();
		return cb;
	}

	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_access_list(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(args) < 3)
	{
		cb.message = CMDIF_ACCESS_LIST_ERROR();
		return cb;
	}

	char* aclname[2];
	cutFirstWord(args,aclname);

	char* inout[2];
	cutFirstWord(aclname[1],inout);
	if(strcmp(inout[0],"in") != 0 && strcmp(inout[0],"out") != 0)
	{
		cb.message = CMDIF_ACCESS_LIST_ERROR();
		return cb;
	}

	acl* cursor = access_lists;
	if(access_lists == NULL)
	{
		cb.message = CMDIF_ACCESS_LIST_UNK();
		return cb;
	}

	if(setInterfaceACL(current_iface,aclname[0],inout[0]) != 0)
		cb.message = CMDIF_ACCESS_LIST_UNK();

	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_noaccess_list(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(args) < 3)
	{
		cb.message = CMDIF_ACCESS_LIST_ERROR();
		return cb;
	}

	char* aclname[2];
	cutFirstWord(args,aclname);

	char* inout[2];
	cutFirstWord(aclname[1],inout);
	if(strcmp(inout[0],"in") != 0 && strcmp(inout[0],"out") != 0)
		return cb;

	acl* cursor = access_lists;
	if(access_lists == NULL)
		return cb;

	setInterfaceACL(current_iface,"",inout[0]);

	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_description(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(args) < 2)
	{
		cb.message = CMDIF_DESC_ERROR();
		return cb;
	}

	if(setInterfaceDesc(current_iface,args) != 0)
		cb.message = CMDIF_DESC_ERROR();

	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_nodescription(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	if(strlen(args) < 2)
	{
		cb.message = CMDIF_DESC_ERROR();
		return cb;
	}

	if(strcmp(getInterfaceDesc(current_iface),args) == 0)
	{
		if(setInterfaceDesc(current_iface,"") != 0)
			cb.message = CMDIF_DESC_ERROR();
	}

	WRITE_RUN();
	return cb;
}