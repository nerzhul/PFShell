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

#include <stdlib.h>
#include "conf_router_rip.h"
#include "configuration.h"
#include "../prompt/prompt_msg.h"
#include "route.h"
#include "../prompt/prompt.h"

cmdCallback crouterCMD_RIP_network(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	char* iface[1];
	uint8_t nbargs = cutString(args,iface);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_RIP_NETWORK_ERROR();
	}
	else
	{
		if(setInterfaceRIPNetwork(iface[0],1) != 0)
			cb.message = CMD_INTERFACE_UNK();
	}

	freeCutString(iface,nbargs);
	return cb;
}
cmdCallback crouterCMD_RIP_nonetwork(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	char* iface[1];
	uint8_t nbargs = cutString(args,iface);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_RIP_NETWORK_ERROR();
	}
	else
	{
		if(setInterfaceRIPNetwork(iface[0],0) != 0)
			cb.message = CMD_INTERFACE_UNK();
	}

	freeCutString(iface,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_redistrib(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}


	char* redistargs[1];
	uint8_t nbargs = cutString(args,redistargs);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	if(strcmp(redistargs[0],"connected") == 0)
		rip_redistrib_conn = 1;
	else if(strcmp(redistargs[0],"static") == 0)
		rip_redistrib_static = 1;
	else if(strcmp(redistargs[0],"default") == 0)
		rip_redistrib_default = 1;
	else
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	WRITE_RUN();
	WRITE_RIPD();

	freeCutString(redistargs,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_noredistrib(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	char* redistargs[1];
	uint8_t nbargs = cutString(args,redistargs);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	if(strcmp(redistargs[0],"connected") == 0)
		rip_redistrib_conn = 0;
	else if(strcmp(redistargs[0],"static") == 0)
		rip_redistrib_static = 0;
	else if(strcmp(redistargs[0],"default") == 0)
		rip_redistrib_default = 0;
	else
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	WRITE_RUN();
	WRITE_RIPD();

	freeCutString(redistargs,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_defaultinformation(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	if(strcmp(args,"originate") == 0)
	{
		rip_redistrib_default = 1;
		WRITE_RUN();
		WRITE_RIPD();
	}
	else
		cb.message = CMDROUTER_RIP_DEFORIG_ERROR();
	return cb;
}

cmdCallback crouterCMD_RIP_nodefaultinformation(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	if(strcmp(args,"originate") == 0)
	{
		rip_redistrib_default = 0;
		WRITE_RUN();
		WRITE_RIPD();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_splithorizon(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDROUTER_RIP_SPLITHORIZ_ERROR();
	}
	else
	{
		rip_split_horizon = 1;
		WRITE_RUN();
		WRITE_RIPD();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_nosplithorizon(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	if(strlen(_none) == 0)
	{
		rip_split_horizon = 0;
		WRITE_RUN();
		WRITE_RIPD();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_passive(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		return cb;
	}

	char* iface[1];
	uint8_t nbargs = cutString(args,iface);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		return cb;
	}

	if(setInterfaceRIPPassive(iface[0],1) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
	}
	else
	{
		WRITE_RUN();
		WRITE_RIPD();
	}

	freeCutString(iface,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_nopassive(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		return cb;
	}

	char* iface[1];
	uint8_t nbargs = cutString(args,iface);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		return cb;
	}

	if(setInterfaceRIPPassive(iface[0],0) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
	}
	else
	{
		WRITE_RUN();
		WRITE_RIPD();
	}

	freeCutString(iface,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_timer(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	char* timers[3];
	uint8_t nbargs = cutString(args,timers);

	if(nbargs != 3)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int update_t = atoi(timers[0]);

	if(update_t <= 0 || update_t > 600)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int fail_t = atoi(timers[1]);

	if(fail_t <= 0 || fail_t < 2 * update_t)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int dead_t = atoi(timers[2]);

	if(dead_t <= 0 || dead_t < fail_t)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	rip_update_timer = update_t;
	rip_fail_timer = fail_t;
	rip_dead_timer = dead_t;

	WRITE_RUN();
	WRITE_RIPD();

	freeCutString(timers,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_notimer(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};
	if(strlen(args) < 1)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	char* timers[3];
	uint8_t nbargs = cutString(args,timers);

	if(nbargs != 3)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int update_t = atoi(timers[0]);
	int fail_t = atoi(timers[1]);
	int dead_t = atoi(timers[2]);

	if(update_t == rip_update_timer && fail_t == rip_fail_timer && dead_t == rip_dead_timer)
	{
		rip_update_timer = 30;
		rip_fail_timer = 180;
		rip_dead_timer = 240;
		WRITE_RUN();
		WRITE_RIPD();
	}

	freeCutString(timers,nbargs);
	return cb;
}

cmdCallback crouterCMD_RIP_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_RIP,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF;

	return cb;
}
