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

#include "cli_socket.h"
#include "command.h"
#include "configuration.h"
#include "prompt.h"

int main(int argc, const char** argv)
{
	// Set default hostname
	hostname = "BSDRouter";

	// @ TODO: load hostname from BSDRouterd

	int sock_error = openShellSocket();
	if(sock_error == 0)
	{
		promptMode = PROMPT_USER;

		printf("Type help to see the commands.\n");
		prompt();

		while(1)
		{
			sendPacket(readCmd());
			// @ TODO: receive and handle the callback
			prompt();
		}
	}
	else if(sock_error == 1)
		printf("BSDRouter-Shell fail to connect with BSDRouter Daemon. Failed to create socket !\n");
	else if(sock_error == 2)
		printf("BSDRouter-Shell fail to connect with BSDRouter Daemon. BSDRouter Daemon not running ?\n");
	else
		printf("BSDRouter-Shell fail to connect with BSDRouter Daemon. Unknown error %d\n",sock_error);
	closeShellSocket();
	return 0;
}