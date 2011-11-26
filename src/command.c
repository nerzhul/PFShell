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
#include <string.h>
#include "command.h"
#include "command_user.h"
#include "command_enable.h"
#include "command_conf.h"
#include "command_conf_if.h"
#include "command_conf_fw.h"
#include "command_conf_rd.h"
#include "prompt.h"

unsigned short initCmds()
{
	//	User Mode Commands
	userCmd[0].name = "exit";
	userCmd[0].handler = &uCMD_exit;
	userCmd[1].name = "enable";
	userCmd[1].handler = &uCMD_enable;

	// Enable Mode Commands
	enableCmd[0].name = "exit";
	enableCmd[0].handler = &eCMD_exit;
	enableCmd[1].name = "configure";
	enableCmd[1].handler = &eCMD_configure;
	enableCmd[2].name = "save";
	enableCmd[2].handler = &eCMD_save;

	// Enable - Configure Commands
	confCmd[0].name = "exit";
	confCmd[0].handler = &cCMD_exit;

	// Enable - Configure - Interface Commands
	confIfCmd[0].name = "exit";
	confIfCmd[0].handler = &cifCMD_exit;
	confIfCmd[1].name = "shutdown";
	// @ TODO
	confIfCmd[2].name = "no shutdown";
	// @ TODO

	// Enable - Configure - Firewall Commands
	confFWCmd[0].name = "exit";
	confFWCmd[0].handler = &cfwCMD_exit;

	// Enable - Configure - Redundancy Commands
	confRDCmd[0].name = "exit";
	confRDCmd[0].handler = &crdCMD_exit;

	masterCmd[0] = userCmd;
	masterCmd[1] = enableCmd;
	masterCmd[2] = confCmd;
	masterCmd[3] = confIfCmd;
	masterCmd[4] = confFWCmd;
	masterCmd[5] = confRDCmd;

	return 1;
}

// read only 1024 chars for this moment
// maybe we read less chars in the future
char* readCmd()
{
	char cmd[1024];
	scanf("%s",cmd);
	return cmd;
}

void handleCmd(char* _cmd)
{
	unsigned int i;
	i = 0;

	while(i < MAX_USER_CMD)
	{
		if(strcmp(_cmd,masterCmd[promptMode][i].name) == 0)
		{
			// Todo: cut the string
			(*masterCmd[promptMode][i].handler)("");
			// Bad thing but improve performance code
			return;
		}
		++i;
	}
	printf("Unknown command\n");
}