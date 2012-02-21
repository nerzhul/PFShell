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
#include "interface.h"
#include "configuration.h"
#include "cmd/command.h"
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
	newIface->state = 1;
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
	char buffer[30] = "";
	strcpy(buffer,"ifconfig bridge0 add ");
	strcat(buffer,name);
	hsystemcmd(buffer);
}

void loadInterfaces()
{
	char input[1024] = "";
	char output[1024] = "";
#ifdef FREEBSD
	execSystemCommand("for IF in $(/sbin/ifconfig | grep HWaddr | awk '{print $1}'); do echo $IF; done;",output);
#else
	execSystemCommand("for IF in $(/sbin/ifconfig | grep BROADCAST | awk '{print $1}' | awk -F':' '{print $1}'); do echo $IF; done;",output);
#endif
	hsystemcmd("ifconfig bridge0 create");

	char* iface[2];
	cutFirstWord(output,iface);

	if(strcmp(iface[0],"") != 0)
	{
		addInterface(iface[0]);
		if(strcmp(iface[1],"") != 0)
		{
			char* iface2[2];
			char ifbuffer[1024] = "";
			strcpy(ifbuffer,iface[1]);

			while(strcmp(ifbuffer,"") != 0)
			{
				cutFirstWord(ifbuffer,iface2);
				net_iface* cursor = interfaces;
				if(cursor == NULL)
					addInterface(iface2[0]);
				else
				{
					unsigned short found = 0;
					while(cursor != NULL && found == 0)
					{
						if(strcmp(cursor->name,iface2[0]) != 0)
							cursor = cursor->next;
						else
							found = 1;
					}

					if(found == 0)
						addInterface(iface2[0]);
				}
				strcpy(ifbuffer,iface2[1]);
			}
		}
	}
}

unsigned short saveInterfaces()
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
				char buffer[1024] = "";
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


unsigned short setInterfaceRIPAuthType(char* name, unsigned short type)
{
	if(type > RIP_AUTH_MD5 && type < RIP_AUTH_NONE)
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
	else
	{
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
	}

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
	else
	{
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
	}

	return 0;
}
