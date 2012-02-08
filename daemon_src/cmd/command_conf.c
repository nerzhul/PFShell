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
#include "command_conf.h"
#include "command_conf_if.h"
#include "../prompt/prompt_msg.h"
#include "../configuration.h"
#include "../iputils.h"
#include "../route.h"

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

cmdCallback cCMD_hostname(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(args) <= 1)
	{
		cb.message = CMDCONF_HOSTNAME_ERROR();
	}
	else
	{
		char* _hostname[2];
		cutFirstWord(args,_hostname);
		if(strlen(_hostname[1]) > 0)
		{
			cb.message = CMDCONF_HOSTNAME_ERROR();
		}
		else
		{
			hostname = _hostname[0];
			FILE* fname = fopen("/etc/myname","w+");
			if(fname != NULL)
			{
				fputs(hostname,fname);
				fclose(fname);
			}
			writeRunningConfig();
		}
	}

	return cb;
}

cmdCallback cCMD_nohostname(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(args) > 1)
	{
		char* _hostname[2];
		cutFirstWord(args,_hostname);
		if(strlen(_hostname[1]) == 0 && strcmp(_hostname[0],hostname) == 0)
		{
			hostname = "PFShell";
			writeRunningConfig();
		}
	}

	return cb;
}

cmdCallback cCMD_interface(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDCONF_INTERFACE_ERROR();
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
		return cb;
	}

	current_iface = args;
	cb.promptMode = PROMPT_CONF_IF;

	return cb;
}

cmdCallback cCMD_ip(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDCONF_IP_ERROR();
		return cb;
	}

	char* ipcmd[2];
	cutFirstWord(args,ipcmd);

	if(strcmp(ipcmd[0],"domain-name") == 0)
	{
		if(strlen(ipcmd[1]) < 2)
		{
			cb.message = CMDCONF_IPDN_ERROR();
			return cb;
		}

		char* dname[2];
		cutFirstWord(ipcmd[1],dname);
		if(regexp(dname[0],"^([a-zA-Z0-9][a-zA-Z0-9_-]*(\.[a-zA-Z0-9][a-zA-Z0-9_-]*)+)$") == 0 && strlen(dname[1]) < 1)
		{
			strcpy(dnssearch,dname[0]);
			WRITE_RUN();
		}
		else
			cb.message = CMDCONF_IPDN_ERROR();
	}
	else if(strcmp(ipcmd[0],"multicast-routing") == 0)
	{
		if(strlen(ipcmd[1]) > 0)
		{
			cb.message = CMDCONF_IP_ERROR();
			return cb;
		}

		mcastrouting = 1;
		hsystemcmd("/usr/sbin/sysctl net.inet.ip.mforwarding=1");
		hsystemcmd("/usr/sbin/sysctl net.inet6.ip6.mforwarding=1");
		WRITE_RUN();
	}
	else if(strcmp(ipcmd[0],"name-server") == 0)
	{
		if(strlen(ipcmd[1]) < 6)
		{
			cb.message = CMDCONF_IPNS_ERROR();
			return cb;
		}

		char* netip[2];
		cutFirstWord(ipcmd[1],netip);
		if(regexp(netip[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0
			&& strlen(netip[1]) < 1)
		{
			strcpy(dnsip,netip[0]);
			WRITE_RUN();
		}
		else
		{
			cb.message = CMDCONF_IPNS_ERROR();
			return cb;
		}
	}
	else if(strcmp(ipcmd[0],"route") == 0)
	{
		if(strlen(ipcmd[1]) < 15)
		{
			cb.message = CMDCONF_IPROUTE_ERROR();
			return cb;
		}

		char* netip[2];
		cutFirstWord(ipcmd[1],netip);

		if(regexp(netip[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
		{
			char* maskip[2];

			cutFirstWord(netip[1],maskip);

			if(is_valid_mask(maskip[0]) == 0)
			{
				unsigned short cidr = calc_cidr(maskip[0]);
				char* gateip[2];
				cutFirstWord(maskip[1],gateip);

				if(regexp(gateip[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
				{
					if(strlen(gateip[1]) > 0)
					{
						cb.message = CMDCONF_IPROUTE_ERROR();
						return cb;
					}

					char buffer[1024];
					char cidrbuf[100];
					sprintf(cidrbuf,"%d",cidr);

					strcpy(buffer,"route add ");
					strcat(buffer,netip[0]);
					strcat(buffer,"/");
					strcat(buffer,cidrbuf);
					strcat(buffer," ");
					strcat(buffer,gateip[0]);

					addRoute(netip[0],maskip[0],gateip[0]);

					hsystemcmd(buffer);

					WRITE_RUN();
				}
				else
				{
					cb.message = CMDCONF_IPROUTE_ERROR();
					return cb;
				}
			}
			else
			{
				cb.message = CMDCONF_IPROUTE_ERROR();
				return cb;
			}
		}
		else
		{
			cb.message = CMDCONF_IPROUTE_ERROR();
			return cb;
		}
	}
	else if(strcmp(ipcmd[0],"routing") == 0)
	{
		if(strlen(ipcmd[1]) > 0)
		{
			cb.message = CMDCONF_IP_ERROR();
			return cb;
		}

		iprouting = 1;
		hsystemcmd("/usr/sbin/sysctl net.inet.ip.forwarding=1");
		hsystemcmd("/usr/sbin/sysctl net.inet6.ip6.forwarding=1");
		WRITE_RUN();
	}
	else
		cb.message = CMDCONF_IP_ERROR();

	return cb;
}

cmdCallback cCMD_noip(char* args)
{
	cmdCallback cb = {PROMPT_CONF,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDCONF_NOIP_ERROR();
		return cb;
	}

	char* ipcmd[2];
	cutFirstWord(args,ipcmd);

	if(strcmp(ipcmd[0],"domain-name") == 0)
	{
		if(strlen(ipcmd[1]) < 15)
			return cb;

		char* netip[2];
		cutFirstWord(ipcmd[1],netip);

		if(strcmp(netip[0],dnssearch) == 0 && strlen(netip[1]) < 1)
		{
			strcpy(dnssearch,"");
			WRITE_RUN();
		}
		else
			return cb;
	}
	else if(strcmp(ipcmd[0],"multicast-routing") == 0)
	{
		if(strlen(ipcmd[1]) > 0)
		{
			cb.message = CMDCONF_NOIP_ERROR();
			return cb;
		}

		mcastrouting = 0;
		hsystemcmd("/usr/sbin/sysctl net.inet.ip.mforwarding=0");
		hsystemcmd("/usr/sbin/sysctl net.inet6.ip6.mforwarding=0");
		WRITE_RUN();
	}
	else if(strcmp(ipcmd[0],"name-server") == 0)
	{
		if(strlen(ipcmd[1]) < 15)
			return cb;

		char* netip[2];
		cutFirstWord(ipcmd[1],netip);

		if(strcmp(netip[0],dnsip) == 0 && strlen(netip[1]) < 1)
		{
			strcpy(dnsip,"");
			WRITE_RUN();
		}
		else
			return cb;
	}
	else if(strcmp(ipcmd[0],"route") == 0)
	{
		if(strlen(ipcmd[1]) < 15)
		{
			cb.message = CMDCONF_NOIPROUTE_ERROR();
			return cb;
		}

		char* netip[2];
		cutFirstWord(ipcmd[1],netip);

		if(regexp(netip[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
		{
			char* maskip[2];

			cutFirstWord(netip[1],maskip);

			if(is_valid_mask(maskip[0]) == 0)
			{
				unsigned short cidr = calc_cidr(maskip[0]);
				char* gateip[2];
				cutFirstWord(maskip[1],gateip);
				if(regexp(gateip[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
				{
					if(strlen(gateip[1]) > 0)
					{
						cb.message = CMDCONF_NOIPROUTE_ERROR();
						return cb;
					}

					char buffer[1024];
					char cidrbuf[100];
					sprintf(cidrbuf,"%d",cidr);

					strcpy(buffer,"route delete ");
					strcat(buffer,netip[0]);
					strcat(buffer,"/");
					strcat(buffer,cidrbuf);
					strcat(buffer," ");
					strcat(buffer,gateip[0]);

					delRoute(netip[0],maskip[0],gateip[0]);

					hsystemcmd(buffer);

					WRITE_RUN();
				}
				else
				{
					cb.message = CMDCONF_NOIPROUTE_ERROR();
					return cb;
				}
			}
			else
			{
				cb.message = CMDCONF_NOIPROUTE_ERROR();
				return cb;
			}
		}
		else
		{
			cb.message = CMDCONF_NOIPROUTE_ERROR();
			return cb;
		}
	}
	else if(strcmp(ipcmd[0],"routing") == 0)
	{
		if(strlen(ipcmd[1]) > 0)
		{
			cb.message = CMDCONF_NOIP_ERROR();
			return cb;
		}

		iprouting = 0;
		hsystemcmd("/usr/sbin/sysctl net.inet.ip.forwarding=0");
		hsystemcmd("/usr/sbin/sysctl net.inet6.ip6.forwarding=0");
		WRITE_RUN();
	}

	return cb;
}
