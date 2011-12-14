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

#include "command_enable.h"
#include "prompt.h"

void eCMD_configure(char* _none) {
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   configure\n");
	else
		promptMode = PROMPT_CONF;
}

void eCMD_exit(char* _none) {
	if(strlen(_none) > 0)
		printError("Syntax error !\nCorrect syntax is: \n   exit\n");
	else
		promptMode = PROMPT_USER;
}

void eCMD_show(char* args)
{
	if(strlen(args) <= 1)
	{
		CMDEN_SHOW_ERROR();
	}
	else
	{
		char* showcmd[2];
		cutFirstWord(args,showcmd);
		if(strcmp(showcmd[0],"version") == 0)
		{
				printf("PFShell version %s\n",VERSION);
		}
		else if(strcmp(showcmd[0],"interfaces") == 0)
		{
			char output[10240] = "";
			execSystemCommand("for IF in $(/sbin/ifconfig | grep HWaddr | awk '{print $1}'); do /sbin/ifconfig $IF; done;",output);
			printf("Network Interfaces:\n%s",output);
		}
		else
		{
			CMDEN_SHOW_ERROR();	
		}
	}
}

void eCMD_save(char* _none)
{
	system("cp /opt/PFShell/running-config /opt/PFShell/startup-config");
	printSuccess("Save done with success !\n");
}
