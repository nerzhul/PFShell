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

#ifndef __FIREWALL_H__
#define __FIREWALL_H__

unsigned short firewallState;

typedef struct access_control access_control;

#define TCP 0
#define UDP 1
#define ICMP 2

struct access_control
{
	unsigned short _proto;
	unsigned short _sport;
	unsigned short _dport;
	char* _saddr;
	char* _daddr;
	unsigned short _allow;
	access_control* next;
	access_control* prev;
};

typedef struct acl_struct acl;

struct acl_struct
{
	char* name;
	access_control* ac;
	acl* next;
	acl* prev;
};

unsigned short readACL(char* acl, unsigned short allow, unsigned short remove);

// Add ACL functions
void addAccessList(acl* list, char* name);
void addAccessControl(access_control* ac, unsigned short proto, unsigned short sport, unsigned short dport, char* saddr, char* daddr, unsigned short allow);
void addACL(char* listname, unsigned short proto, unsigned short sport, unsigned short dport, char* saddr, char* daddr, unsigned short allow);

// Remove ACL functions
void removeAccessList(char* name);
void removeAccessControl(acl* _acl,access_control* ac, unsigned short proto, unsigned short sport, unsigned short dport, char* saddr, char* daddr, unsigned short allow);
void removeACL(char* listname, unsigned short proto, unsigned short sport, unsigned short dport, char* saddr, char* daddr, unsigned short allow);

uint8_t writeFirewall(void);

#endif
