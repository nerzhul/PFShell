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

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd/command_conf_acl.h"
#include "firewall.h"
#include "configuration.h"
#include <unistd.h>

void addAccessList(acl* _list, char* name)
{
	if(strcmp(_list->name,"") == 0)
	{
		_list->name = (char*)malloc(strlen(name)*sizeof(char));
		strcpy(_list->name,name);
		_list->next = NULL;
		_list->ac = NULL;
		_list->prev = NULL;
	}
	else
	{
		acl* newACL = (acl*)malloc(sizeof(acl));
		newACL->name = (char*)malloc(strlen(name)*sizeof(char));
		strcpy(newACL->name,name);
		newACL->next = NULL;
		newACL->ac = NULL;

		acl* cursor = _list;
		while(cursor->next != NULL)
			cursor = cursor->next;

		newACL->prev = cursor;
		cursor->next = newACL;
	}
}

void removeAccessList(char* name)
{
	if(access_lists == NULL)
		return;

	unsigned short found = 0;
	acl* cursor = access_lists;

	while(cursor != NULL && found == 0)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			found = 1;
			acl* tmpcursor = cursor;
			if(cursor == routes && cursor->next == NULL)
				access_lists = NULL;
			else
			{
				if(cursor->prev != NULL)
					cursor->prev->next = cursor->next;
				else
					access_lists = cursor->next;

				if(cursor->next != NULL)
					cursor->next->prev = cursor->prev;

			}

			if(tmpcursor->ac != NULL)
			{
				access_control* cursor2 = tmpcursor->ac;
				while(cursor2 != NULL)
				{
					access_control* tmpac = cursor2;
					cursor2 = cursor2->next;
					free(tmpac);
				}
			}
			free(tmpcursor);
		}
		else
			cursor = cursor->next;
	}
}

void addAccessControl(access_control* ac, unsigned short direction, unsigned short proto, unsigned short sport, unsigned short dport, char* saddr, char* daddr, unsigned short allow)
{
	if(ac->_direction == 10)
	{
		ac->_direction = direction;
		ac->_proto = proto;
		ac->_sport = sport;
		ac->_dport = dport;
		ac->_saddr = (char*)malloc(strlen(saddr)*sizeof(char));
		strcpy(ac->_saddr,saddr);
		ac->_daddr = (char*)malloc(strlen(daddr)*sizeof(char));
		strcpy(ac->_daddr,daddr);
		ac->_allow = allow;
		ac->next = NULL;
		ac->prev = NULL;
	}
	else
	{
		access_control* cursor = ac;

		while(cursor->next != NULL)
		{
			cursor = cursor->next;
		}

		access_control* newAC = (access_control*)malloc(sizeof(access_control));
		newAC->_direction = direction;
		newAC->_proto = proto;
		newAC->_sport = sport;
		newAC->_dport = dport;
		newAC->_saddr = (char*)malloc(strlen(saddr)*sizeof(char));
		strcpy(newAC->_saddr,saddr);
		newAC->_daddr = (char*)malloc(strlen(daddr)*sizeof(char));
		strcpy(newAC->_daddr,daddr);
		newAC->_allow = allow;

		cursor->next = newAC;
		newAC->prev = cursor;
		newAC->next = NULL;
	}
}

void addACL(char* listname, unsigned short direction, unsigned short proto, unsigned short sport, unsigned short dport, char* saddr, char* daddr, unsigned short allow)
{
	acl* cursor = access_lists;
	if(cursor != NULL && strcmp(cursor->name,listname) == 0)
	{
		if(cursor->ac == NULL)
		{
			cursor->ac = (access_control*)malloc(sizeof(access_control));
			cursor->ac->_direction = 10;
		}
		addAccessControl(cursor->ac, direction, proto, sport, dport, saddr, daddr, allow);
	}
	else
	{
		unsigned short found = 0;
		while(cursor != NULL && found == 0 && cursor->next != NULL)
		{
			cursor = cursor->next;
			if(strcmp(cursor->name,listname) == 0)
			{
				if(cursor->ac == NULL)
				{
					cursor->ac = (access_control*)malloc(sizeof(access_control));
					cursor->ac->_direction = 10;
				}
				addAccessControl(cursor->ac, direction, proto, sport, dport, saddr, daddr, allow);
				found = 1;
			}
		}

		if(found == 0)
		{
			if(access_lists == NULL)
			{
				access_lists = (acl*)malloc(sizeof(acl));
				access_lists->name = "";
			}
			addAccessList(access_lists, listname);
			addACL(listname, direction, proto, sport, dport, saddr, daddr, allow);
		}
	}
}

unsigned short readACL(char* args, unsigned short allow)
{
	char* nexttab[2];
	char _nextvar[1024];

	char* name;
	unsigned short _allow;
	unsigned short _direction;
	unsigned short _proto;

	char* _saddr;
	unsigned short _sport;

	char* _daddr;
	unsigned short _dport;

	name = (char*)malloc(strlen(current_acl)*sizeof(char));
	strcpy(name,current_acl);

	_allow = allow;

	strcpy(_nextvar,args);
	cutFirstWord(_nextvar,nexttab);

	if(strcmp(nexttab[0],"in") != 0 && strcmp(nexttab[0],"out") != 0 || strcmp(nexttab[1],"") == 0)
		return 1;

	_direction = (strcmp(nexttab[0],"in") == 0) ? 0 : 1;

	strcpy(_nextvar,nexttab[1]);
	cutFirstWord(_nextvar,nexttab);

	if(strcmp(nexttab[0],"tcp") != 0 && strcmp(nexttab[0],"udp") != 0 && strcmp(nexttab[0],"icmp") || strcmp(nexttab[1],"") == 0)
		return 1;

	if(strcmp(nexttab[0],"tcp") == 0) _proto = TCP;
	else if(strcmp(nexttab[0],"udp") == 0) _proto = UDP;
	else if(strcmp(nexttab[0],"icmp") == 0) _proto = ICMP;
	else return 1;

	strcpy(_nextvar,nexttab[1]);
	cutFirstWord(_nextvar,nexttab);

	if(strcmp(nexttab[0],"any") == 0)
	{
		if(strcmp(nexttab[1],"") == 0)
			return 1;

		_saddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
		strcpy(_saddr,nexttab[0]);
	}
	else
	{
		if(regexp(nexttab[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])((/([0-9]|[1-2][0-9]|3[0-2]))?)$") == 0)
		{
			_saddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
			strcpy(_saddr,nexttab[0]);
		}
		else
			return 1;
	}

	strcpy(_nextvar,nexttab[1]);
	cutFirstWord(_nextvar,nexttab);

	// atoi func get first segment of IP and traduce it into port, we must check if next is IP
	unsigned short notport = 0;
	if(regexp(nexttab[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])((/([0-9]|[1-2][0-9]|3[0-2]))?)$") == 0)
		notport = 1;

	int port = atoi(nexttab[0]);
	if(notport == 0 && port <= 65535 && port > 0)
	{
		if(_proto == ICMP)
			return 1;

		_sport = port;
		strcpy(_nextvar,nexttab[1]);
		cutFirstWord(_nextvar,nexttab);
	}
	else _sport = 0;

	if(strcmp(nexttab[0],"any") == 0)
	{
		_daddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
		strcpy(_daddr,nexttab[0]);
	}
	else
	{
		if(regexp(nexttab[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])((/([8-9]|[1-2][0-9]|3[0-2]))?)$") == 0)
		{
			_daddr = (char*)malloc(strlen(nexttab[0])*sizeof(char));
			strcpy(_daddr,nexttab[0]);
		}
		else
			return 1;
	}

	if(strcmp(nexttab[1],"") == 0)
		_dport = 0;
	else
	{
		strcpy(_nextvar,nexttab[1]);
		cutFirstWord(_nextvar,nexttab);

		int port2 = atoi(nexttab[0]);
		if(port2 > 65535 && port2 < 0)
			return 1;
		else
			_dport = port2;
	}

	addACL(name,_direction,_proto,_sport,_dport,_saddr,_daddr,_allow);

	return 0;
}

unsigned short writeFirewall()
{
	net_iface* cursor = interfaces;
	if(interfaces == NULL)
		return 0;

	FILE* fPF = fopen("/etc/pf.conf.run","w+");

	while(cursor != NULL)
	{
		if(strcmp(cursor->acl_in,"") != 0)
		{
			acl* cursor2 = access_lists;
			unsigned short found = 0;
			while(cursor2 != NULL && found == 0)
			{
				if(strcmp(cursor2->name,cursor->acl_in) == 0)
				{
					access_control* cursor3 = cursor2->ac;
					while(cursor3 != NULL)
					{
						// @Todo: optimize rules
						char buffer[1024];

						if(cursor3->_allow == 1) strcpy(buffer,"pass ");
						else strcpy(buffer,"block ");

						strcat(buffer,"in quick on ");
						strcat(buffer,cursor->name);
						strcat(buffer," proto ");

						if(cursor3->_proto == TCP) strcat(buffer,"tcp ");
						else if(cursor3->_proto == UDP) strcat(buffer,"udp ");
						else if(cursor3->_proto == ICMP) strcat(buffer,"icmp ");

						strcat(buffer,"from ");
						strcat(buffer,cursor3->_saddr);
						strcat(buffer," ");

						if(cursor3->_sport != 0)
						{
							char bufport[15];
							sprintf(bufport,"port %d ",cursor3->_sport);
							strcat(buffer,bufport);
						}

						strcat(buffer,"to ");
						strcat(buffer,cursor3->_daddr);
						strcat(buffer," ");

						if(cursor3->_dport != 0)
						{
							char bufport[15];
							sprintf(bufport,"port %d ",cursor3->_dport);
							strcat(buffer,bufport);
						}

						strcat(buffer,"\n");

						fwrite(buffer,1,strlen(buffer),fPF);
						cursor3 = cursor3->next;
					}
					found = 1;
				}
				cursor2 = cursor2->next;
			}
		}

		if(strcmp(cursor->acl_out,"") != 0)
		{
			acl* cursor2 = access_lists;
			unsigned short found = 0;
			while(cursor2 != NULL && found == 0)
			{
				if(strcmp(cursor2->name,cursor->acl_out) == 0)
				{
					access_control* cursor3 = cursor2->ac;
					while(cursor3 != NULL)
					{
						// @Todo: optimize rules
						char buffer[1024];

						if(cursor3->_allow == 1) strcpy(buffer,"pass ");
						else strcpy(buffer,"block ");

						strcat(buffer,"out quick on ");
						strcat(buffer,cursor->name);
						strcat(buffer," proto ");

						if(cursor3->_proto == TCP) strcat(buffer,"tcp ");
						else if(cursor3->_proto == UDP) strcat(buffer,"udp ");
						else if(cursor3->_proto == ICMP) strcat(buffer,"icmp ");

						strcat(buffer,"from ");
						strcat(buffer,cursor3->_saddr);
						strcat(buffer," ");

						if(cursor3->_sport != 0)
						{
							char bufport[15];
							sprintf(bufport,"port %d ",cursor3->_sport);
							strcat(buffer,bufport);
						}

						strcat(buffer,"to ");
						strcat(buffer,cursor3->_daddr);
						strcat(buffer," ");

						if(cursor3->_dport != 0)
						{
							char bufport[15];
							sprintf(bufport,"port %d ",cursor3->_dport);
							strcat(buffer,bufport);
						}

						strcat(buffer,"\n");
						fwrite(buffer,1,strlen(buffer),fPF);
						cursor3 = cursor3->next;
					}
					found = 1;
				}
				cursor2 = cursor2->next;
			}
		}

		cursor = cursor->next;
	}

	if(pfpolicies[0] == 0) fwrite("block in\n",1,strlen("block in\n"),fPF);
	else fwrite("pass in\n",1,strlen("pass in\n"),fPF);

	if(pfpolicies[1] == 0) fwrite("block out\n",1,strlen("block out\n"),fPF);
	else fwrite("pass out\n",1,strlen("pass out\n"),fPF);

	fclose(fPF);

	// Reload only if firewall is up
	if(firewallState > 0)
		hsystemcmd("/sbin/pfctl -f /etc/pf.conf.run");

	return 0;
}
