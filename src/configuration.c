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
#include "configuration.h"
#include "firewall.h"
#include "interface.h"
#include "route.h"
#include "command.h"
#include "command_conf_fw.h"
#include "command_conf.h"
#include "prompt_msg.h"

unsigned short loadConfiguration()
{
	// Temporary path
	FILE* confFile;
	confFile = fopen("/opt/PFShell/running-config","r");
	if(!confFile)
	{
		system("mkdir -p /opt/PFShell >> /dev/null 2>&1");
		confFile = fopen("/opt/PFShell/running-config", "w+");
		fclose(confFile);
		return 1;
	}

	firewallState = 1;

	pfpolicies[0] = 1;
	pfpolicies[1] = 1;

	access_lists = NULL;
	interfaces = NULL;
	routes = NULL;

	loadInterfaces();

	iprouting = 0;

	hostname = "PFShell";

	// Read file
	char path[1035];

	while (fgets(path, sizeof(path), confFile) != NULL) {
		int len = strlen(path);
		int offset = 0;
		while(offset < len)
		{
			if(path[offset] == '\n')
				path[offset] = '\0';
			++offset;
		}
		handleCmd(path);
	}
	fclose(confFile);
	return 1;
}

unsigned short writeRunningConfig()
{
	FILE* confFile;
	confFile = fopen("/opt/PFShell/running-config","w");
	if(confFile == NULL)
	{
		CMD_WRITE_RUN_FAIL();
	}
	else
	{
		// Hostname
		fputs("hostname ",confFile);
		fputs(hostname,confFile);
		fputs("\n",confFile);

		// Routing
		if(iprouting == 1)
			fputs("ip routing\n",confFile);

		if(mcastrouting == 1)
			fputs("ip multicast-routing\n",confFile);

		route* rcursor = routes;
		while(rcursor != NULL)
		{
			fputs("ip route ",confFile);
			fputs(rcursor->net,confFile);
			fputs(" ",confFile);
			fputs(rcursor->mask,confFile);
			fputs(" ",confFile);
			fputs(rcursor->gate,confFile);
			fputs("\n",confFile);
			rcursor = rcursor->next;
		}
		// Firewall
		fputs("firewall\n",confFile);
		if(firewallState == 0) fputs("disable\n",confFile);
		fputs("default input-policy ",confFile);
		fputs((pfpolicies[0] == 0 ? "deny" : "allow"),confFile);
		fputs("\n",confFile);
		fputs("default output-policy ",confFile);
		fputs((pfpolicies[1] == 0 ? "deny" : "allow"),confFile);
		fputs("\n",confFile);

		acl* cursor = access_lists;
		while(cursor != NULL)
		{
			fputs("access-list ",confFile);
			fputs(cursor->name,confFile);
			fputs("\n",confFile);
			access_control* cursor2 = cursor->ac;
			while(cursor2 != NULL)
			{
				if(cursor2->_allow == 0)
					fputs("deny ",confFile);
				else
					fputs("allow ",confFile);

				if(cursor2->_direction == 0)
					fputs("in ",confFile);
				else
					fputs("out ",confFile);

				if(cursor2->_proto == 0)
					fputs("tcp ",confFile);
				else if(cursor2->_proto == 1)
					fputs("udp ",confFile);
				else
					fputs("icmp ",confFile);

				fputs(cursor2->_saddr,confFile);

				char buffer[1024];

				if(cursor2->_sport > 0)
				{
					sprintf(buffer,"%d",cursor2->_sport);
					fputs(" ",confFile);
					fputs(buffer,confFile);
				}

				fputs(" ",confFile);

				fputs(cursor2->_daddr,confFile);

				if(cursor2->_dport > 0)
				{
					sprintf(buffer,"%d",cursor2->_dport);
					fputs(" ",confFile);
					fputs(buffer,confFile);
				}
				fputs("\n",confFile);
				cursor2 = cursor2->next;
			}
			cursor = cursor->next;
			fputs("!\n",confFile);
		}
		fputs("!\n",confFile);

		// Interfaces
		net_iface* if_cursor = interfaces;
		while(if_cursor != NULL)
		{
			fputs("interface ",confFile);
			fputs(if_cursor->name,confFile);
			fputs("\n",confFile);

			if(strlen(if_cursor->desc) > 0)
			{
				fputs("description ",confFile);
				fputs(if_cursor->desc,confFile);
				fputs("\n",confFile);
			}

			if(strlen(if_cursor->ip) > 0)
			{
				fputs("ip address ",confFile);
				fputs(if_cursor->ip,confFile);
				fputs("\n",confFile);
			}

			if(if_cursor->state == 0)
				fputs("shutdown\n",confFile);

			if(strlen(if_cursor->acl_in) > 0)
			{
				fputs("ip access-group ",confFile);
				fputs(if_cursor->acl_in,confFile);
				fputs(" in\n",confFile);
			}

			if(strlen(if_cursor->acl_out) > 0)
			{
				fputs("ip access-group ",confFile);
				fputs(if_cursor->acl_out,confFile);
				fputs(" out\n",confFile);
			}

			fputs("!\n",confFile);
			if_cursor = if_cursor->next;
		}

		fclose(confFile);

		hsystemcmd("/bin/md5 /opt/PFShell/running-config | awk '{print $4}' > /opt/PFShell/running-config.md5");
		hsystemcmd("/bin/sha1 /opt/PFShell/running-config | awk '{print $4}' > /opt/PFShell/running-config.sha1");
	}

	writeFirewall();
}
