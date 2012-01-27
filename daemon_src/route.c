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
#include "route.h"
#include "configuration.h"

void addRoute(char* ip, char* mask, char* gate)
{
	route* newRoute = (route*)malloc(sizeof(route));

	newRoute->net = (char*)malloc(strlen(ip)*sizeof(char));
	strcpy(newRoute->net,ip);
	newRoute->mask = (char*)malloc(strlen(mask)*sizeof(char));
	strcpy(newRoute->mask,mask);
	newRoute->gate = (char*)malloc(strlen(gate)*sizeof(char));
	strcpy(newRoute->gate,gate);
	newRoute->next = NULL;
	newRoute->prev = NULL;

	if(routes == NULL)
	{
		routes = newRoute;
	}
	else
	{
		route* cursor = newRoute;

		while(cursor->next != NULL)
			cursor = cursor->next;

		newRoute->prev = cursor;
		cursor->next = newRoute;
	}
}

void saveSysctl()
{
	FILE* fSysctl = fopen("/etc/sysctl.conf","w+");
	if(iprouting > 0)
	{
		char ipfw[100] = "net.inet.ip.forwarding=1\n";
		char ip6fw[100] = "net.inet6.ip6.forwarding=1\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}
	else
	{
		char ipfw[100] = "net.inet.ip.forwarding=0\n";
		char ip6fw[100] = "net.inet6.ip6.forwarding=0\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}

	if(mcastrouting > 0)
	{
		char ipfw[100] = "net.inet.ip.mforwarding=1\n";
		char ip6fw[100] = "net.inet6.ip6.mforwarding=1\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}
	else
	{
		char ipfw[100] = "net.inet.ip.mforwarding=0\n";
		char ip6fw[100] = "net.inet6.ip6.mforwarding=0\n";
		fwrite(ipfw,1,strlen(ipfw),fSysctl);
		fwrite(ip6fw,1,strlen(ip6fw),fSysctl);
	}

	fclose(fSysctl);
}