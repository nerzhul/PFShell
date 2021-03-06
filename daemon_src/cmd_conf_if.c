/*
* Copyright (c) 2011-2012, Loïc BLOT, CNRS
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the BSDRouterd nor the
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
#include "cmd_command.h"
#include "cmd_conf_if.h"
#include "firewall.h"
#include "configuration.h"
#include "interface.h"
#include "prompt_msg.h"
#include "iputils.h"
#include "string_mgmt.h"
#include "sysunix.h"

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
		current_iface_id = 0;
	}
	return cb;
}

cmdCallback cifCMD_ip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	char* subcmd[2];
	cutFirstWord(args,subcmd);
	if(strcmp(subcmd[0],"address") == 0)
		return cifCMD_ip_address(subcmd[1]);
	else if(strcmp(subcmd[0],"access-group") == 0)
		return cifCMD_access_list(subcmd[1]);
	else if(strcmp(subcmd[0],"helper-address") == 0)
		return cifCMD_ip_helper(subcmd[1]);
	else if(strcmp(subcmd[0],"ospf") == 0)
		return cifCMD_ip_ospf(subcmd[1]);
	else if(strcmp(subcmd[0],"rip") == 0)
		return cifCMD_ip_rip(subcmd[1]);
	else
		cb.message = CMDIF_IP_ERROR();
	return cb;
}

cmdCallback cifCMD_ip_rip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keywords[3];
	uint8_t nbargs = cutString(args,keywords,3);

	if(nbargs < 2 || nbargs == -1)
	{
		cb.message = CMDIF_IP_RIP_ERROR();
		freeCutString(keywords,nbargs);
		return cb;
	}

	if(strcmp(keywords[0],"authentication") == 0)
	{
		if(nbargs == 3)
		{
			if(strcmp(keywords[1],"key-string") == 0)
			{
				if(strlen(keywords[2]) < 17)
				{
					setInterfaceRIPAuthKey(current_iface,keywords[2]);
					WRITE_RUN();
					WRITE_RIPD();
				}
				else
					cb.message = CMDROUTER_RIP_AUTHENTICATION_ERROR();
			}
			else if(strcmp(keywords[1],"mode") == 0)
			{
					if(strcmp(keywords[2],"none") == 0)
					{
						setInterfaceRIPAuthType(current_iface,RIP_AUTH_NONE);
					}
					else if(strcmp(keywords[2],"text") == 0)
					{
						setInterfaceRIPAuthType(current_iface,RIP_AUTH_TEXT);
					}
					else if(strcmp(keywords[2],"md5") == 0)
					{
						setInterfaceRIPAuthType(current_iface,RIP_AUTH_MD5);
					}
					else
					{
						cb.message = CMDROUTER_RIP_AUTH_MODE_ERROR();
						freeCutString(keywords,nbargs);
						return cb;
					}
					WRITE_RUN();
					WRITE_RIPD();
			}
			else
				cb.message = CMDROUTER_RIP_AUTHENTICATION_ERROR();
		}
		else
			cb.message = CMDROUTER_RIP_AUTHENTICATION_ERROR();
	}
	else if(strcmp(keywords[0],"cost") == 0)
	{
		if(nbargs == 2)
		{
			int tmpcost = atoi(keywords[1]);

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

	freeCutString(keywords,nbargs);
	return cb;
}

cmdCallback cifCMD_noip_rip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keywords[3];
	uint8_t nbargs = cutString(args,keywords,3);

	if(nbargs < 2 || nbargs == -1)
	{
		cb.message = CMDIF_IP_RIP_ERROR();
		freeCutString(keywords,nbargs);
		return cb;
	}

	if(strcmp(keywords[0],"authentication") == 0)
	{
		if(nbargs == 3)
		{
			if(strcmp(keywords[1],"key-string") == 0)
			{
				if(strlen(keywords[2]) < 17)
				{
					if(strcmp(getInterfaceRIPAuthKey(current_iface),keywords[2]) == 0)
					{
						setInterfaceRIPAuthKey(current_iface,"");
						WRITE_RUN();
						WRITE_RIPD();
					}
				}
				else
					cb.message = CMDROUTER_RIP_AUTHENTICATION_ERROR();
			}
			else if(strcmp(keywords[1],"mode") == 0)
			{
				unsigned short auth_mode = getInterfaceRIPAuthType(current_iface);
				if(auth_mode == RIP_AUTH_TEXT && strcmp(keywords[2],"text") == 0)
				{
					setInterfaceRIPAuthType(current_iface,RIP_AUTH_NONE);
				}
				else if(auth_mode == RIP_AUTH_MD5 && strcmp(keywords[2],"md5") == 0)
				{
					setInterfaceRIPAuthType(current_iface,RIP_AUTH_NONE);
				}
				WRITE_RUN();
				WRITE_RIPD();
			}
			else
				cb.message = CMDROUTER_RIP_AUTHENTICATION_ERROR();
		}
		else
			cb.message = CMDROUTER_RIP_AUTHENTICATION_ERROR();
	}
	else if(strcmp(keywords[0],"cost") == 0)
	{
		if(nbargs == 2)
		{
			int tmpcost = atoi(keywords[1]);

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

	freeCutString(keywords,nbargs);
	return cb;
}

cmdCallback cifCMD_ip_ospf(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keywords[3];
	uint8_t nbargs = cutString(args,keywords,2);

	if(strlen(args) == 0 || nbargs == -1)
	{
		cb.message = CMDIF_IP_OSPF_ERROR();
		freeCutString(keywords,nbargs);
		return cb;
	}

	if(strcmp(keywords[0],"authentication") == 0)
	{
		if(nbargs == 3)
		{
			if(strcmp(keywords[1],"key-string") == 0)
			{
				if(strlen(keywords[2]) < 17)
				{
					setInterfaceOSPFAuthKey(current_iface,keywords[2]);
					WRITE_RUN();
					WRITE_RIPD();
				}
				else
					cb.message = CMDROUTER_OSPF_AUTHENTICATION_ERROR();
			}
			else if(strcmp(keywords[1],"mode") == 0)
			{
					if(strcmp(keywords[2],"none") == 0)
					{
						setInterfaceOSPFAuthType(current_iface,RIP_AUTH_NONE);
					}
					else if(strcmp(keywords[2],"text") == 0)
					{
						setInterfaceOSPFAuthType(current_iface,RIP_AUTH_TEXT);
					}
					else if(strcmp(keywords[2],"md5") == 0)
					{
						setInterfaceOSPFAuthType(current_iface,RIP_AUTH_MD5);
					}
					else
					{
						cb.message = CMDROUTER_OSPF_AUTH_MODE_ERROR();
						freeCutString(keywords,nbargs);
						return cb;
					}
					WRITE_RUN();
					WRITE_RIPD();
			}
			else
				cb.message = CMDROUTER_OSPF_AUTHENTICATION_ERROR();
		}
		else
			cb.message = CMDROUTER_OSPF_AUTHENTICATION_ERROR();
	}
	else if(strcmp(keywords[0],"cost") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDIF_IP_OSPF_ERROR();
			freeCutString(keywords,nbargs);
			return cb;
		}

		int tmpcost = atoi(keywords[1]);

		if(tmpcost > 0 && tmpcost < 65536)
		{
			setInterfaceOSPFCost(current_iface,tmpcost);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_COST_ERROR();
		}
	}
	else if(strcmp(keywords[0],"priority") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDIF_IP_OSPF_ERROR();
			freeCutString(keywords,nbargs);
			return cb;
		}

		int tmpvar = atoi(keywords[1]);

		if(tmpvar >= 0 && tmpvar <= 255)
		{
			setInterfaceOSPFPriority(current_iface,tmpvar);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_PRIORITY_ERROR();
		}
	}
	else if(strcmp(keywords[0],"hello-interval") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDIF_IP_OSPF_ERROR();
			freeCutString(keywords,nbargs);
			return cb;
		}

		int tmpvar = atoi(keywords[1]);

		if(tmpvar > 0 && tmpvar < 65536)
		{
			setInterfaceOSPFHello(current_iface,tmpvar);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_HELLO_ERROR();
		}
	}
	else if(strcmp(keywords[0],"dead-interval") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDIF_IP_OSPF_ERROR();
			freeCutString(keywords,nbargs);
			return cb;
		}

		int tmpvar = atoi(keywords[1]);

		if(tmpvar >= 2 && tmpvar < 2147483647)
		{
			setInterfaceOSPFDead(current_iface,tmpvar);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_DEAD_ERROR();
		}
	}
	else if(strcmp(keywords[0],"transmit-delay") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDIF_IP_OSPF_ERROR();
			freeCutString(keywords,nbargs);
			return cb;
		}

		int tmpvar = atoi(keywords[1]);

		if(tmpvar > 0 && tmpvar <= 3600)
		{
			setInterfaceOSPFTransmit(current_iface,tmpvar);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_TRANSMIT_ERROR();
		}
	}
	else if(strcmp(keywords[0],"retransmit-interval") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDIF_IP_OSPF_ERROR();
			freeCutString(keywords,nbargs);
			return cb;
		}

		int tmpvar = atoi(keywords[1]);

		if(tmpvar >= 5 && tmpvar <= 3600)
		{
			setInterfaceOSPFRetransmit(current_iface,tmpvar);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_RETRANSMIT_ERROR();
		}
	}
	else
		cb.message = CMDIF_IP_OSPF_ERROR();

	freeCutString(keywords,nbargs);
	return cb;
}

cmdCallback cifCMD_noip_ospf(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* keywords[3];
	uint8_t nbargs = cutString(args,keywords,3);

	if(strlen(args) == 0 || nbargs == -1)
	{
		cb.message = CMDIF_IP_OSPF_ERROR();
		freeCutString(keywords,nbargs);
		return cb;
	}

	if(strcmp(keywords[0],"authentication") == 0)
	{
		if(nbargs == 3)
		{
			if(strcmp(keywords[1],"key-string") == 0)
			{
				if(strlen(keywords[2]) < 17)
				{
					if(strcmp(getInterfaceOSPFAuthKey(current_iface),keywords[2]) == 0)
					{
						setInterfaceOSPFAuthKey(current_iface,"");
						WRITE_RUN();
						WRITE_RIPD();
					}
				}
				else
					cb.message = CMDROUTER_OSPF_AUTHENTICATION_ERROR();
			}
			else if(strcmp(keywords[1],"mode") == 0)
			{
				unsigned short auth_mode = getInterfaceOSPFAuthType(current_iface);
				if(auth_mode == RIP_AUTH_TEXT && strcmp(keywords[2],"text") == 0)
				{
					setInterfaceOSPFAuthType(current_iface,RIP_AUTH_NONE);
				}
				else if(auth_mode == RIP_AUTH_MD5 && strcmp(keywords[2],"md5") == 0)
				{
					setInterfaceOSPFAuthType(current_iface,RIP_AUTH_NONE);
				}
				WRITE_RUN();
				WRITE_RIPD();
			}
			else
				cb.message = CMDROUTER_OSPF_AUTHENTICATION_ERROR();
		}
		else
			cb.message = CMDROUTER_OSPF_AUTHENTICATION_ERROR();
	}
	else if(strcmp(keywords[0],"cost") == 0)
	{
		int tmpcost = atoi(keywords[1]);

		if(tmpcost == getInterfaceOSPFCost(current_iface))
		{
			setInterfaceOSPFCost(current_iface,OSPF_DEFAULT_COST);
			WRITE_RUN();
			WRITE_OSPFD();
		}
	}
	else if(strcmp(keywords[0],"priority") == 0)
	{
		int tmpvar = atoi(keywords[1]);

		if(tmpvar == getInterfaceOSPFPriority(current_iface))
		{
			setInterfaceOSPFPriority(current_iface,1);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_PRIORITY_ERROR();
		}
	}
	else if(strcmp(keywords[0],"hello-interval") == 0)
	{
		int tmpvar = atoi(keywords[1]);

		if(tmpvar == getInterfaceOSPFHello(current_iface))
		{
			setInterfaceOSPFHello(current_iface,10);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_HELLO_ERROR();
		}
	}
	else if(strcmp(keywords[0],"dead-interval") == 0)
	{
		int tmpvar = atoi(keywords[1]);

		if(tmpvar == (int)getInterfaceOSPFDead(current_iface))
		{
			setInterfaceOSPFDead(current_iface,40);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_DEAD_ERROR();
		}
	}
	else if(strcmp(keywords[0],"transmit-delay") == 0)
	{
		int tmpvar = atoi(keywords[1]);

		if(tmpvar == getInterfaceOSPFTransmit(current_iface))
		{
			setInterfaceOSPFTransmit(current_iface,1);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_TRANSMIT_ERROR();
		}
	}
	else if(strcmp(keywords[0],"retransmit-interval") == 0)
	{
		int tmpvar = atoi(keywords[1]);

		if(tmpvar == getInterfaceOSPFRetransmit(current_iface))
		{
			setInterfaceOSPFRetransmit(current_iface,5);
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
		{
			cb.message = CMDIF_IP_OSPF_RETRANSMIT_ERROR();
		}
	}
	else
		cb.message = CMDIF_IP_OSPF_ERROR();

	freeCutString(keywords,nbargs);
	return cb;
}

cmdCallback cifCMD_ip_address(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* _ip[2];
	uint8_t nbargs = cutString(args,_ip,2);

	if(strlen(args) == 0 || nbargs == -1)
	{
		cb.message = CMDIF_IPADDR_ERROR();
		freeCutString(_ip,nbargs);
		return cb;

	}

	if(nbargs == 1 && strcmp(_ip[0],"DHCP") == 0)
	{
		if(getInterfaceState(current_iface) == 1)
		{
			char buffer[1024] = "";
			strcpy(buffer,"dhclient ");
			strcat(buffer,current_iface);
			hsystemcmd(buffer);
			launchInterfaceIPHelpers(current_iface);
		}

		if(setInterfaceIP(current_iface,"DHCP") != 0)
		{
			cb.message = CMDIF_FATAL_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	else
	{

		if(nbargs == 2)
		{
			if(is_valid_ip(_ip[0]) == 0)
			{
				if(is_valid_mask(_ip[1]) == 0)
				{
					char buffer[512] = "";

					if(current_iface_id == 0)
						sprintf(buffer,"ifconfig %s %s netmask %s",current_iface,_ip[0],_ip[1]);
					else
						sprintf(buffer,"ifconfig vlan%d%d %s netmask %s",getInterfacePosition(current_iface),current_iface_id,_ip[0],_ip[1]);
					system(buffer);

					bzero(buffer,512);
					sprintf(buffer,"%s %s",_ip[0],_ip[1]);

					if(setInterfaceIP(current_iface,buffer) != 0)
					{
						cb.message = CMDIF_FATAL_ERROR();
						freeCutString(_ip,nbargs);
						return cb;
					}

					launchInterfaceIPHelpers(current_iface);

				}
				else
				{
					cb.message = CMDIF_IPADDR_ERROR();
					freeCutString(_ip,nbargs);
					return cb;
				}
			}
			else
			{
				cb.message = CMDIF_IPADDR_ERROR();
				freeCutString(_ip,nbargs);
				return cb;
			}
		}
		else if(is_valid_ip_and_cidr(_ip[0]) == 0)
		{
			char buffer[512] = "";
			if(current_iface_id == 0)
				sprintf(buffer,"ifconfig %s %s",current_iface,_ip[0]);
			else
				sprintf(buffer,"ifconfig vlan%d%d %s",getInterfacePosition(current_iface),current_iface_id,_ip[0]);
			system(buffer);

			if(setInterfaceIP(current_iface,_ip[0]) != 0)
			{
				cb.message = CMDIF_FATAL_ERROR();
				freeCutString(_ip,nbargs);
				return cb;
			}

			launchInterfaceIPHelpers(current_iface);
		}
		else
		{
			cb.message = CMDIF_IPADDR_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	WRITE_RUN();
	freeCutString(_ip,nbargs);
	return cb;
}

cmdCallback cifCMD_noip(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};
	char* subcmd[2];
	cutFirstWord(args,subcmd);
	/*if(strcmp(iface[0],"address") == 0)
		return ;
	else */if(strcmp(subcmd[0],"access-group") == 0)
		return cifCMD_noaccess_list(subcmd[1]);
	else if(strcmp(subcmd[0],"helper-address") == 0)
		return cifCMD_noip_helper(subcmd[1]);
	else if(strcmp(subcmd[0],"ospf") == 0)
		return cifCMD_noip_ospf(subcmd[1]);
	else if(strcmp(subcmd[0],"rip") == 0)
		return cifCMD_noip_rip(subcmd[1]);
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

	char buffer[64];
	bzero(buffer,64);
	if(current_iface_id == 0)
		sprintf(buffer,"ifconfig %s down",current_iface);
	else
		sprintf(buffer,"ifconfig vlan%d%d down",getInterfacePosition(current_iface),current_iface_id);
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

	char buffer[64];
	bzero(buffer,64);
	if(current_iface_id == 0)
		sprintf(buffer,"ifconfig %s up",current_iface);
	else
		sprintf(buffer,"ifconfig vlan%d%d up",getInterfacePosition(current_iface),current_iface_id);
	system(buffer);

	char* ifconf = getInterfaceIP(current_iface);
	if(strcmp(ifconf,"DHCP") == 0)
	{
		bzero(buffer,64);
		if(current_iface_id == 0)
			sprintf(buffer,"dhclient %s",current_iface);
		else
			sprintf(buffer,"dhclient vlan%d%d",getInterfacePosition(current_iface),current_iface_id);
		system(buffer);
	}
	else if(strlen(ifconf) > 0)
	{
		char* _ip[2];
		uint8_t nbargs = cutString(ifconf,_ip,2);
		if(nbargs == 1)
		{
			if(current_iface_id == 0)
				sprintf(buffer,"ifconfig %s %s",current_iface,_ip[0]);
			else
				sprintf(buffer,"ifconfig vlan%d%d %s",getInterfacePosition(current_iface),current_iface_id,_ip[0]);
			system(buffer);
		}
		else if(nbargs != -1)
		{
			if(current_iface_id == 0)
				sprintf(buffer,"ifconfig %s %s netmask %s",current_iface,_ip[0],_ip[1]);
			else
				sprintf(buffer,"ifconfig vlan%d%d %s netmask %s",getInterfacePosition(current_iface),current_iface_id,_ip[0],_ip[1]);
			system(buffer);
		}
	}

	if(setInterfaceState(current_iface,1) != 0)
	{
		cb.message = CMDIF_FATAL_ERROR();
		return cb;
	}

	launchInterfaceIPHelpers(current_iface);
	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_access_list(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* acls[2];
	uint8_t nbargs = cutString(args,acls,2);

	if(nbargs != 2 || nbargs == -1)
	{
		cb.message = CMDIF_ACCESS_LIST_ERROR();
		freeCutString(acls,nbargs);
		return cb;
	}

	if(strcmp(acls[1],"in") != 0 && strcmp(acls[1],"out") != 0)
	{
		cb.message = CMDIF_ACCESS_LIST_ERROR();
		freeCutString(acls,nbargs);
		return cb;
	}

	if(access_lists == NULL)
	{
		cb.message = CMDIF_ACCESS_LIST_UNK();
		freeCutString(acls,nbargs);
		return cb;
	}

	if(setInterfaceACL(current_iface,acls[0],acls[1]) != 0)
		cb.message = CMDIF_ACCESS_LIST_UNK();

	WRITE_RUN();
	freeCutString(acls,nbargs);
	return cb;
}

cmdCallback cifCMD_noaccess_list(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* acls[2];
	uint8_t nbargs = cutString(args,acls,2);

	if(nbargs != 2 || nbargs == -1)
	{
		cb.message = CMDIF_ACCESS_LIST_ERROR();
		freeCutString(acls,nbargs);
		return cb;
	}

	if(strcmp(acls[1],"in") != 0 && strcmp(acls[1],"out") != 0)
	{
		freeCutString(acls,nbargs);
		return cb;
	}

	if(access_lists == NULL)
	{
		freeCutString(acls,nbargs);
		return cb;
	}

	setInterfaceACL(current_iface,"",acls[1]);

	WRITE_RUN();
	freeCutString(acls,nbargs);
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
	{
		cb.message = CMDIF_DESC_ERROR();
	}
	else
	{
		char cmdbuffer[256];
		bzero(cmdbuffer,256);
		sprintf(cmdbuffer,"ifconfig %s description %s",current_iface,args);
		hsystemcmd(cmdbuffer);
	}

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
		{
			cb.message = CMDIF_DESC_ERROR();
		}
		else
		{
			char cmdbuffer[256];
			bzero(cmdbuffer,256);
			sprintf(cmdbuffer,"ifconfig %s -description",current_iface);
			hsystemcmd(cmdbuffer);
		}
	}

	WRITE_RUN();
	return cb;
}

cmdCallback cifCMD_encap(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(current_iface_id == 0)
	{
		cb.message = CMD_UNK();
		return cb;
	}

	char* enc[2];
	uint8_t nbargs = cutString(args,enc,2);

	if(nbargs != 2)
	{
		cb.message = CMDIF_ENCAPSULATION_ERROR();
		freeCutString(enc,nbargs);
		return cb;
	}

	if(strcmp(enc[0],"dot1q") == 0)
	{
		if(is_numeric(enc[1]) != 0)
		{
			cb.message = CMDIF_ENCAPSULATION_ERROR();
			freeCutString(enc,nbargs);
			return cb;
		}

		int vlanid = atoi(enc[1]);
		if(vlanid < 0 || vlanid > 1005)
		{
			cb.message = CMDIF_ENCAPSULATION_ERROR();
			freeCutString(enc,nbargs);
			return cb;
		}

		setInterfaceVLAN(current_iface,vlanid);
		char cmdbuffer[200];
		bzero(cmdbuffer,200);
		sprintf(cmdbuffer,"ifconfig vlan%d%d vlan %d",getInterfacePosition(current_iface),current_iface_id,vlanid);
		hsystemcmd(cmdbuffer);
		WRITE_RUN();
	}
	else
	{
		cb.message = CMDIF_ENCAPSULATION_ERROR();
		freeCutString(enc,nbargs);
		return cb;
	}

	freeCutString(enc,nbargs);
	return cb;
}

cmdCallback cifCMD_noencap(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(current_iface_id == 0)
	{
		cb.message = CMD_UNK();
		return cb;
	}

	char* enc[2];
	uint8_t nbargs = cutString(args,enc,2);

	if(nbargs != 2)
	{
		cb.message = CMDIF_ENCAPSULATION_ERROR();
		freeCutString(enc,nbargs);
		return cb;
	}

	if(strcmp(enc[0],"dot1q") == 0)
	{
		if(is_numeric(enc[1]) != 0)
		{
			cb.message = CMDIF_ENCAPSULATION_ERROR();
			freeCutString(enc,nbargs);
			return cb;
		}

		int vlanid = atoi(enc[1]);
		if(vlanid != getInterfaceVLAN(current_iface))
		{
			freeCutString(enc,nbargs);
			return cb;
		}

		setInterfaceVLAN(current_iface,0);
		char cmdbuffer[200];
		bzero(cmdbuffer,200);
		sprintf(cmdbuffer,"ifconfig vlan%d%d vlan 0",getInterfacePosition(current_iface),current_iface_id);
		hsystemcmd(cmdbuffer);
		WRITE_RUN();
	}
	else
	{
		cb.message = CMDIF_ENCAPSULATION_ERROR();
		freeCutString(enc,nbargs);
		return cb;
	}

	freeCutString(enc,nbargs);
	return cb;
}

cmdCallback cifCMD_ip_helper(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* iphelper[1];
	uint8_t nbargs = cutString(args,iphelper,1);

	if(strlen(args) == 0 || nbargs == -1 || is_valid_ip(iphelper[0]) != 0)
	{
		cb.message = CMDIF_IP_HELPER_ERROR();
		freeCutString(iphelper,nbargs);
		return cb;
	}

	if(addInterfaceIPHelper(current_iface,iphelper[0]) == 0)
	{
		char buffer[64];
		bzero(buffer,64);
		if(current_iface_id == 0)
			sprintf(buffer,"dhcrelay -i %s %s",current_iface,iphelper[0]);
		else
			sprintf(buffer,"dhcrelay -i vlan%d%d %s",getInterfacePosition(current_iface),current_iface_id,iphelper[0]);
		system(buffer);
	}

	freeCutString(iphelper,nbargs);
	return cb;
}

cmdCallback cifCMD_noip_helper(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* iphelper[1];
	uint8_t nbargs = cutString(args,iphelper,1);

	if(strlen(args) == 0 || nbargs == -1 || is_valid_ip(iphelper[0]) != 0)
	{
		cb.message = CMDIF_IP_HELPER_ERROR();
		freeCutString(iphelper,nbargs);
		return cb;
	}

	delInterfaceIPHelper(current_iface,iphelper[0]);

	char buffer[150];
	bzero(buffer,150);
	if(current_iface_id == 0)
		sprintf(buffer,"kill -9 $(ps ax |grep dhcrelay|grep %s|grep %s|awk '{print $1}')",current_iface,iphelper[0]);
	else
		sprintf(buffer,"kill -9 $(ps ax |grep dhcrelay|grep vlan%d%d|grep %s|awk '{print $1}')",getInterfacePosition(current_iface),current_iface_id,iphelper[0]);
	system(buffer);

	freeCutString(iphelper,nbargs);
	return cb;
}

cmdCallback cifCMD_help(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(strlen(_none) > 0)
		return cb;

	char buffer[3072];
	bzero(buffer,3072);
	uint8_t i;
	for(i=0;i<MAX_CONF_IF_CMD;i++)
	{
		char buffer2[256];
		bzero(buffer2,256);
		sprintf(buffer2,"%s%s\n",confIfCmd[i].name,confIfCmd[i].help);
		strcat(buffer,buffer2);
	}

	cb.message = (char*)malloc((strlen(buffer)+1)*sizeof(char));
	strcpy(cb.message,buffer);
	return cb;
}

cmdCallback cifCMD_mac(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* macaddr[1];
	uint8_t nbargs = cutString(args,macaddr,1);

	if(nbargs != 1 || is_valid_macaddr(macaddr[0]) != 0 && strcmp(macaddr[0],"random") != 0)
	{
		cb.message = CMDIF_MACADDR_ERROR();
		freeCutString(macaddr,nbargs);
		return cb;
	}

	setInterfaceMAC(current_iface,macaddr[0]);
	char buffer[1024];
	bzero(buffer,1024);
	sprintf(buffer,"ifconfig %s lladdr %s",current_iface,macaddr[0]);
	system(buffer);
	WRITE_RUN();
	freeCutString(macaddr,nbargs);
	return cb;
}

cmdCallback cifCMD_nomac(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	char* macaddr[1];
	uint8_t nbargs = cutString(args,macaddr,1);

	if(strlen(args) == 0 || nbargs == -1 || is_valid_macaddr(macaddr[0]) != 0 && strcmp(macaddr[0],"random") != 0)
	{
		cb.message = CMDIF_MACADDR_ERROR();
		freeCutString(macaddr,nbargs);
		return cb;
	}

	if(strcmp(getInterfaceMAC(current_iface),macaddr[0]) == 0)
	{
		setInterfaceMAC(current_iface,"");
		char buffer[1024];
		bzero(buffer,1024);
		sprintf(buffer,"ifconfig %s lladdr %s",current_iface,getInterfaceRealMAC(current_iface));
		system(buffer);
	}

	WRITE_RUN();
	freeCutString(macaddr,nbargs);
	return cb;
}

cmdCallback cifCMD_speed(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDIF_SPEED_ERROR();
		return cb;
	}

	if(current_iface_id != 0)
	{
		cb.message = CMD_UNK();
		return cb;
	}

	if(strcmp(args,"auto") == 0)
	{
		char cmdbuffer[256];
		bzero(cmdbuffer,256);
		sprintf(cmdbuffer,"ifconfig %s media autoselect",current_iface);
		system(cmdbuffer);
		setInterfaceSpeed(current_iface,"");
		WRITE_RUN();
	}

	char* speed[1];
	uint8_t nbargs = cutString(args,speed,1);

	if(nbargs == -1)
	{
		cb.message = CMDIF_SPEED_ERROR();
		freeCutString(speed,nbargs);
		return cb;
	}

	char output[1024];
	char output2[1024];
	char buffer[128];
	char buffer2[128];
	bzero(output,1024);
	bzero(output2,1024);
	bzero(buffer,128);
	bzero(buffer2,128);

	sprintf(buffer,"ifconfig %s media | grep media | awk '{print $2}' | grep 1|uniq|cut -f'1' -d 'b'",current_iface);
	sprintf(buffer2,"ifconfig %s media | grep media | awk '{print $2}' | grep 1|uniq",current_iface);
	execSystemCommand(buffer,output);
	execSystemCommand(buffer2,output2);

	char* speeds[128];
	char* realspeeds[128];
	uint8_t nbif = cutString(output,speeds,128);
	cutString(output2,realspeeds,128);

	if(nbif == -1)
	{
		freeCutString(speeds,nbif);
		freeCutString(realspeeds,nbif);
		freeCutString(speed,nbargs);
		return cb;
	}

	uint8_t i;
	for(i=0;i<nbif;i++)
	{
		if(strcmp(speed[0],speeds[i]) == 0)
		{
			char cmdbuffer[256];
			bzero(cmdbuffer,256);
			sprintf(cmdbuffer,"ifconfig %s media %s",current_iface,realspeeds[i]);
			system(cmdbuffer);
			setInterfaceSpeed(current_iface,speed[0]);
			WRITE_RUN();
			freeCutString(speeds,nbif);
			freeCutString(realspeeds,nbif);
			freeCutString(speed,nbargs);
			return cb;
		}
	}

	bzero(buffer,128);
	sprintf(buffer,"Invalid speed, valid speeds are \n%s",output);
	cb.message = printError(buffer);
	freeCutString(speeds,nbif);
	freeCutString(realspeeds,nbif);
	freeCutString(speed,nbargs);
	return cb;
}

cmdCallback cifCMD_nospeed(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDIF_SPEED_ERROR();
		return cb;
	}

	if(current_iface_id != 0)
	{
		cb.message = CMD_UNK();
		return cb;
	}

	char* speed[1];
	uint8_t nbargs = cutString(args,speed,1);

	if(nbargs == -1)
	{
		cb.message = CMDIF_SPEED_ERROR();
		freeCutString(speed,nbargs);
		return cb;
	}

	if(strcmp(getInterfaceSpeed(current_iface),speed[0]) == 0)
	{
		char cmdbuffer[256];
		bzero(cmdbuffer,256);
		sprintf(cmdbuffer,"ifconfig %s media autoselect",current_iface);
		system(cmdbuffer);
		setInterfaceSpeed(current_iface,"");
		WRITE_RUN();
	}

	freeCutString(speed,nbargs);
	return cb;
}

cmdCallback cifCMD_duplex(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDIF_DUPLEX_ERROR();
		return cb;
	}

	if(current_iface_id != 0)
	{
		cb.message = CMD_UNK();
		return cb;
	}

	char* duplex[1];
	uint8_t nbargs = cutString(args,duplex,1);

	if(nbargs == -1)
	{
		cb.message = CMDIF_DUPLEX_ERROR();
		freeCutString(duplex,nbargs);
		return cb;
	}

	char output[1024];
	char output2[1024];
	char buffer[128];
	char buffer2[128];
	bzero(output,1024);
	bzero(output2,1024);
	bzero(buffer,128);
	bzero(buffer2,128);

	sprintf(buffer,"ifconfig %s media | grep mediaopt | awk '{print $4}' | awk -F'-' '{print $1}'|uniq",current_iface);
	sprintf(buffer2,"ifconfig %s media | grep mediaopt | awk '{print $4}'|uniq",current_iface);
	execSystemCommand(buffer,output);
	execSystemCommand(buffer2,output2);

	char* dupl[128];
	char* reals[128];
	uint8_t nbif = cutString(output,dupl,128);
	cutString(output2,reals,128);

	uint8_t i;
	for(i=0;i<nbif;i++)
	{
		if(strcmp(duplex[0],dupl[i]) == 0)
		{
			char cmdbuffer[256];
			bzero(cmdbuffer,256);
			sprintf(cmdbuffer,"ifconfig %s mediaopt %s",current_iface,reals[i]);
			system(cmdbuffer);
			if(strcmp(reals[i],"full-duplex") == 0)
				setInterfaceDuplex(current_iface,DUPLEX_FULL);
			else if(strcmp(reals[i],"half-duplex") == 0)
				setInterfaceDuplex(current_iface,DUPLEX_HALF);

			WRITE_RUN();
			freeCutString(dupl,nbif);
			freeCutString(reals,nbif);
			freeCutString(duplex,nbargs);
			return cb;
		}
	}

	bzero(buffer,128);
	sprintf(buffer,"Invalid dulex, valid duplexes are \n%s",output);
	cb.message = printError(buffer);

	freeCutString(dupl,nbif);
	freeCutString(reals,nbif);
	freeCutString(duplex,nbargs);
	return cb;
}

cmdCallback cifCMD_noduplex(char* args)
{
	cmdCallback cb = {PROMPT_CONF_IF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDIF_DUPLEX_ERROR();
		return cb;
	}

	if(current_iface_id != 0)
	{
		cb.message = CMD_UNK();
		return cb;
	}

	char* duplex[1];
	uint8_t nbargs = cutString(args,duplex,1);

	if(nbargs == -1)
	{
		cb.message = CMDIF_DUPLEX_ERROR();
		freeCutString(duplex,nbargs);
		return cb;
	}

	char curduplex[10];
	bzero(curduplex,10);
	if(getInterfaceDuplex(current_iface) == DUPLEX_FULL)
		strcpy(curduplex,"full");
	else if(getInterfaceDuplex(current_iface) == DUPLEX_HALF)
		strcpy(curduplex,"half");
	else
		strcpy(curduplex,"");

	if(strcmp(curduplex,duplex[0]) == 0)
	{
		char cmdbuffer[256];
		bzero(cmdbuffer,256);

		sprintf(cmdbuffer,"ifconfig %s media autoselect",current_iface);
		system(cmdbuffer);
		if(strlen(getInterfaceSpeed(current_iface)) > 0)
			cifCMD_speed(getInterfaceSpeed(current_iface));
		setInterfaceDuplex(current_iface,DUPLEX_AUTO);
		WRITE_RUN();
	}

	freeCutString(duplex,nbargs);
	return cb;
}