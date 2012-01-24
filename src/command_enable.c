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

#include "configuration.h"
#include "command_enable.h"
#include "interface.h"
#include "prompt_msg.h"
#include "route.h"

void eCMD_configure(char* args) {
	if(strlen(args) > 0)
	{
		if(strcmp(args,"firewall") == 0)
		{
			promptMode = PROMPT_CONF_FW;
		}
		else
			CMDEN_CONFIGURE_ERROR();
	}
	else
		promptMode = PROMPT_CONF;
}

void eCMD_exit(char* _none) {
	if(strlen(_none) > 0)
	{
		CMDCOMMON_EXIT_ERROR();
	}
	else
		promptMode = PROMPT_USER;
}

void eCMD_show(char* args)
{
	if(strlen(args) <= 1)
	{
		CMDEN_SHOW_ERROR();
	}
	else
	{
		char* showcmd[2];
		cutFirstWord(args,showcmd);
		if(strcmp(showcmd[0],"version") == 0)
		{
			printf("PFShell version %s\n",VERSION);
		}
		else if(strcmp(showcmd[0],"acls") == 0)
		{
			printf("------------- Current ACLs -------------\n");
			unsigned short found = 0;
			acl* cursor = access_lists;
			while(cursor != NULL)
			{
				printf("Access-List %s\n",cursor->name);
				access_control* cursor2 = cursor->ac;
				while(cursor2 != NULL)
				{
					found = 1;
					putchar(' ');
					if(cursor2->_allow == 0)
						printf("deny ");
					else
						printf("allow ");

					if(cursor2->_direction == 0)
						printf("in ");
					else
						printf("out ");

					if(cursor2->_proto == 0)
						printf("tcp ");
					else if(cursor2->_proto == 1)
						printf("udp ");
					else
						printf("icmp ");

					printf("%s ",cursor2->_saddr);
					if(cursor2->_sport > 0)
						printf("%d ",cursor2->_sport);

					printf("%s ",cursor2->_daddr);
					if(cursor2->_dport > 0)
						printf("%d",cursor2->_dport);
					printf("\n");
					cursor2 = cursor2->next;
				}
				putchar('\n');
				cursor = cursor->next;
			}
			if(found == 0) printf("No Access-Lists found !\n");
		}
		else if(strcmp(showcmd[0],"firewall") == 0)
		{
			char* fwArg[2];
			cutFirstWord(showcmd[1],fwArg);
			if(strcmp(fwArg[0],"stats") == 0)
			{
				if(strlen(fwArg[1]) > 0)
				{
					CMDEN_SHOW_FIREWALL_ERROR();
				}
				else
					system("/sbin/pfctl -si");
			}
			else
				CMDEN_SHOW_FIREWALL_ERROR();
		}
		else if(strcmp(showcmd[0],"interfaces") == 0)
		{
			net_iface* iface = interfaces;
			if(iface == NULL)
			{
				CMDEN_SHOW_INTERFACES_NOTFOUND();
			}
			else
			{
				printf("Running Interfaces:\n");
				while(iface != NULL)
				{
					char buffer[1024] = "";
					char output[10240] = "";

					strcpy(buffer,"/sbin/ifconfig ");
					strcat(buffer,iface->name);

					execSystemCommand(buffer,output);

					printf("%s\n",output);
					iface = iface->next;
				}
			}
		}
		else if(strcmp(showcmd[0],"ip") == 0)
		{
			char* ipt[2];
			cutFirstWord(showcmd[1],ipt);

			if(strcmp(ipt[0],"routes") == 0)
			{
				char output[10240] = "";
				execSystemCommand("netstat -rn -f inet",output);
				printf("\n-------------------------------------------------------------------\n");
				printf("%s-------------------------------------------------------------------\n",output);
			}
			else
				CMDEN_SHOW_IP_ERROR();
		}
		else if(strcmp(showcmd[0],"running-config") == 0)
		{
			char output[10240] = "";
			execSystemCommand("cat /opt/PFShell/running-config",output);
			printf("\n%s\n",output);
		}
		else if(strcmp(showcmd[0],"startup-config") == 0)
		{
			char output[10240] = "";
			execSystemCommand("cat /opt/PFShell/startup-config",output);
			printf("\n%s\n",output);
		}
		else
		{
			CMDEN_SHOW_ERROR();
		}
	}
}

void eCMD_save(char* _none)
{
	// Save PFShell conf
	system("cp /opt/PFShell/running-config /opt/PFShell/startup-config");
	system("/bin/md5 /opt/PFShell/startup-config | /usr/bin/awk '{print $4}' > /opt/PFShell/startup-config.md5");
	system("/bin/sha1 /opt/PFShell/startup-config | /usr/bin/awk '{print $4}' > /opt/PFShell/startup-config.sha1");
	system("/bin/sha256 /opt/PFShell/startup-config | /usr/bin/awk '{print $4}' > /opt/PFShell/startup-config.sha256");
	// Save system interface conf
	saveInterfaces();
	// Save sysctl vars
	saveSysctl();
	system("/bin/md5 /etc/sysctl.conf | /usr/bin/awk '{print $4}' > /etc/sysctl.conf.md5");
	system("/bin/sha1 /etc/sysctl.conf | /usr/bin/awk '{print $4}' > /etc/sysctl.conf.sha1");
	system("/bin/sha256 /etc/sysctl.conf | /usr/bin/awk '{print $4}' > /etc/sysctl.conf.sha256");
	// Save PacketFilter conf
	system("cp /etc/pf.conf.run /etc/pf.conf");
	system("/bin/md5 /etc/pf.conf | /usr/bin/awk '{print $4}' > /etc/pf.conf.md5");
	system("/bin/sha1 /etc/pf.conf | /usr/bin/awk '{print $4}' > /etc/pf.conf.sha1");
	system("/bin/sha256 /etc/pf.conf | /usr/bin/awk '{print $4}' > /etc/pf.conf.sha256");
	CMDEN_SAVE_SUCCESS();
}
