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

#include "command_conf.h"
#include "prompt.h"
#include "configuration.h"

void cCMD_exit(char* _none)
{
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   exit\n");
	else
		promptMode = PROMPT_ENABLE;
}

void cCMD_firewall(char* _none)
{
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   firewall\n");
	else
		promptMode = PROMPT_CONF_FW;
}

void cCMD_hostname(char* args)
{
	if(strlen(args) <= 1)
	{
		CMDCONF_HOSTNAME_ERROR();
	}
	else
	{
		char* _hostname[2];
		cutFirstWord(args,_hostname);
		if(strlen(_hostname[1]) > 0)
		{
			CMDCONF_HOSTNAME_ERROR();
		}
		else
		{
			hostname = _hostname[0];
			writeRunningConfig();
		}
	}
}
