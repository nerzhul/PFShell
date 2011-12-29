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
	strcpy(newIface->ip,"");
	strcpy(newIface->acl_in,"");
	strcpy(newIface->acl_out,"");

	net_iface* cursor = interfaces;
	if(cursor == NULL)
	{
		cursor = newIface;
	}
	else
	{
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
	execSystemCommand(input,output);
	char* iface[2];
	cutFirstWord(output,iface);
	printf("iface1 %s iface2 %s\n",iface[0],iface[1]);
}
