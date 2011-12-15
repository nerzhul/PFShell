/*
* Copyright (c) 2011, Frost Sapphire Studios
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

#include "firewall.h"
#include "configuration.h"

void addAccessList(acl* list, char* name)
{
	acl* newACL = malloc(sizeof(acl));
	newACL->name = name;
	newACL->next = NULL;

	if(list == NULL)
	{
		newACL->prev = NULL;
		list = newACL;
	}
	else
	{
		acl* cursor = list;
		while(cursor->next != NULL)
			cursor = cursor->next;

		newACL->prev = list;
		list->next = newACL;
	}
}

void addAccessControl(access_control* ac, unsigned int direction, unsigned int proto, int sport, int dport, char* saddr, char* daddr, unsigned short allow)
{
	access_control* newAC = malloc(sizeof(access_control));
	newAC->direction = direction;
	newAC->proto = proto;
	newAC->_sport = sport;
	newAC->_dport = dport;
	newAC->_saddr = saddr;
	newAC->_daddr = daddr;
	newAC->_allow = allow;

	if(ac == NULL)
	{
		newAC->prev = NULL;
		ac = newAC;
	}
	else
	{
		access_control* cursor = ac;
		while(cursor->next != NULL)
			cursor = cursor->next;

		newAC->prev = ac;
		ac->next = newAC;
	}
}

void addACL(char* listname, unsigned int direction, unsigned int proto, int sport, int dport, char* saddr, char* daddr, unsigned short allow)
{
	acl* cursor = access_lists;
	if(strcmp(cursor->name,listname) == 0)
	{
		addAccessControl(cursor->ac, direction, proto, sport, dport, saddr, daddr, allow);
	}
	else
	{
		unsigned short found = 0;
		while(found == 0 && cursor->next != NULL)
		{
			cursor = cursor->next;
			if(strcmp(cursor->name,listname) == 0)
			{
				addAccessControl(cursor->ac, direction, proto, sport, dport, saddr, daddr, allow);
				found = 1;
			}
		}

		if(found == 0)
		{
			addAccessList(access_lists, listname);
			addACL(listname, direction, proto, sport, dport, saddr, daddr, allow);
		}
	}
}