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
#include "command_conf_router.h"
#include "configuration.h"
#include "../prompt/prompt_msg.h"
#include "route.h"

cmdCallback crouterCMD_RIP_redistrib(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_REDIST_ERROR();
		return cb;
	}

	if(strcmp(args,"connected") == 0)
	{
		rip_redistrib_conn = 1;
		WRITE_RUN();
	}
	else if(strcmp(args,"static") == 0)
	{
		rip_redistrib_static = 1;
		WRITE_RUN();
	}
	else if(strcmp(args,"default") == 0)
	{
		rip_redistrib_default = 1;
		WRITE_RUN();
	}
	else
		cb.message = CMDROUTER_RIP_REDIST_ERROR();

	return cb;
}

cmdCallback crouterCMD_RIP_noredistrib(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) == 0)
	{
		return cb;
	}

	if(strcmp(args,"connected") == 0)
	{
		rip_redistrib_conn = 0;
		WRITE_RUN();
	}
	else if(strcmp(args,"static") == 0)
	{
		rip_redistrib_static = 0;
		WRITE_RUN();
	}
	else if(strcmp(args,"default") == 0)
	{
		rip_redistrib_default = 0;
		WRITE_RUN();
	}
/*	else
		cb.message = CMDROUTER_RIP_REDIST_ERROR();*/

	return cb;
}

cmdCallback crouterCMD_RIP_defaultinformation(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strcmp(args,"originate") == 0)
	{
		rip_redistrib_default = 1;
		WRITE_RUN();
	}
	else
		cb.message = CMDROUTER_RIP_DEFORIG_ERROR();
	return cb;
}

cmdCallback crouterCMD_RIP_nodefaultinformation(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strcmp(args,"originate") == 0)
	{
		rip_redistrib_default = 0;
		WRITE_RUN();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_splithorizon(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDROUTER_RIP_SPLITHORIZ_ERROR();
	}
	else
	{
		rip_split_horizon = 1;
		WRITE_RUN();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_nosplithorizon(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};

	if(strlen(_none) == 0)
	{
		rip_split_horizon = 0;
		WRITE_RUN();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_passive(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_INTERFACE_ERROR();
		return cb;
	}

	char* iface[2];
	cutFirstWord(args,iface);

	if(strlen(iface[1]) > 0)
	{
		cb.message = CMDROUTER_RIP_INTERFACE_ERROR();
		return cb;
	}

	if(setInterfaceRIPPassive(iface[0],1) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
	}
	else
		WRITE_RUN();

	return cb;
}

cmdCallback crouterCMD_RIP_nopassive(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) == 0)
		return cb;

	char* iface[2];
	cutFirstWord(args,iface);

	if(strlen(iface[1]) > 0)
		return cb;

	if(setInterfaceRIPPassive(iface[0],0) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
	}
	else
		WRITE_RUN();

	return cb;
}

cmdCallback crouterCMD_RIP_timer(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};

	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	char* upd_timer[2];
	cutFirstWord(args,upd_timer);

	if(strlen(upd_timer[1]) == 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int update_t = atoi(upd_timer[0]);

	if(update_t <= 0 || update_t > 600)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	char* fail_timer[2];
	cutFirstWord(upd_timer[1],fail_timer);

	if(strlen(fail_timer[1]) == 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int fail_t = atoi(fail_timer[0]);

	if(fail_t <= 0 || fail_t < 2 * update_t)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	char* dead_timer[2];
	cutFirstWord(fail_timer[1],dead_timer);

	if(strlen(dead_timer[1]) > 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int dead_t = atoi(dead_timer[0]);

	if(dead_t <= 0 || dead_t < fail_t)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	rip_update_timer = update_t;
	rip_fail_timer = fail_t;
	rip_dead_timer = dead_t;

	WRITE_RUN();
	return cb;
}

cmdCallback crouterCMD_RIP_notimer(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) < 1)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	char* upd_timer[2];
	cutFirstWord(args,upd_timer);

	if(strlen(upd_timer[1]) == 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int update_t = atoi(upd_timer[0]);

	char* fail_timer[2];
	cutFirstWord(upd_timer[1],fail_timer);

	if(strlen(fail_timer[1]) == 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int fail_t = atoi(fail_timer[0]);

	char* dead_timer[2];
	cutFirstWord(fail_timer[1],dead_timer);

	if(strlen(dead_timer[1]) > 0)
	{
		cb.message = CMDROUTER_RIP_TIMERS_ERROR();
		return cb;
	}

	int dead_t = atoi(dead_timer[0]);

	if(update_t == rip_update_timer && fail_t == rip_fail_timer && dead_t == rip_dead_timer)
	{
		rip_update_timer = 30;
		rip_fail_timer = 180;
		rip_dead_timer = 240;
		WRITE_RUN();
	}

	return cb;
}

cmdCallback crouterCMD_RIP_cost(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_COST_ERROR();
		return cb;
	}

	char* iface[2];
	cutFirstWord(args,iface);

	if(strlen(iface[1]) == 0)
	{
		cb.message = CMDROUTER_RIP_COST_ERROR();
		return cb;
	}

	char* cost[2];
	cutFirstWord(iface[1],cost);

	if(strlen(cost[1]) > 0)
	{
		cb.message = CMDROUTER_RIP_COST_ERROR();
		return cb;
	}

	int icost = atoi(cost[0]);
	if(icost < 1 || icost > 16)
	{
		cb.message = CMDROUTER_RIP_COST_ERROR();
		return cb;
	}

	if(setInterfaceRIPCost(iface[0],icost) == 0)
	{
		WRITE_RUN();
	}
	else
	{
		cb.message = CMD_INTERFACE_UNK();
	}


	return cb;
}

cmdCallback crouterCMD_RIP_nocost(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};
	if(strlen(args) == 0)
		return cb;

	char* iface[2];
	cutFirstWord(args,iface);

	if(strlen(iface[1]) == 0)
		return cb;

	char* cost[2];
	cutFirstWord(iface[1],cost);

	if(strlen(cost[1]) > 0)
		return cb;

	int icost = atoi(cost[0]);
	if(icost < 1 || icost > 16)
		return cb;

	if(getInterfaceRIPCost(iface[0]) == icost)
	{
		setInterfaceRIPCost(iface[0],1);
		WRITE_RUN();

	}
	else
	{
		cb.message = CMD_INTERFACE_UNK();
	}

	return cb;
}

cmdCallback crouterCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF;

	return cb;
}
