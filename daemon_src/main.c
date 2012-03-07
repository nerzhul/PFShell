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
*     * Neither the name of the BSDRouterd nor the
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

#include "daemon_socket.h"
#include "cmd_command.h"
#include "configuration.h"
#include "prompt_msg.h"
#include "sysunix.h"

int main(int argc, const char** argv)
{
	printf("\x1b[0mBSDRouterd version \x1b[33m%s\x1b[0m\n",VERSION);
	is_loading = 1;
	initCmds();

	if(checkSystemIntegrity() == 0)
		printf("\nIntegrity: \x1b[32mOK\x1b[0m\n");
	else
		printf("\nIntegrity: \x1b[31mFAIL\x1b[0m\n");

	// Flush routes to manage it with the software
	hsystemcmd("route flush");

	// Kill ripd, ospfd and all dhcrelay processes
	hsystemcmd("pkill ripd");
	hsystemcmd("pkill ospfd");
	hsystemcmd("pkill dhcrelay");

	if(!loadConfiguration())
	{
		printf("\x1b[31mSystem configuration is corrupted\x1b[0m !\n");
		return -1;
	}

	int sock_error = openServerSocket();
	if(sock_error == 0)
	{
		system("/opt/bin/ripd");
		system("/usr/sbin/ospfd");
		printf("BSDRouterd is \x1b[32mrunning\x1b[0m !\n");
		is_loading = 0;

		// Now we daemonize
		/*pid_t pid = fork();
		if(pid < 0)
			printf("\x1b[31mFATAL Fork Error !\x1b[0m !\n");
		else if(pid > 0)
			return 0;*/

		waitAndHandleClients();
		closeServerSocket();
	}
	else if(sock_error == 1)
		printf("%s",printError("[ERROR] BSDRouterd: TCP/IP Stack error\n"));
	else if(sock_error == 2)
		printf("%s",printError("[ERROR] BSDRouterd: unable to bind port... already in use ? (error %d)\n",sock_error));
	else if(sock_error == 3)
		printf("%s",printError("[ERROR] BSDRouterd: unable to listen port... already in use (error %d)?\n",sock_error));
	else
		printf("%s",printError("[ERROR] BSDRouterd: unk error\n"));

	return 0;
}
