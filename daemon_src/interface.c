/*
* Copyright (c) 2011-2012, CNRS
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
#include "interface.h"
#include "configuration.h"
#include "cmd_command.h"
#include "iputils.h"
#include "string_mgmt.h"
#include "sysunix.h"

void addInterface(char* name)
{
	net_iface* newIface = (net_iface*)malloc(sizeof(net_iface));
	newIface->name = (char*)malloc(strlen(name)*sizeof(char));
	strcpy(newIface->name,name);
	newIface->next = NULL;
	newIface->prev = NULL;
	newIface->ip = "";
	newIface->acl_in = "";
	newIface->acl_out = "";
	newIface->desc = "";
	newIface->vlan = 0;
	newIface->state = 1;
	newIface->ip_helper_list = "";
	newIface->is_rip_network = 0;
	newIface->rip_passive = 0;
	newIface->rip_cost = RIP_DEFAULT_COST;
	newIface->rip_auth_type = RIP_AUTH_NONE;
	newIface->rip_auth_pwd = "";
	newIface->ospf_passive = 0;
	newIface->ospf_cost = OSPF_DEFAULT_COST;
	newIface->ospf_priority = 1;
	newIface->ospf_hello_int = 10;
	newIface->ospf_dead_int = 40;
	newIface->ospf_transmit_delay = 1;
	newIface->ospf_retransmit_delay = 5;
	newIface->ospf_auth_type = RIP_AUTH_NONE;
	newIface->ospf_auth_pwd = "";
	newIface->ospf_area_id = 0;

	if(interfaces == NULL)
	{
		interfaces = newIface;
	}
	else
	{
		net_iface* cursor = interfaces;
		if(strcmp(name,cursor->name) == 0)
			return;

		while(cursor->next != NULL) {
			cursor = cursor->next;
			if(strcmp(name,cursor->name) == 0)
				return;
		}

		newIface->prev = cursor;
		cursor->next = newIface;
	}
}

void delInterface(char* name)
{
	if(interfaces == NULL)
		return;


	uint8_t found = 0;
	net_iface* cursor = interfaces;

	while(cursor != NULL && found == 0)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			found = 1;
			net_iface* tmpcursor = cursor;
			if(cursor == interfaces && cursor->next == NULL)
				interfaces = NULL;
			else
			{
				if(cursor->prev != NULL)
					cursor->prev->next = cursor->next;
				else
					interfaces = cursor->next;

				if(cursor->next != NULL)
					cursor->next->prev = cursor->prev;

			}
			free(tmpcursor);
		}
		else
			cursor = cursor->next;
	}
}


void loadInterfaces(void)
{
	char output[1024] = "";
#ifdef FREEBSD
	execSystemCommand("for IF in $(/sbin/ifconfig | grep HWaddr | awk '{print $1}'); do echo $IF; done;",output);
#else
	execSystemCommand("for IF in $(/sbin/ifconfig | grep BROADCAST | awk '{print $1}' | awk -F':' '{print $1}'); do echo $IF; done;",output);
#endif
	if(strcmp(output,"") == 0)
		return;

	char* iface[128];
	uint8_t nbif = cutString(output,iface);

	uint8_t i;
	for(i=0;i<nbif;i++)
	{
		if((strlen(iface[i]) < 2) || (strlen(iface[i]) > 4 && iface[i][0] == 'v' && iface[i][1] == 'l' && iface[i][2] == 'a' && iface[i][3] == 'n'))
			continue;

		if(getInterfacePosition(iface[i]) == -1)
			addInterface(iface[i]);
	}
}

unsigned short saveInterfaces(void)
{
	net_iface* cursor = interfaces;
	if(interfaces == NULL)
		return 0;

	while(cursor != NULL)
	{
		if(strcmp(cursor->ip,"DHCP") == 0)
		{
			if(getInterfaceState(cursor->name) == 1)
			{
				char pathbuffer[100] = "";
				strcpy(pathbuffer,"/etc/hostname.");
				strcat(pathbuffer,cursor->name);

				FILE* fIface = fopen(pathbuffer,"w+");
				fwrite("dhcp\n",1,sizeof("dhcp\n"),fIface);
				fclose(fIface);
			}
		}
		else
		{
			char* ipmask[2];
			cutFirstWord(cursor->ip,ipmask);
			if(strlen(ipmask[1]) > 1)
			{
				if(is_valid_ip(ipmask[0]) == 0)
				{
					if(is_valid_mask(ipmask[1]) == 0)
					{
						char buffer[1024] = "";
						char pathbuffer[100] = "";
						strcpy(pathbuffer,"/etc/hostname.");
						strcat(pathbuffer,cursor->name);

						strcpy(buffer,"inet ");
						strcat(buffer,ipmask[0]);
						strcat(buffer," ");
						strcat(buffer,ipmask[1]);
						strcat(buffer," NONE\n");

						FILE* fIface = fopen(pathbuffer,"w+");
						fwrite(buffer,1,strlen(buffer),fIface);
						fclose(fIface);
					}
				}
			}
			else if(is_valid_ip_and_cidr(ipmask[0]) == 0)
			{
				char* mask[2];
				cutByChar(ipmask[0],mask,'/');

				char buffer[1024] = "";
				char pathbuffer[100] = "";

				strcpy(pathbuffer,"/etc/hostname.");
				strcat(pathbuffer,cursor->name);

				strcpy(buffer,"inet ");
				strcat(buffer,mask[0]);
				strcat(buffer," ");
				strcat(buffer,calc_mask_from_cidr(mask[1]));
				strcat(buffer," NONE\n");

				FILE* fIface = fopen(pathbuffer,"w+");
				fwrite(buffer,1,strlen(buffer),fIface);
				fclose(fIface);
			}
		}
		cursor = cursor->next;
	}

	return 0;
}

unsigned short setInterfaceIP(char* name, char* ip)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ip = ip;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

int8_t getInterfacePosition(char* name)
{
	int8_t pos = 0;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;

		while(cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
				return pos;
			else
			{
				pos++;
				cursor = cursor->next;
			}
		}
	}

	return -1;
}

char* getInterfaceIP(char* name)
{
	char _ip[50] = "";

	if(interfaces == NULL)
		return "";
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				strcpy(_ip,cursor->ip);
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return "";
	}

	return _ip;
}

unsigned short setInterfaceDesc(char* name, char* desc)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->desc = desc;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

char* getInterfaceDesc(char* name)
{
	char _desc[50] = "";

	if(interfaces == NULL)
		return "";
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				strcpy(_desc,cursor->desc);
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return "";
	}

	return _desc;
}

unsigned short setInterfaceRIPNetwork(char* name, uint8_t network)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->is_rip_network = network;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

uint8_t getInterfaceRIPNetwork(char* name)
{
	uint8_t result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->is_rip_network;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceVLAN(char* name, uint16_t vlan)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->vlan = vlan;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short setInterfaceState(char* name, unsigned short state)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->state = state;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

uint16_t getInterfaceVLAN(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;

		while(cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
				return cursor->vlan;
			else
				cursor = cursor->next;
		}
	}

	return result;
}

unsigned short getInterfaceState(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->state;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFCost(char* name, unsigned short cost)
{
	if(cost > 16 && cost < 1)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_cost = cost;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFCost(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_cost;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFPriority(char* name, uint8_t prio)
{
	/*if(prio > 255 && prio < 0) // Due to variable type
		return 1;*/

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_priority = prio;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFPriority(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_priority;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFHello(char* name, unsigned short hello)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_hello_int = hello;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFHello(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_hello_int;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFDead(char* name, unsigned int dead)
{
	if(dead > 2147483647 && dead < 2)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_dead_int = dead;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned int getInterfaceOSPFDead(char* name)
{
	unsigned int result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_dead_int;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFTransmit(char* name, unsigned short transmit)
{
	if(transmit > 3600 && transmit < 1)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_transmit_delay = transmit;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFTransmit(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_transmit_delay;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFRetransmit(char* name, unsigned short transmit)
{
	if(transmit > 3600 && transmit < 5)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_retransmit_delay = transmit;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFRetransmit(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_retransmit_delay;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceRIPCost(char* name, unsigned short cost)
{
	if(cost > 16 && cost < 1)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->rip_cost = cost;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceRIPCost(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->rip_cost;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFAuthKey(char* name, char* key)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_auth_pwd = key;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short setInterfaceRIPAuthKey(char* name, char* key)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->rip_auth_pwd = key;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

char* getInterfaceOSPFAuthKey(char* name)
{
	char _key[50] = "";

	if(interfaces == NULL)
		return "";
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				strcpy(_key,cursor->ospf_auth_pwd);
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return "";
	}

	return _key;
}

char* getInterfaceRIPAuthKey(char* name)
{
	char _key[50] = "";

	if(interfaces == NULL)
		return "";
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				strcpy(_key,cursor->rip_auth_pwd);
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return "";
	}

	return _key;
}

unsigned short setInterfaceOSPFAuthType(char* name, unsigned short type)
{
	if(type > RIP_AUTH_MD5)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_auth_type = type;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short setInterfaceRIPAuthType(char* name, unsigned short type)
{
	if(type > RIP_AUTH_MD5)
		return 1;

	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->rip_auth_type = type;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFAuthType(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_auth_type;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short getInterfaceRIPAuthType(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->rip_auth_type;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceRIPPassive(char* name, unsigned short passive)
{
	if(interfaces == NULL)
		return 1;

	net_iface* cursor = interfaces;
	unsigned short found = 0;

	while(found == 0 && cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			found = 1;
			cursor->rip_passive = passive;
		}
		else
			cursor = cursor->next;
	}

	if(found == 0)
		return 1;

	return 0;
}

unsigned short getInterfaceRIPPassive(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->rip_passive;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceOSPFPassive(char* name, unsigned short passive)
{
	if(interfaces == NULL)
		return 1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				cursor->ospf_passive = passive;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}

unsigned short getInterfaceOSPFPassive(char* name)
{
	unsigned short result = -1;

	if(interfaces == NULL)
		return -1;
	else
	{
		net_iface* cursor = interfaces;
		unsigned short found = 0;

		while(found == 0 && cursor != NULL)
		{
			if(strcmp(cursor->name,name) == 0)
			{
				found = 1;
				result = cursor->ospf_passive;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return -1;
	}

	return result;
}

unsigned short setInterfaceACL(char* name, char* aclname, char* direction)
{
	if(interfaces == NULL)
		return 1;

		net_iface* cursor = interfaces;
	unsigned short found = 0;

	while(found == 0 && cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			found = 1;

			if(strlen(aclname) != 0)
			{
				acl* cursor2 = access_lists;
				unsigned short found2 = 0;

				while(found2 == 0 && cursor2 != NULL)
				{
					if(strcmp(cursor2->name,aclname) == 0)
					{
						found2 = 1;

						if(strcmp(direction,"in") == 0)
							cursor->acl_in = aclname;
						else if(strcmp(direction,"out") == 0)
							cursor->acl_out = aclname;
						else
							return 1;
					}
					else
						cursor2 = cursor2->next;
				}

				if(found2 == 0)
					return 1;
			}
			else
			{
				if(strcmp(direction,"in") == 0)
					cursor->acl_in = "";
				else if(strcmp(direction,"out") == 0)
					cursor->acl_out = "";
			}
		}
		else
			cursor = cursor->next;
	}

	if(found == 0)
		return 1;

	return 0;
}

uint8_t is_interface(char* name)
{
	if(interfaces == NULL)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return 0;
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t addInterfaceIPHelper(char* name, char* ip)
{
	if(interfaces == NULL)
		return 1;

	if(is_valid_ip(ip) != 0)
		return 2;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			char* helpers[64];
			uint8_t nbhelpers = cutString(cursor->ip_helper_list,helpers);

			uint8_t i;
			for(i=0;i<nbhelpers;i++)
			{
				if(strcmp(helpers[i],ip) == 0)
					return 3;
			}

			char* buffer = (char*)malloc((strlen(cursor->ip_helper_list)+1+1+15)*sizeof(char));
			strcpy(buffer,cursor->ip_helper_list);
			strcat(buffer," ");
			strcat(buffer,ip);

			if(strlen(cursor->ip_helper_list) > 0)
				free(cursor->ip_helper_list);

			cursor->ip_helper_list = (char*)malloc((strlen(cursor->ip_helper_list)+1+1+15)*sizeof(char));
			bzero(cursor->ip_helper_list,strlen(cursor->ip_helper_list)+1+1+15);
			strcpy(cursor->ip_helper_list,buffer);

			WRITE_RUN();
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

void delInterfaceIPHelper(char* name, char* ip)
{
	if(interfaces == NULL)
		return;

	if(is_valid_ip(ip) != 0)
		return;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			char* buffer = (char*)malloc(strlen(cursor->ip_helper_list)*sizeof(char));
			bzero(buffer,strlen(cursor->ip_helper_list));

			char* helpers[64];
			uint8_t nbhelpers = cutString(cursor->ip_helper_list,helpers);

			uint8_t first = 1;

			uint8_t i;
			for(i=0;i<nbhelpers;i++)
			{
				if(strcmp(helpers[i],ip) != 0)
				{
					if(first != 1)
						strcat(buffer," ");
					if(first == 1) first = 0;
					strcat(buffer,helpers[i]);
				}
			}

			cursor->ip_helper_list = (char*)malloc(strlen(buffer)*sizeof(char));
			bzero(cursor->ip_helper_list,strlen(buffer));
			strcpy(cursor->ip_helper_list,buffer);

			WRITE_RUN();
			return;
		}
		else
			cursor = cursor->next;
	}

	return;
}

char* getInterfaceIPHelpers(char* name)
{
	if(interfaces == NULL)
		return NULL;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ip_helper_list;
		else
			cursor = cursor->next;
	}

	return NULL;
}

void launchInterfaceIPHelpers(char* name)
{
	if(interfaces == NULL)
		return;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			char* helpers[64];
			uint8_t nbhelpers = cutString(cursor->ip_helper_list,helpers);

			char* subiface[2];
			cutByChar(name,subiface,'.');

			uint8_t i;
			for(i=0;i<nbhelpers;i++)
			{
				if(is_valid_ip(helpers[i]) == 0)
				{
					char buffer[50] = "";
					if(strlen(subiface[1]) == 0)
						sprintf(buffer,"dhcrelay -i %s %s",cursor->name,helpers[i]);
					else
						sprintf(buffer,"dhcrelay -i vlan%d%s %s",getInterfacePosition(cursor->name),subiface[1],helpers[i]);
					system(buffer);
				}
			}

			freeCutString(subiface,strlen(subiface[1]) > 0 ? 2 : 1);
			return;
		}
		else
			cursor = cursor->next;
	}
}
