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

#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include <sys/types.h>

typedef struct net_iface net_iface;

struct net_iface
{
	char* name;
	char* ip;
	char* acl_in;
	char* acl_out;
	char* desc;
	uint16_t vlan;
	unsigned short state;
	char* ip_helper_list;
	uint8_t is_rip_network;
	unsigned short rip_passive;
	unsigned short rip_cost;
	unsigned short rip_auth_type;
	char* rip_auth_pwd;
	uint32_t ospf_area_id;
	unsigned short ospf_passive;
	unsigned short ospf_cost;
	unsigned short ospf_priority;
	unsigned short ospf_hello_int;
	unsigned short ospf_dead_int;
	unsigned short ospf_transmit_delay;
	unsigned short ospf_retransmit_delay;
	unsigned short ospf_auth_type;
	char* ospf_auth_pwd;

	net_iface* next;
	net_iface* prev;
};

void addInterface(char* name);
void delInterface(char* name);
void loadInterfaces(void);
unsigned short saveInterfaces(void);

uint8_t is_interface(char* name);

int8_t getInterfacePosition(char* name);

unsigned short setInterfaceIP(char* name, char* ip);
unsigned short setInterfaceVLAN(char* name, uint16_t vlan);
unsigned short setInterfaceState(char* name, unsigned short state);
unsigned short setInterfaceACL(char* name, char* aclname, char* direction);
unsigned short setInterfaceDesc(char* name, char* desc);

// Ip Helpers
uint8_t addInterfaceIPHelper(char* name, char* ip);
void delInterfaceIPHelper(char* name, char* ip);
char* getInterfaceIPHelpers(char* name);
void launchInterfaceIPHelpers(char* name);

// RIP
unsigned short setInterfaceRIPPassive(char* name, unsigned short passive);
unsigned short setInterfaceRIPCost(char* name, unsigned short cost);
unsigned short setInterfaceRIPAuthType(char* name, unsigned short type);
unsigned short setInterfaceRIPAuthKey(char* name, char* key);
unsigned short setInterfaceRIPNetwork(char* name, uint8_t net);

// OSPF
unsigned short setInterfaceOSPFPassive(char* name, unsigned short passive);
unsigned short setInterfaceOSPFCost(char* name, unsigned short cost);
unsigned short setInterfaceOSPFPriority(char* name, uint8_t prio);
unsigned short setInterfaceOSPFDead(char* name, unsigned int dead);
unsigned short setInterfaceOSPFHello(char* name, unsigned short hello);
unsigned short setInterfaceOSPFTransmit(char* name, unsigned short tr);
unsigned short setInterfaceOSPFRetransmit(char* name, unsigned short retr);
unsigned short setInterfaceOSPFAuthType(char* name, unsigned short type);
unsigned short setInterfaceOSPFAuthKey(char* name, char* key);

char* getInterfaceIP(char* name);
uint16_t getInterfaceVLAN(char* name);
unsigned short getInterfaceState(char* name);
char* getInterfaceDesc(char* name);

// RIP
unsigned short getInterfaceRIPPassive(char* name);
unsigned short getInterfaceRIPCost(char* name);
unsigned short getInterfaceRIPAuthType(char* name);
char* getInterfaceRIPAuthKey(char* name);
uint8_t getInterfaceRIPNetwork(char* name);

// OSPF
unsigned short getInterfaceOSPFPassive(char* name);
unsigned short getInterfaceOSPFCost(char* name);
unsigned short getInterfaceOSPFPriority(char* name);
unsigned int getInterfaceOSPFDead(char* name);
unsigned short getInterfaceOSPFHello(char* name);
unsigned short getInterfaceOSPFTransmit(char* name);
unsigned short getInterfaceOSPFRetransmit(char* name);
unsigned short getInterfaceOSPFAuthType(char* name);
char* getInterfaceOSPFAuthKey(char* name);


#endif
