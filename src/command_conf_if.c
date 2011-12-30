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

#include <stdlib.h>
#include "command.h"
#include "command_conf_if.h"
#include "configuration.h"
#include "interface.h"
#include "prompt.h"
#include "iputils.h"

void cifCMD_exit(char* _none)
{
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   exit\n");
	else
	{
		promptMode = PROMPT_CONF;
		current_iface = "";
	}
}

void cifCMD_ip(char* args)
{
	char* iface[2];
	cutFirstWord(args,iface);
	if(strcmp(iface[0],"address") == 0)
		cifCMD_ip_address(iface[1]);
	else
		CMDIF_IP_ERROR();
}

void cifCMD_ip_address(char* args)
{
	if(strlen(args) < 4)
	{
		CMDIF_IPADDR_ERROR();
		return;

	}

	if(strcmp(args,"DHCP") == 0)
	{
		char buffer[1024];
		strcpy(buffer,"dhclient ");
		strcat(buffer,current_iface);
		system(buffer);
		if(setInterfaceIP(current_iface,"DHCP") != 0)
		{
			CMDIF_FATAL_ERROR();
			return;
		}
	}
	else
	{
		char* ipmask[2];
		cutFirstWord(args,ipmask);
		if(strlen(ipmask[1]) > 1)
		{
			if(regexp(ipmask[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
			{
				if(is_valid_mask(ipmask[1]) == 0)
				{
					char buffer[1024];
					char ipbuffer[100];
					strcpy(buffer,"ifconfig ");
					strcat(buffer,current_iface);
					strcat(buffer," ");

					strcpy(ipbuffer,ipmask[0]);
					strcat(ipbuffer," ");
					strcat(ipbuffer,ipmask[1]);

					strcat(buffer,ipbuffer);

					system(buffer);

					if(setInterfaceIP(current_iface,ipbuffer) != 0)
					{
						CMDIF_FATAL_ERROR();
						return;
					}

				}
				else
				{
					CMDIF_IPADDR_ERROR();
					return;
				}
			}
			else
			{
				CMDIF_IPADDR_ERROR();
				return;
			}
		}
		else if(regexp(ipmask[0],"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])((/([0-9]|[1-2][0-9]|3[0-2]))?)$") == 0)
		{
			char buffer[1024];
			strcpy(buffer,"ifconfig ");
			strcat(buffer,current_iface);
			strcat(buffer," ");
			strcat(buffer,ipmask[0]);
			system(buffer);

			if(setInterfaceIP(current_iface,ipmask[0]) != 0)
			{
				CMDIF_FATAL_ERROR();
				return;
			}
		}
		else
		{
			CMDIF_IPADDR_ERROR();
			return;
		}
	}
	WRITE_RUN();
}

void cifCMD_noip_address(char* args)
{
	// @ TODO
}

void cifCMD_shutdown(char* _none)
{
	if(strlen(_none) > 0)
	{
		CMDIF_SHUTDOWN_ERROR();
		return;
	}

	char buffer[1024];
	strcpy(buffer,"ifconfig ");
	strcat(buffer,current_iface);
	strcat(buffer," down");
	system(buffer);
}

void cifCMD_noshutdown(char* _none)
{
	if(strlen(_none) > 0)
	{
		CMDIF_NOSHUTDOWN_ERROR();
		return;
	}

	char buffer[1024];
	strcpy(buffer,"ifconfig ");
	strcat(buffer,current_iface);
	strcat(buffer," up");
	system(buffer);
}

void cifCMD_access_list(char* args)
{
	if(strlen(args) < 3)
	{
		CMDIF_ACCESS_LIST_ERROR();
		return;
	}
}