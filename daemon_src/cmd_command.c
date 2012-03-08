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
*     * Neither the name of the CNRS nor the
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
#include "cmd_command.h"
#include "cmd_user.h"
#include "cmd_enable.h"
#include "cmd_conf.h"
#include "cmd_conf_acl.h"
#include "cmd_conf_if.h"
#include "cmd_conf_fw.h"
#include "cmd_conf_rd.h"
#include "cmd_conf_router_rip.h"
#include "cmd_conf_router_ospf.h"
#include "string_mgmt.h"
#include "prompt_msg.h"
#include "prompt.h"

cmdHdlr userCmd[MAX_USER_CMD] =
{
	{"exit",		&uCMD_exit,		NULL,				"\t\t\t\t\texit the shell"},
	{"enable",		&uCMD_enable,		NULL,				"\t\t\t\t\tgo into privileged mode"},
	{"help",		&uCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"},
	{"show",		&uCMD_show,		NULL,				"\t\t\t\t\tshow some informations"}
};

cmdHdlr enableCmd[MAX_ENABLE_CMD] =
{
	{"configure",		&eCMD_configure,	NULL,				"\t\t\t\tgo to configure prompt"},
	{"exit",		&eCMD_exit,		NULL,				"\t\t\t\t\texit the privileged mode"},
	{"help",		&eCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"},
	{"save",		&eCMD_save,		NULL,				"\t\t\t\t\tsave running configuration for later restart"},
	{"show",		eCMD_show,		NULL,				"\t\t\t\t\tshow some informations"}
};

cmdHdlr confCmd[MAX_CONF_CMD] =
{
	{"exit",		&eCMD_exit,		NULL,				"\t\t\t\t\treturn back to privileged mode"},
	{"firewall",		&cCMD_firewall,		NULL,				"\t\t\t\tconfigure the firewall"},
	{"help",		&cCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"},
	{"hostname",		&cCMD_hostname,		&cCMD_nohostname,		"\t\t\t\tchange router hostname"},
	{"interface",		&cCMD_interface,	&cCMD_nointerface,		"\t\t\t\tconfigure some (sub-)interface"},
	{"ip",			&cCMD_ip,		&cCMD_noip,			"\t\t\t\t\tsome ipv4 modifications"},
	{"router",		&cCMD_router,		NULL,				"\t\t\t\t\tdynamic router prompts"}
};

cmdHdlr confIfCmd[MAX_CONF_IF_CMD] =
{
	{"description",		&cifCMD_description,	&cifCMD_nodescription,		"\t\t\t\tchange the description of the interface"},
	{"duplex",		&cifCMD_duplex,		&cifCMD_noduplex,		"\t\t\t\tmodify interface transmission mode"},
	{"encapsulation",	&cifCMD_encap,		&cifCMD_noencap,		"\t\t\t\t{in sub-interface} do dot1q encapsulation for outgoing packets"},
	{"exit",		&cifCMD_exit,		NULL,				"\t\t\t\t\treturn back to configuration mode"},
	{"help",		&cifCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"},
	{"ip",			&cifCMD_ip,		&cifCMD_noip,			"\t\t\t\t\tsome ipv4 modifications"},
	{"mac-address",		&cifCMD_mac,		&cifCMD_nomac,			"\t\t\t\tmodify interface mac address (hw address)"},
	{"shutdown",		&cifCMD_shutdown,	&cifCMD_noshutdown,		"\t\t\t\tadmistratively shutdown the interface"},
	{"speed",		&cifCMD_speed,		&cifCMD_nospeed,		"\t\t\t\tmodify interface speed"},
};

cmdHdlr confFWCmd[MAX_CONF_FW_CMD] =
{
	{"access-list",		&cfwCMD_acl,		&cfwCMD_noacl,			"\t\t\t\tgo to access-list prompt"},
	{"default",		&cfwCMD_default,	NULL,				"\t\t\t\t\tmanage default firewall parameters"},
	{"disable",		&cfwCMD_disable,	&cfwCMD_enable,			"\t\t\t\t\tdisable the firewall"},
	{"enable",		&cfwCMD_enable,		&cfwCMD_disable,		"\t\t\t\t\tenable the firewall"},
	{"exit",		&cfwCMD_exit,		NULL,				"\t\t\t\t\treturn back to the configuration mode"},
	{"help",		&cfwCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"}
};

cmdHdlr confACLCmd[MAX_CONF_ACL_CMD] =
{
	{"allow",		&caclCMD_allow_acl,	&caclCMD_noallow_acl,		"\t\t\t\t\tallow some traffic"},
	{"deny",		&caclCMD_deny_acl,	&caclCMD_nodeny_acl,		"\t\t\t\t\tdrop some traffic"},
	{"exit",		&caclCMD_exit,		NULL,				"\t\t\t\t\treturn back to the firewall mode"},
	{"help",		&caclCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"}
};

cmdHdlr confRDCmd[MAX_CONF_RD_CMD] =
{
	{"exit",		&crdCMD_exit,		NULL,				"\t\t\t\t\treturn back to the configuration mode"},
	{"help",		&crdCMD_help,		NULL,				"\t\t\t\t\tdisplay this help"},
};

cmdHdlr confRouterRIPCmd[MAX_CONF_ROUTER_RIP_CMD] =
{
	{"default-information",	&crouterCMD_RIP_defaultinformation,	&crouterCMD_RIP_nodefaultinformation,	"\t\t\tredistribute default route"},
	{"exit",		&crouterCMD_RIP_exit,			NULL,					"\t\t\t\t\treturn back to the configuration mode"},
	{"help",		&crouterCMD_RIP_help,			NULL,					"\t\t\t\t\tdisplay this help"},
	{"network",		&crouterCMD_RIP_network,		&crouterCMD_RIP_nonetwork,		"\t\t\t\t\tset RIP diffusion network"},
	{"passive-interface",	&crouterCMD_RIP_passive,		&crouterCMD_RIP_nopassive,		"\t\t\tset non diffusing interface"},
	{"redistribute",	&crouterCMD_RIP_redistrib,		&crouterCMD_RIP_noredistrib,		"\t\t\t\tredistribute some routes"},
	{"split-horizon",	&crouterCMD_RIP_splithorizon,		&crouterCMD_RIP_nosplithorizon,		"\t\t\t\tmanage split-horizon"},
	{"timers",		&crouterCMD_RIP_timer,			crouterCMD_RIP_notimer,			"\t\t\t\t\tmanage RIP timers"}
};

cmdHdlr confRouterOSPFCmd[MAX_CONF_ROUTER_OSPF_CMD] =
{
	{"area",		&crouterCMD_OSPF_area,		&crouterCMD_OSPF_noarea,		"\t\t\t\t\tmodify an OSPF area"},
	{"exit",		&crouterCMD_OSPF_exit,		NULL,					"\t\t\t\t\treturn back to the configuration mode"},
	{"help",		&crouterCMD_OSPF_help,		NULL,					"\t\t\t\t\tdisplay this help"},
	{"network",		&crouterCMD_OSPF_network,	&crouterCMD_OSPF_nonetwork,		"\t\t\t\t\tset OSPF diffusion network and area"},
	{"passive-interface",	&crouterCMD_OSPF_passive,	&crouterCMD_OSPF_nopassive,		"\t\t\tset non diffusing interface"},
	{"redistribute",	&crouterCMD_OSPF_redistrib,	&crouterCMD_OSPF_noredistrib,		"\t\t\t\tredistribute some routes"},
	{"router-id",		&crouterCMD_OSPF_routerid,	&crouterCMD_OSPF_norouterid,		"\t\t\t\tmodify OSPF router id"},
	{"stub",		&crouterCMD_OSPF_stub,		&crouterCMD_OSPF_nostub,		"\t\t\t\t\tdeclare router as stub"},
	{"timers",		&crouterCMD_OSPF_timers,	&crouterCMD_OSPF_notimers,		"\t\t\t\t\tmodify OSPF timers"}
};

uint8_t initCmds()
{
	masterCmd[0] = userCmd;
	masterCmd[1] = enableCmd;
	masterCmd[2] = confCmd;
	masterCmd[3] = confIfCmd;
	masterCmd[4] = confFWCmd;
	masterCmd[5] = confRDCmd;
	masterCmd[6] = confACLCmd;
	masterCmd[7] = confRouterRIPCmd;
	masterCmd[8] = confRouterOSPFCmd;

	return 0;
}

cmdCallback handleCmd(char* _fullcmd, unsigned short promptMode)
{
	unsigned int i;
	i = 0;
	char* cmd[2];

	// For load configuration mode
	if(strcmp(_fullcmd,"!") == 0)
		_fullcmd = "exit";

	cutFirstWord(_fullcmd,cmd);

	if(strcmp(cmd[0],"no") == 0)
	{
		char* nocmd[2];
		cutFirstWord(cmd[1],nocmd);
		while(i < MAX_CMDS[promptMode])
		{
			if(strcmp(nocmd[0],masterCmd[promptMode][i].name) == 0 && masterCmd[promptMode][i].invhandler != NULL)
			{
				return (*masterCmd[promptMode][i].invhandler)(nocmd[1]);
			}
			++i;
		}
	}
	else
	{
		while(i < MAX_CMDS[promptMode])
		{
			if(strcmp(cmd[0],masterCmd[promptMode][i].name) == 0 && masterCmd[promptMode][i].handler != NULL)
			{
				return (*masterCmd[promptMode][i].handler)(cmd[1]);
			}
			++i;
		}
	}

	cmdCallback cb = {promptMode,""};
	if(strlen(_fullcmd) > 0)
		cb.message = printError("Command Unknown !\n");

	return cb;
}

void hsystemcmd(char* cmd)
{
	char cmd2[1024] = "";
	strcpy(cmd2,cmd);
	strcat(cmd2," > /dev/null 2>&1");
	system(cmd2);
}
