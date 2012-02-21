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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "route.h"
#include "configuration.h"
#include "string_mgmt.h"

void addRoute(char* ip, char* mask, char* gate)
{
	route* newRoute = (route*)malloc(sizeof(route));

	newRoute->net = (char*)malloc(strlen(ip)*sizeof(char));
	strcpy(newRoute->net,ip);
	newRoute->mask = (char*)malloc(strlen(mask)*sizeof(char));
	strcpy(newRoute->mask,mask);
	newRoute->gate = (char*)malloc(strlen(gate)*sizeof(char));
	strcpy(newRoute->gate,gate);
	newRoute->next = NULL;
	newRoute->prev = NULL;

	if(routes == NULL)
	{
		routes = newRoute;
	}
	else
	{
		route* cursor = routes;

		while(cursor->next != NULL)
			cursor = cursor->next;

		newRoute->prev = cursor;
		cursor->next = newRoute;
	}
}

void addOSPFArea(char* id)
{
	ospf_area* oa = (ospf_area*)malloc(sizeof(ospf_area));

	oa->id = (char*)malloc(strlen(id)*sizeof(char));
	strcpy(oa->id,id);
	oa->ifacelist = "";
	oa->stub = 0;
	oa->next = NULL;
	oa->prev = NULL;

	if(ospfareas == NULL)
	{
		ospfareas = oa;
	}
	else
	{
		ospf_area* cursor = ospfareas;

		while(cursor->next != NULL)
			cursor = cursor->next;

		oa->prev = cursor;
		cursor->next = oa;
	}
}

void delRoute(char* ip, char* mask, char* gate)
{
	if(routes == NULL)
		return;


	uint8_t found = 0;
	route* cursor = routes;

	while(cursor != NULL && found == 0)
	{
		if(strcmp(cursor->net,ip) == 0 && strcmp(cursor->mask,mask) == 0 && strcmp(cursor->gate,gate) == 0)
		{
			found = 1;
			route* tmpcursor = cursor;
			if(cursor == routes && cursor->next == NULL)
				routes = NULL;
			else
			{
				if(cursor->prev != NULL)
					cursor->prev->next = cursor->next;
				else
					routes = cursor->next;

				if(cursor->next != NULL)
					cursor->next->prev = cursor->prev;

			}
			free(tmpcursor);
		}
		else
			cursor = cursor->next;
	}
}

void delOSPFArea(char* id)
{
	if(ospfareas == NULL)
		return;


	uint8_t found = 0;
	ospf_area* cursor = ospfareas;

	while(cursor != NULL && found == 0)
	{
		if(strcmp(cursor->id,id) == 0)
		{
			found = 1;
			ospf_area* tmpcursor = cursor;
			if(cursor == ospfareas && cursor->next == NULL)
				ospfareas = NULL;
			else
			{
				if(cursor->prev != NULL)
					cursor->prev->next = cursor->next;
				else
					ospfareas = cursor->next;

				if(cursor->next != NULL)
					cursor->next->prev = cursor->prev;

			}
			free(tmpcursor);
		}
		else
			cursor = cursor->next;
	}
}

void saveSysctl()
{
	FILE* fSysctl = fopen("/etc/sysctl.conf","w+");
	if(iprouting > 0)
	{
		char ipfw[100] = "net.inet.ip.forwarding=1\n";
		char ip6fw[100] = "net.inet6.ip6.forwarding=1\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}
	else
	{
		char ipfw[100] = "net.inet.ip.forwarding=0\n";
		char ip6fw[100] = "net.inet6.ip6.forwarding=0\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}

	if(mcastrouting > 0)
	{
		char ipfw[100] = "net.inet.ip.mforwarding=1\n";
		char ip6fw[100] = "net.inet6.ip6.mforwarding=1\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}
	else
	{
		char ipfw[100] = "net.inet.ip.mforwarding=0\n";
		char ip6fw[100] = "net.inet6.ip6.mforwarding=0\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}

	fclose(fSysctl);
}

void saveRipd()
{
	FILE* fRIPd = fopen("/etc/ripd.conf","w+");

	// Global configuration
	if(rip_enabled == 1)
	{
		fputs("#\n#Global configuration\n#\n\n",fRIPd);

		fwrite("fib-update yes\n",1,strlen("fib-update yes\n"),fRIPd);
		fwrite("triggered-updates yes\n",1,strlen("triggered-updates yes\n"),fRIPd);

		if(rip_split_horizon == 0)
			fwrite("split-horizon none\n",1,strlen("split-horizon none\n"),fRIPd);
		else
			fwrite("split-horizon poisoned\n",1,strlen("split-horizon poisoned\n"),fRIPd);

		if(rip_redistrib_conn == 1)
			fwrite("redistribute connected\n",1,strlen("redistribute connected\n"),fRIPd);
		else
			fwrite("no redistribute connected\n",1,strlen("no redistribute connected\n"),fRIPd);

		if(rip_redistrib_static == 1)
			fwrite("redistribute static\n",1,strlen("redistribute static\n"),fRIPd);
		else
			fwrite("no redistribute static\n",1,strlen("no redistribute static\n"),fRIPd);

		if(rip_redistrib_default == 1)
			fwrite("redistribute default\n",1,strlen("redistribute default\n"),fRIPd);
		else
			fwrite("no redistribute default\n",1,strlen("no redistribute default\n"),fRIPd);

		if(rip_update_timer != 30)
			fprintf(fRIPd,"update-timer %d\n",rip_update_timer);
		if(rip_fail_timer != 180)
			fprintf(fRIPd,"fail-timer %d\n",rip_fail_timer);
		if(rip_dead_timer != 240)
			fprintf(fRIPd,"dead-timer %d\n",rip_dead_timer);
	}

	fputs("\n#\n#Interfaces configuration\n#\n\n",fRIPd);

	// Interface specific
	net_iface* if_cursor = interfaces;
	while(if_cursor != NULL)
	{
		if(if_cursor->is_rip_network > 0)
		{
			fputs("interface ",fRIPd);
			fputs(if_cursor->name,fRIPd);
			fputs("{\n",fRIPd);

			if(if_cursor->rip_passive == 1)
				fputs("\tpassive\n",fRIPd);
			if(if_cursor->rip_cost > 1 && if_cursor->rip_cost < 16)
				fprintf(fRIPd,"\tcost %d\n",if_cursor->rip_cost);

			if(if_cursor->rip_auth_type > RIP_AUTH_NONE && strlen(if_cursor->rip_auth_pwd) > 0 && strlen(if_cursor->rip_auth_pwd) < 17)
			{
				if(if_cursor->rip_auth_type == RIP_AUTH_TEXT)
				{
					fputs("\tauth-type simple\n",fRIPd);
					fprintf(fRIPd,"\tauth-key %s\n",if_cursor->rip_auth_pwd);
				}
				else if(if_cursor->rip_auth_type == RIP_AUTH_MD5)
				{
					fputs("\tauth-type crypt\n",fRIPd);
					fprintf(fRIPd,"\tauth-md 1 \"%s\"\n",if_cursor->rip_auth_pwd);
					fputs("\tauth-md-keyid 1\n",fRIPd);
				}
			}
			fputs("}\n",fRIPd);
		}
		if_cursor = if_cursor->next;
	}

	fclose(fRIPd);
	if(is_loading == 0)
	{
		hsystemcmd("kill -9 $(ps aux|grep ripd|grep parent|awk '{print $2}')");
		system("/opt/bin/ripd");
	}
}

void saveOspfd()
{
	FILE* fOSPFd = fopen("/etc/ospfd.conf","w+");

	// Global configuration
	if(ospf_enabled == 1)
	{
		fputs("#\n#Global configuration\n#\n\n",fOSPFd);

		fputs("fib-update yes\n",fOSPFd);

		if(is_valid_ip(ospf_router_id) == 0)
			fprintf(fOSPFd,"router-id %s\n",ospf_router_id);

		if(ospf_delay_timer != OSPF_DEFAULT_DELAY)
			fprintf(fOSPFd,"spf-delay %d\n",ospf_delay_timer);

		if(ospf_holdtime_timer != OSPF_DEFAULT_HOLDTIME)
			fprintf(fOSPFd,"spf-holdtime %d\n",ospf_holdtime_timer);

		if(ospf_redistrib_conn == 1)
		{
			fputs("redistribute connected",fOSPFd);
			if(ospf_redistrib_conn_metric != OSPF_DEFAULT_METRIC || ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
				fprintf(fOSPFd," set { metric %d type %d }",ospf_redistrib_conn_metric,ospf_redistrib_conn_type);
			fputs("\n",fOSPFd);
		}
		else
			fputs("no redistribute connected\n",fOSPFd);

		if(ospf_redistrib_static == 1)
		{
			fputs("redistribute static",fOSPFd);
			if(ospf_redistrib_static_metric != OSPF_DEFAULT_METRIC || ospf_redistrib_static_type != OSPF_DEFAULT_METRIC_TYPE)
				fprintf(fOSPFd," set { metric %d type %d }",ospf_redistrib_static_metric,ospf_redistrib_static_type);
			fputs("\n",fOSPFd);
		}
		else
			fputs("no redistribute static\n",fOSPFd);

		if(ospf_redistrib_default == 1)
		{
			fputs("redistribute default",fOSPFd);
			if(ospf_redistrib_default_metric != OSPF_DEFAULT_METRIC || ospf_redistrib_default_type != OSPF_DEFAULT_METRIC_TYPE)
				fprintf(fOSPFd," set { metric %d type %d }",ospf_redistrib_default_metric,ospf_redistrib_default_type);
			fputs("\n",fOSPFd);
		}
		else
			fputs("no redistribute default\n",fOSPFd);
	}

	fputs("\n#\n# Areas configuration\n#\n\n",fOSPFd);

	// @ TODO interfaces & areas

	if(is_loading == 0)
	{
		hsystemcmd("kill -9 $(ps aux|grep ospfd|grep parent|awk '{print $2}')");
		hsystemcmd("/usr/sbin/ospfd");
	}

	fclose(fOSPFd);
}