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

#ifndef __INTERFACE_H_
#define __INTERFACE_H_

typedef struct net_iface net_iface;

struct net_iface
{
	char* name;
	char* ip;
	char* acl_in;
	char* acl_out;
	char* desc;
	unsigned short state;
	unsigned short rip_passive;
	unsigned short rip_cost;

	net_iface* next;
	net_iface* prev;
};

void addInterface(char* name);
void loadInterfaces();
unsigned short saveInterfaces();

unsigned short setInterfaceIP(char* name, char* ip);
unsigned short setInterfaceState(char* name, unsigned short state);
unsigned short setInterfaceACL(char* name, char* aclname, char* direction);
unsigned short setInterfaceDesc(char* name, char* desc);
unsigned short setInterfaceRIPPassive(char* name, unsigned short passive);
unsigned short setInterfaceRIPCost(char* name, unsigned short cost);

char* getInterfaceIP(char* name);
unsigned short getInterfaceState(char* name);
char* getInterfaceDesc(char* name);
unsigned short getInterfaceRIPPassive(char* name);
unsigned short getInterfaceRIPCost(char* name);

#endif
