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

#include <stdio.h>
#include "interface.h"
#include "configuration.h"
#include "command.h"

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
	newIface->state = 1;

	if(interfaces == NULL)
	{
		interfaces = newIface;
	}
	else
	{
		net_iface* cursor = interfaces;

		while(cursor->next != NULL)
			cursor = cursor->next;

		newIface->prev = cursor;
		cursor->next = newIface;
	}
}

void loadInterfaces()
{
	char input[1024];
	char output[1024] = "";
#ifdef FREEBSD
	execSystemCommand("for IF in $(/sbin/ifconfig | grep HWaddr | awk '{print $1}'); do echo $IF; done;",output);
#else
	execSystemCommand("for IF in $(/sbin/ifconfig | grep BROADCAST | awk '{print $1}' | awk -F':' '{print $1}'); do echo $IF; done;",output);
#endif
	char* iface[2];
	cutFirstWord(output,iface);
	if(strcmp(iface[0],"") != 0)
	{
		addInterface(iface[0]);
		if(strcmp(iface[1],"") != 0)
		{
			char* iface2[2];
			char ifbuffer[1024];
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
				if(strcmp(direction,"in") == 0)
					cursor->acl_in = aclname;
				else if(strcmp(direction,"out") == 0)
					cursor->acl_out = aclname;
				else
					return 1;
			}
			else
				cursor = cursor->next;
		}

		if(found == 0)
			return 1;
	}

	return 0;
}
