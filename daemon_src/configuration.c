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
#include <stdio.h>
#include <sys/types.h>

#include "configuration.h"
#include "firewall.h"
#include "interface.h"
#include "iputils.h"
#include "route.h"
#include "string_mgmt.h"
#include "cmd_command.h"
#include "cmd_conf_fw.h"
#include "cmd_conf.h"
#include "prompt_msg.h"


uint8_t loadConfiguration(void)
{
	// Temporary path
	FILE* confFile;
	confFile = fopen("/opt/PFShell/startup-config","r");
	if(!confFile)
	{
		system("mkdir -p /opt/PFShell >> /dev/null 2>&1");
		confFile = fopen("/opt/PFShell/startup-config", "w+");
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
	mcastrouting = 0;

	rip_enabled = 0;
	rip_split_horizon = 1;
	rip_redistrib_static = 0;
	rip_redistrib_conn = 0;
	rip_redistrib_default = 0;
	rip_update_timer = 30;
	rip_fail_timer = 180;
	rip_dead_timer = 240;

	ospf_enabled = 0;
	ospf_redistrib_conn = 0;
	ospf_redistrib_conn_metric = OSPF_DEFAULT_COST;
	ospf_redistrib_conn_type = OSPF_DEFAULT_METRIC_TYPE;
	ospf_redistrib_default = 0;
	ospf_redistrib_default_metric = OSPF_DEFAULT_COST;
	ospf_redistrib_default_type = OSPF_DEFAULT_METRIC_TYPE;
	ospf_redistrib_static = 0;
	ospf_redistrib_static_metric = OSPF_DEFAULT_COST;
	ospf_redistrib_static_type = OSPF_DEFAULT_METRIC_TYPE;
	ospfredistnets = NULL;
	ospf_router_id = "";
	ospf_delay_timer = OSPF_DEFAULT_DELAY;
	ospf_holdtime_timer = OSPF_DEFAULT_HOLDTIME;

	hostname = "PFShell";
	strcpy(dnssearch,"local");
	strcpy(dnsip,"");

	// Read file
	char path[1035] = "";

	cmdCallback promptMode = {PROMPT_CONF,""};
	while (fgets(path, sizeof(path), confFile) != NULL) {
		int len = strlen(path);
		int offset = 0;
		while(offset < len)
		{
			if(path[offset] == '\n')
				path[offset] = '\0';
			++offset;
		}
		promptMode = handleCmd(path,promptMode.promptMode);
	}
	fclose(confFile);
	return 1;
}

uint8_t writeRunningConfig(void)
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

		// DNS
		if(strlen(dnsip) > 10)
		{
			fputs("ip name-server ",confFile);
			fputs(dnsip,confFile);
			fputs("\n",confFile);
		}

		if(strlen(dnssearch) > 1)
		{
			fputs("ip domain-name ",confFile);
			fputs(dnssearch,confFile);
			fputs("\n",confFile);
		}

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

		if(rip_enabled == 1)
		{
			fputs("router rip\n", confFile);
			if(rip_redistrib_conn == 1)
				fputs("redistribute connected\n",confFile);
			if(rip_redistrib_default == 1)
				fputs("redistribute default\n",confFile);
			if(rip_redistrib_static == 1)
				fputs("redistribute static\n",confFile);
			if(rip_split_horizon == 0)
				fputs("no split-horizon\n",confFile);
			if(rip_update_timer != 30 || rip_fail_timer != 180 || rip_dead_timer != 240)
				fprintf(confFile,"timers %d %d %d\n",rip_update_timer,rip_fail_timer,rip_dead_timer);
			// Passive-Interfaces
			net_iface* if_cursor = interfaces;
			while(if_cursor != NULL)
			{
				if(if_cursor->is_rip_network > 0)
				{
					fputs("network ",confFile);
					fputs(if_cursor->name,confFile);
					fputs("\n",confFile);
				}
				if_cursor = if_cursor->next;
			}
			// Reinit cursor to write passive arg, and order file
			if_cursor = interfaces;
			while(if_cursor != NULL)
			{
				if(if_cursor->rip_passive > 0)
				{
					fputs("passive-interface ",confFile);
					fputs(if_cursor->name,confFile);
					fputs("\n",confFile);
				}
				if_cursor = if_cursor->next;
			}
			fputs("!\n",confFile);
		}

		if(ospf_enabled == 1)
		{
			fputs("router ospf\n", confFile);
			if(is_valid_ip(ospf_router_id) == 0)
				fprintf(confFile,"router-id %s\n",ospf_router_id);

			if(ospf_delay_timer != OSPF_DEFAULT_DELAY || ospf_holdtime_timer != OSPF_DEFAULT_HOLDTIME)
				fprintf(confFile,"timers spf %d %d\n",ospf_delay_timer,ospf_holdtime_timer);

			if(ospf_redistrib_conn == 1)
			{
				fputs("redistribute connected",confFile);
				if(ospf_redistrib_conn_metric != 100 || ospf_redistrib_conn_type != 1)
				{
					fprintf(confFile," metric %d",ospf_redistrib_conn_metric);
					if(ospf_redistrib_conn_type != 1)
						fprintf(confFile," metric-type %d",ospf_redistrib_conn_type);
				}
				fputs("\n",confFile);
			}
			if(ospf_redistrib_default == 1)
			{
				fputs("redistribute default",confFile);
				if(ospf_redistrib_default_metric != 100 || ospf_redistrib_default_type != 1)
				{
					fprintf(confFile," metric %d",ospf_redistrib_default_metric);
					if(ospf_redistrib_default_type != 1)
						fprintf(confFile," metric-type %d",ospf_redistrib_default_type);
				}
				fputs("\n",confFile);
			}
			if(ospf_redistrib_static == 1)
			{
				fputs("redistribute static",confFile);
				if(ospf_redistrib_static_metric != 100 || ospf_redistrib_static_type != 1)
				{
					fprintf(confFile," metric %d",ospf_redistrib_static_metric);
					if(ospf_redistrib_static_type != 1)
						fprintf(confFile," metric-type %d",ospf_redistrib_static_type);
				}
				fputs("\n",confFile);
			}

			ospf_redist_net* orn_cursor = ospfredistnets;
			while(orn_cursor != NULL)
			{
				fprintf(confFile,"redistribute %s",orn_cursor->net);
				if(orn_cursor->metric != OSPF_DEFAULT_METRIC || orn_cursor->type != OSPF_DEFAULT_METRIC_TYPE)
				{
					fprintf(confFile," metric %d",orn_cursor->metric);
					if(orn_cursor->type != 1)
						fprintf(confFile," metric-type %d",orn_cursor->type);
				}
				fputs("\n",confFile);
				orn_cursor = orn_cursor->next;
			}
			// Passive-Interfaces
			net_iface* if_cursor = interfaces;
			while(if_cursor != NULL)
			{
				if(if_cursor->ospf_passive > 0)
					fprintf(confFile,"passive-interface %s\n",if_cursor->name);
				if_cursor = if_cursor->next;
			}
			// Areas
			ospf_area* oa = ospfareas;
			while(oa != NULL)
			{
				char* iflist[256];
				uint8_t ifnb = cutString(oa->ifacelist,iflist);

				uint8_t i;
				for(i=0;i<ifnb;i++)
					fprintf(confFile,"network %s area %s\n",iflist[i],convert_int_to_ip(oa->id));

				freeCutString(iflist,ifnb);

				if(oa->stub > 0)
					fprintf(confFile,"area %s stub%s\n",convert_int_to_ip(oa->id),(oa->stub_summary == 0) ? " no-summary" : "");

				oa = oa->next;
			}
			fputs("!\n",confFile);
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

				if(cursor2->_proto == 0)
					fputs("tcp ",confFile);
				else if(cursor2->_proto == 1)
					fputs("udp ",confFile);
				else
					fputs("icmp ",confFile);

				fputs(cursor2->_saddr,confFile);

				char buffer[1024] = "";

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
				fprintf(confFile,"description %s\n",if_cursor->desc);

			if(if_cursor->vlan > 0 && if_cursor->vlan <= 1005)
				fprintf(confFile,"encapsulation dot1q %d\n",if_cursor->vlan);

			if(strlen(if_cursor->ip) > 0)
				fprintf(confFile,"ip address %s\n",if_cursor->ip);

			if(strlen(if_cursor->mac_addr) > 0)
				fprintf(confFile,"mac-address %s\n",if_cursor->mac_addr);

			if(if_cursor->state == 0)
				fputs("shutdown\n",confFile);

			if(strlen(if_cursor->ip_helper_list) > 0)
			{
				char* helpers[64];
				uint8_t nbhelpers = cutString(if_cursor->ip_helper_list,helpers);

				uint8_t i;
				for(i=0;i<nbhelpers;i++)
				{
					if(is_valid_ip(helpers[i]) == 0)
						fprintf(confFile,"ip helper-address %s\n",helpers[i]);
				}
			}

			if(strlen(if_cursor->acl_in) > 0)
				fprintf(confFile,"ip access-group %s in\n",if_cursor->acl_in);

			if(strlen(if_cursor->acl_out) > 0)
				fprintf(confFile,"ip access-group %s out\n",if_cursor->acl_out);

			if(if_cursor->rip_cost != RIP_DEFAULT_COST)
				fprintf(confFile,"ip rip cost %d\n",if_cursor->rip_cost);

			if(if_cursor->rip_auth_type != RIP_AUTH_NONE)
			{
				fputs("ip rip authentication mode ",confFile);
				if(if_cursor->rip_auth_type == RIP_AUTH_TEXT)
					fputs("text",confFile);
				else if(if_cursor->rip_auth_type == RIP_AUTH_MD5)
					fputs("md5",confFile);
				fputs("\n",confFile);
			}

			if(strlen(if_cursor->rip_auth_pwd) > 0 && strlen(if_cursor->rip_auth_pwd) <= 16)
				fprintf(confFile,"ip rip authentication key-string %s\n",if_cursor->rip_auth_pwd);

			if(if_cursor->ospf_cost != OSPF_DEFAULT_COST)
				fprintf(confFile,"ip ospf cost %d\n",if_cursor->ospf_cost);

			if(if_cursor->ospf_priority != 1)
				fprintf(confFile,"ip ospf priority %d\n",if_cursor->ospf_priority);

			if(if_cursor->ospf_hello_int != 10)
				fprintf(confFile,"ip ospf hello-interval %d\n",if_cursor->ospf_hello_int);

			if(if_cursor->ospf_dead_int != 40)
				fprintf(confFile,"ip ospf dead-interval %d\n",if_cursor->ospf_dead_int);

			if(if_cursor->ospf_transmit_delay != 1)
				fprintf(confFile,"ip ospf transmit-delay %d\n",if_cursor->ospf_transmit_delay);

			if(if_cursor->ospf_retransmit_delay != 5)
				fprintf(confFile,"ip ospf retransmit-interval %d\n",if_cursor->ospf_retransmit_delay);

			if(if_cursor->ospf_auth_type != RIP_AUTH_NONE)
			{
				fputs("ip ospf authentication mode ",confFile);
				if(if_cursor->ospf_auth_type == RIP_AUTH_TEXT)
					fputs("text",confFile);
				else if(if_cursor->ospf_auth_type == RIP_AUTH_MD5)
					fputs("md5",confFile);
				fputs("\n",confFile);
			}

			if(strlen(if_cursor->ospf_auth_pwd) > 0 && strlen(if_cursor->ospf_auth_pwd) <= 16)
				fprintf(confFile,"ip ospf authentication key-string %s\n",if_cursor->ospf_auth_pwd);
			fputs("!\n",confFile);
			if_cursor = if_cursor->next;
		}

		fclose(confFile);

		system("/bin/md5 /opt/PFShell/running-config | awk '{print $4}' > /opt/PFShell/running-config.md5");
		system("/bin/sha1 /opt/PFShell/running-config | awk '{print $4}' > /opt/PFShell/running-config.sha1");
		system("/bin/sha256 /opt/PFShell/running-config | awk '{print $4}' > /opt/PFShell/running-config.sha256");
	}

	writeFirewall();

	confFile = fopen("/etc/resolv.conf","w");
	if(confFile == NULL)
	{
		CMD_WRITE_RESOLV_FAIL();
	}
	else
	{
		if(strlen(dnssearch) > 0)
		{
			fputs("search ",confFile);
			fputs(dnssearch,confFile);
			fputs("\n",confFile);
		}

		if(strlen(dnsip) > 0)
		{
			fputs("nameserver ",confFile);
			fputs(dnsip,confFile);
			fputs("\n",confFile);
		}

		fclose(confFile);
	}
	return 0;
}

uint8_t saveHostname(void)
{
	FILE* fname = fopen("/etc/myname.run","w");
	if(fname != NULL)
	{
		fputs(hostname,fname);
		fclose(fname);
	}

	return 0;
}
