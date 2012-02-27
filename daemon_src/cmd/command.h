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

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <sys/types.h>

#define CMD_ASKCONFIRM() askConfirm();

typedef struct cmdCallback
{
	unsigned short promptMode;
	char* message;
} cmdCallback;

typedef struct commandHandler
{
	const char* name;
	cmdCallback (*handler)(char* otherArgs);
} cmdHdlr;

#define MAX_CMD_TYPES	9

#define MAX_USER_CMD			4
#define MAX_ENABLE_CMD			4
#define MAX_CONF_CMD			6
#define MAX_CONF_IF_CMD 		5
#define MAX_CONF_FW_CMD			5
#define MAX_CONF_RD_CMD			1
#define MAX_CONF_ACL_CMD		3
#define MAX_CONF_ROUTER_RIP_CMD		7
#define MAX_CONF_ROUTER_OSPF_CMD	6

static const unsigned short MAX_CMDS[MAX_CMD_TYPES] =
{
	MAX_USER_CMD,
	MAX_ENABLE_CMD,
	MAX_CONF_CMD,
	MAX_CONF_IF_CMD,
	MAX_CONF_FW_CMD,
	MAX_CONF_RD_CMD,
	MAX_CONF_ACL_CMD,
	MAX_CONF_ROUTER_RIP_CMD,
	MAX_CONF_ROUTER_OSPF_CMD
};

cmdHdlr userCmd[MAX_USER_CMD];
cmdHdlr enableCmd[MAX_ENABLE_CMD];
cmdHdlr confCmd[MAX_CONF_CMD];
cmdHdlr confIfCmd[MAX_CONF_IF_CMD];
cmdHdlr confFWCmd[MAX_CONF_FW_CMD];
cmdHdlr confRDCmd[MAX_CONF_RD_CMD];
cmdHdlr confACLCmd[MAX_CONF_ACL_CMD];
cmdHdlr confRouterRIPCmd[MAX_CONF_ROUTER_RIP_CMD];
cmdHdlr confRouterOSPFCmd[MAX_CONF_ROUTER_OSPF_CMD];

cmdHdlr* masterCmd[MAX_CMD_TYPES];

// Inverted commands

#define MAX_USER_NO_CMD 0
#define MAX_ENABLE_NO_CMD 0
#define MAX_CONF_NO_CMD 3
#define MAX_CONF_IF_NO_CMD 4
#define MAX_CONF_FW_NO_CMD 1
#define MAX_CONF_RD_NO_CMD 0
#define MAX_CONF_ACL_NO_CMD 2
#define MAX_CONF_ROUTER_RIP_NO_CMD 6
#define MAX_CONF_ROUTER_OSPF_NO_CMD 5

static const unsigned short MAX_NO_CMDS[MAX_CMD_TYPES] =
{
	MAX_USER_NO_CMD,
	MAX_ENABLE_NO_CMD,
	MAX_CONF_NO_CMD,
	MAX_CONF_IF_NO_CMD,
	MAX_CONF_FW_NO_CMD,
	MAX_CONF_RD_NO_CMD,
	MAX_CONF_ACL_NO_CMD,
	MAX_CONF_ROUTER_RIP_NO_CMD,
	MAX_CONF_ROUTER_OSPF_NO_CMD
};


cmdHdlr nouserCmd[MAX_USER_NO_CMD];
cmdHdlr noenableCmd[MAX_ENABLE_NO_CMD];
cmdHdlr noconfCmd[MAX_CONF_NO_CMD];
cmdHdlr noconfIfCmd[MAX_CONF_IF_NO_CMD];
cmdHdlr noconfFWCmd[MAX_CONF_FW_NO_CMD];
cmdHdlr noconfRDCmd[MAX_CONF_RD_NO_CMD];
cmdHdlr noconfACLCmd[MAX_CONF_ACL_NO_CMD];
cmdHdlr noconfRouterCmd[MAX_CONF_ROUTER_RIP_NO_CMD];
cmdHdlr noconfRouterRIPCmd[MAX_CONF_ROUTER_RIP_NO_CMD];
cmdHdlr noconfRouterOSPFCmd[MAX_CONF_ROUTER_OSPF_NO_CMD];

cmdHdlr* masternoCmd[MAX_CMD_TYPES];

unsigned short initCmds();

cmdCallback handleCmd(char* _cmd, unsigned short promptMode);

void hsystemcmd(char* cmd);

#endif
