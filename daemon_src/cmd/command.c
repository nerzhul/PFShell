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
#include "user.h"
#include "enable.h"
#include "conf.h"
#include "conf_acl.h"
#include "conf_if.h"
#include "conf_fw.h"
#include "conf_rd.h"
#include "conf_router_rip.h"
#include "conf_router_ospf.h"
#include "../string_mgmt.h"
#include "../prompt/prompt_msg.h"
#include "../prompt/prompt.h"

unsigned short initCmds()
{
	// User Mode Commands
	userCmd[0].name = "exit";
	userCmd[0].handler = &uCMD_exit;
	userCmd[1].name = "enable";
	userCmd[1].handler = &uCMD_enable;
	userCmd[2].name = "help";
	userCmd[2].handler = &uCMD_help;
	userCmd[3].name = "show";
	userCmd[3].handler = &uCMD_show;

	// Enable Mode Commands
	enableCmd[0].name = "exit";
	enableCmd[0].handler = &eCMD_exit;
	enableCmd[1].name = "configure";
	enableCmd[1].handler = &eCMD_configure;
	enableCmd[2].name = "save";
	enableCmd[2].handler = &eCMD_save;
	enableCmd[3].name = "show";
	enableCmd[3].handler = &eCMD_show;

	// Enable - Configure Commands
	confCmd[0].name = "exit";
	confCmd[0].handler = &cCMD_exit;
	confCmd[1].name = "firewall";
	confCmd[1].handler = &cCMD_firewall;
	confCmd[2].name = "hostname";
	confCmd[2].handler = &cCMD_hostname;
	confCmd[3].name = "interface";
	confCmd[3].handler = &cCMD_interface;
	confCmd[4].name = "ip";
	confCmd[4].handler = &cCMD_ip;
	confCmd[5].name = "router";
	confCmd[5].handler = &cCMD_router;

	// Enable - Configure Inverted Commands
	noconfCmd[0].name = "hostname";
	noconfCmd[0].handler = &cCMD_nohostname;
	noconfCmd[1].name = "ip";
	noconfCmd[1].handler = &cCMD_noip;
	noconfCmd[2].name = "interface";
	noconfCmd[2].handler = &cCMD_nointerface;

	// Enable - Configure - Interface Commands
	confIfCmd[0].name = "exit";
	confIfCmd[0].handler = &cifCMD_exit;
	confIfCmd[1].name = "shutdown";
	confIfCmd[1].handler = &cifCMD_shutdown;
	confIfCmd[2].name = "ip";
	confIfCmd[2].handler = &cifCMD_ip;
	confIfCmd[3].name = "description";
	confIfCmd[3].handler = &cifCMD_description;
	confIfCmd[4].name = "encapsulation";
	confIfCmd[4].handler = &cifCMD_encap;

	// Enable - Configure - Interface Inverted commands
	noconfIfCmd[0].name = "shutdown";
	noconfIfCmd[0].handler = &cifCMD_noshutdown;
	noconfIfCmd[1].name = "ip";
	noconfIfCmd[1].handler = &cifCMD_noip;
	noconfIfCmd[2].name = "description";
	noconfIfCmd[2].handler = &cifCMD_nodescription;
	noconfIfCmd[3].name = "encapsulation";
	noconfIfCmd[3].handler = &cifCMD_noencap;

	// Enable - Configure - Firewall Commands
	confFWCmd[0].name = "exit";
	confFWCmd[0].handler = &cfwCMD_exit;
	confFWCmd[1].name = "default";
	confFWCmd[1].handler = &cfwCMD_default;
	confFWCmd[2].name = "enable";
	confFWCmd[2].handler = &cfwCMD_enable;
	confFWCmd[3].name = "disable";
	confFWCmd[3].handler = &cfwCMD_disable;
	confFWCmd[4].name = "access-list";
	confFWCmd[4].handler = &cfwCMD_acl;

	// Enable - Configure - Firewall Inverted Commands
	noconfFWCmd[0].name = "access-list";
	noconfFWCmd[0].handler = &cfwCMD_noacl;

	// Enable - Configure - Firewall - ACL Commands
	confACLCmd[0].name = "allow";
	confACLCmd[0].handler = &caclCMD_allow_acl;
	confACLCmd[1].name = "deny";
	confACLCmd[1].handler = &caclCMD_deny_acl;
	confACLCmd[2].name = "exit";
	confACLCmd[2].handler = &caclCMD_exit;

	// Enable - Configure - Firewall No ACL Commands
	noconfACLCmd[0].name = "allow";
	noconfACLCmd[0].handler = &caclCMD_noallow_acl;
	noconfACLCmd[1].name = "deny";
	noconfACLCmd[1].handler = &caclCMD_nodeny_acl;

	// Enable - Configure - Redundancy Commands
	confRDCmd[0].name = "exit";
	confRDCmd[0].handler = &crdCMD_exit;

	// Enable - Configure - Router RIP Commands
	confRouterRIPCmd[0].name = "exit";
	confRouterRIPCmd[0].handler = &crouterCMD_RIP_exit;
	confRouterRIPCmd[1].name = "redistribute";
	confRouterRIPCmd[1].handler = crouterCMD_RIP_redistrib;
	confRouterRIPCmd[2].name = "default-information";
	confRouterRIPCmd[2].handler = crouterCMD_RIP_defaultinformation;
	confRouterRIPCmd[3].name = "split-horizon";
	confRouterRIPCmd[3].handler = crouterCMD_RIP_splithorizon;
	confRouterRIPCmd[4].name = "passive-interface";
	confRouterRIPCmd[4].handler = crouterCMD_RIP_passive;
	confRouterRIPCmd[5].name = "timers";
	confRouterRIPCmd[5].handler = crouterCMD_RIP_timer;
	confRouterRIPCmd[6].name = "network";
	confRouterRIPCmd[6].handler = crouterCMD_RIP_network;

	// Enable - Configure - Router RIP No Commands
	noconfRouterRIPCmd[0].name = "redistribute";
	noconfRouterRIPCmd[0].handler = crouterCMD_RIP_noredistrib;
	noconfRouterRIPCmd[1].name = "default-information";
	noconfRouterRIPCmd[1].handler = crouterCMD_RIP_nodefaultinformation;
	noconfRouterRIPCmd[2].name = "split-horizon";
	noconfRouterRIPCmd[2].handler = crouterCMD_RIP_nosplithorizon;
	noconfRouterRIPCmd[3].name = "passive-interface";
	noconfRouterRIPCmd[3].handler = crouterCMD_RIP_nopassive;
	noconfRouterRIPCmd[4].name = "timers";
	noconfRouterRIPCmd[4].handler = crouterCMD_RIP_notimer;
	noconfRouterRIPCmd[5].name = "network";
	noconfRouterRIPCmd[5].handler = crouterCMD_RIP_nonetwork;

	// Enable - Configure - Router OSPF Commands
	confRouterOSPFCmd[0].name = "exit";
	confRouterOSPFCmd[0].handler = &crouterCMD_OSPF_exit;
	confRouterOSPFCmd[1].name = "redistribute";
	confRouterOSPFCmd[1].handler = crouterCMD_OSPF_redistrib;
	confRouterOSPFCmd[2].name = "passive-interface";
	confRouterOSPFCmd[2].handler = crouterCMD_OSPF_passive;
	confRouterOSPFCmd[3].name = "router-id";
	confRouterOSPFCmd[3].handler = crouterCMD_OSPF_routerid;
	confRouterOSPFCmd[4].name = "timers";
	confRouterOSPFCmd[4].handler = crouterCMD_OSPF_timers;
	confRouterOSPFCmd[5].name = "network";
	confRouterOSPFCmd[5].handler = crouterCMD_OSPF_network;
	confRouterOSPFCmd[6].name = "area";
	confRouterOSPFCmd[6].handler = crouterCMD_OSPF_area;

	// Enable - Configure - Router OSPF No Commands
	noconfRouterOSPFCmd[0].name = "redistribute";
	noconfRouterOSPFCmd[0].handler = crouterCMD_OSPF_noredistrib;
	noconfRouterOSPFCmd[1].name = "passive-interface";
	noconfRouterOSPFCmd[1].handler = crouterCMD_OSPF_nopassive;
	noconfRouterOSPFCmd[2].name = "router-id";
	noconfRouterOSPFCmd[2].handler = crouterCMD_OSPF_norouterid;
	noconfRouterOSPFCmd[3].name = "timers";
	noconfRouterOSPFCmd[3].handler = crouterCMD_OSPF_notimers;
	noconfRouterOSPFCmd[4].name = "network";
	noconfRouterOSPFCmd[4].handler = crouterCMD_OSPF_nonetwork;
	noconfRouterOSPFCmd[5].name = "area";
	noconfRouterOSPFCmd[5].handler = crouterCMD_OSPF_noarea;

	masterCmd[0] = userCmd;
	masterCmd[1] = enableCmd;
	masterCmd[2] = confCmd;
	masterCmd[3] = confIfCmd;
	masterCmd[4] = confFWCmd;
	masterCmd[5] = confRDCmd;
	masterCmd[6] = confACLCmd;
	masterCmd[7] = confRouterRIPCmd;
	masterCmd[8] = confRouterOSPFCmd;

	masternoCmd[0] = nouserCmd;
	masternoCmd[1] = noenableCmd;
	masternoCmd[2] = noconfCmd;
	masternoCmd[3] = noconfIfCmd;
	masternoCmd[4] = noconfFWCmd;
	masternoCmd[5] = noconfRDCmd;
	masternoCmd[6] = noconfACLCmd;
	masternoCmd[7] = noconfRouterRIPCmd;
	masternoCmd[8] = noconfRouterOSPFCmd;

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
		while(i < MAX_NO_CMDS[promptMode])
		{
			if(strcmp(nocmd[0],masternoCmd[promptMode][i].name) == 0)
			{
				return (*masternoCmd[promptMode][i].handler)(nocmd[1]);
			}
			++i;
		}
	}
	else
	{
		while(i < MAX_CMDS[promptMode])
		{
			if(strcmp(cmd[0],masterCmd[promptMode][i].name) == 0)
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
