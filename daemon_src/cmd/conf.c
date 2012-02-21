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
#include <stdio.h>
#include "conf.h"
#include "conf_if.h"
#include "../prompt/prompt_msg.h"
#include "../configuration.h"
#include "../iputils.h"
#include "../route.h"
#include "../string_mgmt.h"

cmdCallback cCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_ENABLE;

	return cb;
}

cmdCallback cCMD_firewall(char* _none)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDCONF_FIREWALL_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF_FW;

	return cb;
}

cmdCallback cCMD_router(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* routertype[1];
	uint8_t nbargs = cutString(args,routertype);

	if(nbargs != 1)
	{
		cb.message = CMDCONF_ROUTER_ERROR();
	}
	else
	{
		if(strcmp(routertype[0],"rip") == 0)
		{
			rip_enabled = 1;
			cb.promptMode = PROMPT_CONF_ROUTER_RIP;
			WRITE_RUN();
		}
		else if(strcmp(routertype[0],"ospf") == 0)
		{
			ospf_enabled = 1;
			cb.promptMode = PROMPT_CONF_ROUTER_OSPF;
			WRITE_RUN();
		}
	}

	freeCutString(routertype,nbargs);
	return cb;
}

cmdCallback cCMD_norouter(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* routertype[1];
	uint8_t nbargs = cutString(args,routertype);

	if(nbargs != 1)
	{
		cb.message = CMDCONF_ROUTER_ERROR();
	}
	else
	{
		if(strcmp(routertype[0],"rip") == 0)
		{
			rip_enabled = 0;
			cb.promptMode = PROMPT_CONF_ROUTER_RIP;
			hsystemcmd("kill -9 $(ps aux|grep ripd|grep parent|awk '{print $2}')");
			WRITE_RUN();
		}
		else if(strcmp(routertype[0],"ospf") == 0)
		{
			ospf_enabled = 0;
			cb.promptMode = PROMPT_CONF_ROUTER_OSPF;
			hsystemcmd("kill -9 $(ps aux|grep ospfd|grep parent|awk '{print $2}')");
			WRITE_RUN();
		}
	}

	freeCutString(routertype,nbargs);
	return cb;
}

cmdCallback cCMD_hostname(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* hname[1];
	uint8_t nbargs = cutString(args,hname);

	if(nbargs != 1)
	{
		cb.message = CMDCONF_HOSTNAME_ERROR();
	}
	else
	{
		hostname = hname[0];
		WRITE_RUN();
		WRITE_HOSTNAME();
	}

	freeCutString(hname,nbargs);
	return cb;
}

cmdCallback cCMD_nohostname(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* hname[1];
	uint8_t nbargs = cutString(args,hname);

	if(nbargs != 1)
	{
		cb.message = CMDCONF_HOSTNAME_ERROR();
	}
	else
	{
		if(strcmp(hname[0],hostname) == 0)
		{
			hostname = "PFShell";
			WRITE_RUN();
			WRITE_HOSTNAME();
		}
	}

	freeCutString(hname,nbargs);
	return cb;
}

cmdCallback cCMD_interface(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* iface[1];
	uint8_t nbargs = cutString(args,iface);

	if(nbargs != 1)
	{
		cb.message = CMDCONF_INTERFACE_ERROR();
		freeCutString(iface,nbargs);
		return cb;
	}

	net_iface* cursor = interfaces;
	short found = 0;

	if(cursor != NULL)
	{
		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,args) == 0)
				found = 1;
			else
				cursor = cursor->next;
		}
	}

	if(found == 0)
	{
		cb.message = CMDCONF_INTERFACE_UNK(args);
		freeCutString(iface,nbargs);
		return cb;
	}

	current_iface = args;
	cb.promptMode = PROMPT_CONF_IF;

	freeCutString(iface,nbargs);
	return cb;
}

cmdCallback cCMD_ip(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* _ip[4];
	uint8_t nbargs = cutString(args,_ip);

	if(nbargs < 1)
	{
		cb.message = CMDCONF_IP_ERROR();
		freeCutString(_ip,nbargs);
		return cb;
	}

	char* ipcmd[2];
	cutFirstWord(args,ipcmd);

	if(strcmp(_ip[0],"domain-name") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDCONF_IPDN_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		if(regexp(_ip[1],"^([a-zA-Z0-9][a-zA-Z0-9_-]*(\.[a-zA-Z0-9][a-zA-Z0-9_-]*)+)$") == 0)
		{
			strcpy(dnssearch,_ip[1]);
			WRITE_RUN();
		}
		else
			cb.message = CMDCONF_IPDN_ERROR();
	}
	else if(strcmp(_ip[0],"multicast-routing") == 0)
	{
		if(nbargs != 1)
		{
			cb.message = CMDCONF_IP_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		mcastrouting = 1;
		hsystemcmd("/usr/sbin/sysctl -w net.inet.ip.mforwarding=1");
		hsystemcmd("/usr/sbin/sysctl -w net.inet6.ip6.mforwarding=1");
		WRITE_RUN();
	}
	else if(strcmp(_ip[0],"name-server") == 0)
	{
		if(nbargs != 2)
		{
			cb.message = CMDCONF_IPNS_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		if(is_valid_ip(_ip[1]) == 0)
		{
			strcpy(dnsip,_ip[1]);
			WRITE_RUN();
		}
		else
		{
			cb.message = CMDCONF_IPNS_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	else if(strcmp(_ip[0],"route") == 0)
	{
		if(nbargs != 4)
		{
			cb.message = CMDCONF_IPROUTE_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		if(is_valid_ip(_ip[1]) == 0)
		{
			if(is_valid_mask(_ip[2]) == 0)
			{
				unsigned short cidr = calc_cidr(_ip[2]);

				if(is_valid_ip(_ip[3]) == 0)
				{
					char buffer[1024] = "";
					char cidrbuf[100] = "";
					sprintf(cidrbuf,"%d",cidr);

					strcpy(buffer,"route add ");
					strcat(buffer,_ip[1]);
					strcat(buffer,"/");
					strcat(buffer,cidrbuf);
					strcat(buffer," ");
					strcat(buffer,_ip[3]);

					addRoute(_ip[1],_ip[2],_ip[3]);

					hsystemcmd(buffer);

					WRITE_RUN();
				}
				else
				{
					cb.message = CMDCONF_IPROUTE_ERROR();
					freeCutString(_ip,nbargs);
					return cb;
				}
			}
			else
			{
				cb.message = CMDCONF_IPROUTE_ERROR();
				freeCutString(_ip,nbargs);
				return cb;
			}
		}
		else
		{
			cb.message = CMDCONF_IPROUTE_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	else if(strcmp(_ip[0],"routing") == 0)
	{
		if(nbargs != 1)
		{
			cb.message = CMDCONF_IP_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		iprouting = 1;
		hsystemcmd("/usr/sbin/sysctl -w net.inet.ip.forwarding=1");
		hsystemcmd("/usr/sbin/sysctl -w net.inet6.ip6.forwarding=1");
		WRITE_RUN();
	}
	else
		cb.message = CMDCONF_IP_ERROR();

	freeCutString(_ip,nbargs);
	return cb;
}

cmdCallback cCMD_noip(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	char* _ip[4];
	uint8_t nbargs = cutString(args,_ip);

	if(nbargs < 1)
	{
		cb.message = CMDCONF_NOIP_ERROR();
		return cb;
	}

	if(strcmp(_ip[0],"domain-name") == 0)
	{
		if(nbargs != 2)
		{
			freeCutString(_ip,nbargs);
			return cb;
		}

		if(strcmp(_ip[1],dnssearch) == 0)
		{
			strcpy(dnssearch,"");
			WRITE_RUN();
		}
		else
		{
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	else if(strcmp(_ip[0],"multicast-routing") == 0)
	{
		if(nbargs == 1)
		{
			cb.message = CMDCONF_NOIP_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		mcastrouting = 0;
		hsystemcmd("/usr/sbin/sysctl -w net.inet.ip.mforwarding=0");
		hsystemcmd("/usr/sbin/sysctl -w net.inet6.ip6.mforwarding=0");
		WRITE_RUN();
	}
	else if(strcmp(_ip[0],"name-server") == 0)
	{
		if(nbargs != 2)
		{
			freeCutString(_ip,nbargs);
			return cb;
		}

		if(strcmp(_ip[1],dnsip) == 0)
		{
			strcpy(dnsip,"");
			WRITE_RUN();
		}
		else
		{
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	else if(strcmp(_ip[0],"route") == 0)
	{
		if(nbargs != 4)
		{
			cb.message = CMDCONF_NOIPROUTE_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		if(is_valid_ip(_ip[1]) == 0)
		{
			if(is_valid_mask(_ip[2]) == 0)
			{
				unsigned short cidr = calc_cidr(_ip[2]);

				if(is_valid_ip(_ip[3]) == 0)
				{
					char buffer[1024] = "";
					char cidrbuf[100] = "";
					sprintf(cidrbuf,"%d",cidr);

					strcpy(buffer,"route delete ");
					strcat(buffer,_ip[1]);
					strcat(buffer,"/");
					strcat(buffer,cidrbuf);
					strcat(buffer," ");
					strcat(buffer,_ip[3]);

					delRoute(_ip[1],_ip[2],_ip[3]);

					hsystemcmd(buffer);

					WRITE_RUN();
				}
				else
				{
					cb.message = CMDCONF_NOIPROUTE_ERROR();
					freeCutString(_ip,nbargs);
					return cb;
				}
			}
			else
			{
				cb.message = CMDCONF_NOIPROUTE_ERROR();
				freeCutString(_ip,nbargs);
				return cb;
			}
		}
		else
		{
			cb.message = CMDCONF_NOIPROUTE_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}
	}
	else if(strcmp(_ip[0],"routing") == 0)
	{
		if(nbargs != 1)
		{
			cb.message = CMDCONF_NOIP_ERROR();
			freeCutString(_ip,nbargs);
			return cb;
		}

		iprouting = 0;
		hsystemcmd("/usr/sbin/sysctl -w net.inet.ip.forwarding=0");
		hsystemcmd("/usr/sbin/sysctl -w net.inet6.ip6.forwarding=0");
		WRITE_RUN();
	}

	freeCutString(_ip,nbargs);
	return cb;
}
