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
	newIface->real_mac_addr = "";
	newIface->mac_addr = "";
	newIface->acl_in = "";
	newIface->acl_out = "";
	newIface->desc = "";
	newIface->vlan = 0;
	newIface->speed = "";
	newIface->duplex = DUPLEX_AUTO;
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
	char output[1024];
	bzero(output,1024);
	execSystemCommand("for IF in $(/sbin/ifconfig | grep BROADCAST | awk '{print $1}' | awk -F':' '{print $1}'); do echo $IF; done;",output);
	if(strcmp(output,"") == 0)
		return;

	char* iface[128];
	uint8_t nbif = cutString(output,iface,128);

	uint8_t i;
	for(i=0;i<nbif;i++)
	{
		if((strlen(iface[i]) < 2) || (strlen(iface[i]) > 4 && iface[i][0] == 'v' && iface[i][1] == 'l' && iface[i][2] == 'a' && iface[i][3] == 'n'))
			continue;

		if(getInterfacePosition(iface[i]) == -1)
			addInterface(iface[i]);

		char buffer[1024];
		bzero(buffer,1024);
		sprintf(buffer,"/sbin/ifconfig %s | grep lladdr | awk '{print $2}' | tr -d '\n'",iface[i]);
		bzero(output,1024);

		execSystemCommand(buffer,output);
		if(is_valid_macaddr(output) == 0)
			setInterfaceRealMAC(iface[i],output);
	}

	freeCutString(iface,nbif);
}

uint8_t saveInterfaces(void)
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

uint8_t setInterfaceIP(char* name, char* ip)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->ip != NULL && strlen(cursor->ip) > 0)
				free(cursor->ip);
			cursor->ip = (char*)malloc(strlen(ip)*sizeof(char));
			strcpy(cursor->ip,ip);
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceMAC(char* name, char* mac)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->mac_addr != NULL && strlen(cursor->mac_addr) > 0)
				free(cursor->mac_addr);

			cursor->mac_addr = (char*)malloc(strlen(mac)*sizeof(char));
			strcpy(cursor->mac_addr,mac);
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceRealMAC(char* name, char* mac)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->real_mac_addr != NULL && strlen(cursor->real_mac_addr) > 0)
				free(cursor->real_mac_addr);

			cursor->real_mac_addr = (char*)malloc(strlen(mac)*sizeof(char));
			strcpy(cursor->real_mac_addr,mac);
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

int8_t getInterfacePosition(char* name)
{
	int8_t pos = 0;

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

	return -1;
}

char* getInterfaceIP(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			return cursor->ip;
		}
		else
			cursor = cursor->next;
	}

	return "";
}

char* getInterfaceMAC(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			return cursor->mac_addr;
		}
		else
			cursor = cursor->next;
	}

	return "";
}

char* getInterfaceRealMAC(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->real_mac_addr;
		else
			cursor = cursor->next;
	}

	return "";
}


uint8_t setInterfaceDesc(char* name, char* desc)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->desc != NULL && strlen(cursor->desc) > 0)
				free(cursor->desc);

			cursor->desc = (char*)malloc(strlen(desc)*sizeof(char));
			strcpy(cursor->desc,desc);
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

char* getInterfaceDesc(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->desc;
		else
			cursor = cursor->next;
	}

	return "";
}

uint8_t setInterfaceRIPNetwork(char* name, uint8_t network)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->is_rip_network = network;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

char* getInterfaceSpeed(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->speed;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t getInterfaceRIPNetwork(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->is_rip_network;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceVLAN(char* name, uint16_t vlan)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->vlan = vlan;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceSpeed(char* name, char* speed)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->speed != NULL && strlen(cursor->speed) > 0)
				free(cursor->speed);

			cursor->speed = (char*)malloc((strlen(speed)+1)*sizeof(char));
			strcpy(cursor->speed,speed);
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceState(char* name, uint8_t state)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->state = state;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceDuplex(char* name, uint8_t duplex)
{
	if(duplex > DUPLEX_FULL)
		return 2;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->duplex = duplex;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint16_t getInterfaceVLAN(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->vlan;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t getInterfaceState(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->state;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t getInterfaceDuplex(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->duplex;
		else
			cursor = cursor->duplex;
	}

	return -1;
}

uint8_t setInterfaceOSPFCost(char* name, unsigned short cost)
{
	if(cost > 16 && cost < 1)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_cost = cost;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFCost(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_cost;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFPriority(char* name, uint8_t prio)
{
	/*if(prio > 255 && prio < 0) // Due to variable type
		return 1;*/

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_priority = prio;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFPriority(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_priority;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFHello(char* name, unsigned short hello)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_hello_int = hello;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFHello(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_hello_int;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFDead(char* name, unsigned int dead)
{
	if(dead > 2147483647 && dead < 2)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_dead_int = dead;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned int getInterfaceOSPFDead(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_dead_int;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFTransmit(char* name, unsigned short transmit)
{
	if(transmit > 3600 && transmit < 1)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_transmit_delay = transmit;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFTransmit(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_transmit_delay;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFRetransmit(char* name, unsigned short transmit)
{
	if(transmit > 3600 && transmit < 5)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_retransmit_delay = transmit;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFRetransmit(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_retransmit_delay;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceRIPCost(char* name, unsigned short cost)
{
	if(cost > 16 && cost < 1)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->rip_cost = cost;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceRIPCost(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->rip_cost;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFAuthKey(char* name, char* key)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->ospf_auth_pwd != NULL && strlen(cursor->ospf_auth_pwd) > 0)
				free(cursor->ospf_auth_pwd);

			cursor->ospf_auth_pwd = (char*)malloc(strlen(key)*sizeof(char));
			strcpy(cursor->ospf_auth_pwd,key);
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceRIPAuthKey(char* name, char* key)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(cursor->rip_auth_pwd != NULL && strlen(cursor->rip_auth_pwd) > 0)
				free(cursor->rip_auth_pwd);

			cursor->rip_auth_pwd = (char*)malloc(strlen(key)*sizeof(char));
			strcpy(cursor->rip_auth_pwd,key);
			return 0;

		}
		else
			cursor = cursor->next;
	}

	return 1;
}

char* getInterfaceOSPFAuthKey(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_auth_pwd;
		else
			cursor = cursor->next;
	}

	return "";
}

char* getInterfaceRIPAuthKey(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->rip_auth_pwd;
		else
			cursor = cursor->next;
	}

	return "";
}

uint8_t setInterfaceOSPFAuthType(char* name, unsigned short type)
{
	if(type > RIP_AUTH_MD5)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_auth_type = type;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

uint8_t setInterfaceRIPAuthType(char* name, unsigned short type)
{
	if(type > RIP_AUTH_MD5)
		return 1;

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->rip_auth_type = type;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFAuthType(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_auth_type;
		else
			cursor = cursor->next;
	}

	return -1;
}

unsigned short getInterfaceRIPAuthType(char* name)
{

	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->rip_auth_type;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceRIPPassive(char* name, unsigned short passive)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->rip_passive = passive;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceRIPPassive(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->rip_passive;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceOSPFPassive(char* name, unsigned short passive)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			cursor->ospf_passive = passive;
			return 0;
		}
		else
			cursor = cursor->next;
	}

	return 1;
}

unsigned short getInterfaceOSPFPassive(char* name)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
			return cursor->ospf_passive;
		else
			cursor = cursor->next;
	}

	return -1;
}

uint8_t setInterfaceACL(char* name, char* aclname, char* direction)
{
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			if(strlen(aclname) != 0)
			{
				acl* cursor2 = access_lists;

				while(cursor2 != NULL)
				{
					if(strcmp(cursor2->name,aclname) == 0)
					{
						if(strcmp(direction,"in") == 0)
						{
							cursor->acl_in = aclname;
							return 0;
						}
						else if(strcmp(direction,"out") == 0)
						{
							cursor->acl_out = aclname;
							return 0;
						}
						else
							return 1;
					}
					else
						cursor2 = cursor2->next;
				}
			}
			else
			{
				if(strcmp(direction,"in") == 0)
				{
					cursor->acl_in = "";
					return 0;
				}
				else if(strcmp(direction,"out") == 0)
				{
					cursor->acl_out = "";
					return 0;
				}
			}
		}
		else
			cursor = cursor->next;
	}

	return 1;
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
			uint8_t nbhelpers = cutString(cursor->ip_helper_list,helpers,64);

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
			freeCutString(helpers,nbhelpers);
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
			uint8_t nbhelpers = cutString(cursor->ip_helper_list,helpers,64);

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
			freeCutString(helpers,nbhelpers);
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
	net_iface* cursor = interfaces;

	while(cursor != NULL)
	{
		if(strcmp(cursor->name,name) == 0)
		{
			char* helpers[64];
			uint8_t nbhelpers = cutString(cursor->ip_helper_list,helpers,64);

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
					hsystemcmd(buffer);
				}
			}
			freeCutString(helpers,nbhelpers);
			freeCutString(subiface,strlen(subiface[1]) > 0 ? 2 : 1);
			return;
		}
		else
			cursor = cursor->next;
	}
}
