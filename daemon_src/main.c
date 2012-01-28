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

#include <stdio.h>

#include "daemon_socket.h"
#include "cmd/command.h"
#include "configuration.h"
#include "prompt/prompt_msg.h"

int main(int argc, const char** argv)
{
	printf("BSDRouterd version %s\n",VERSION);
	initCmds();
	// @TODO daemonize
	// @TODO: verify integrity for configuration files

	// Flush routes to manage it with the software
	hsystemcmd("route flush");

	if(!loadConfiguration())
	{
		printf("%s",printError("System configuration is corrupted !\n"));
		return -1;
	}

	int sock_error = openServerSocket();
	if(sock_error == 0)
	{
		printf("%s",printSuccess("BSDRouterd is running !\n"));
		waitAndHandleClients();
	}
	else if(sock_error == 1)
		printf("%s",printError("[ERROR] BSDRouterd: TCP/IP Stack error\n"));
	else if(sock_error == 2)
		printf("%s",printError("[ERROR] BSDRouterd: unable to bind port... already in use ? (error %d)\n",sock_error));
	else if(sock_error == 3)
		printf("%s",printError("[ERROR] BSDRouterd: unable to listen port... already in use (error %d)?\n",sock_error));
	else
		printf("%s",printError("[ERROR] BSDRouterd: unk error\n"));
	closeServerSocket();
	return 0;
}