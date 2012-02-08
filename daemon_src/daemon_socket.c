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
#include <stdio.h>

#include "daemon_socket.h"
#include "cmd/command.h"
#include "prompt/prompt.h"

unsigned short openServerSocket()
{
	ssock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(ssock == INVALID_SOCKET)
		return 1;

	memset(&ssin, 0, sizeof(ssin));

	ssin.sin_family = AF_INET;
	ssin.sin_port = htons(12589);
	ssin.sin_addr.s_addr = inet_addr("127.0.0.1");

	serror = bind(ssock, (struct sockaddr*)&ssin, sizeof(ssin));

	if(serror == SOCKET_ERROR)
	{
		closeServerSocket();
		return 2;
	}

	serror = listen(ssock, 5);

	if(serror == SOCKET_ERROR)
	{
		closeServerSocket();
		return 3;
	}

	return 0;
}

void waitAndHandleClients()
{
	while(1)
	{
		// @TODO: thread this
		if(csock = accept(ssock, (struct sockaddr*)&csin, &csize))
		{
			unsigned short closeSock = 0;
			while(closeSock == 0)
			{
				char buffer[4096] = "";
				int recvsize = recv(csock,buffer,4096,0);
				if(recvsize == 0 || recvsize < 1)
				{
					shutdown(csock,SHUT_RDWR);
					closeSock = 1;
				}
				else
					decodePacket(buffer);
			}
		}
	}
}

unsigned short closeServerSocket()
{
	shutdown(ssock,SHUT_RDWR);
	return 0;
}

void decodePacket(char* pkt)
{
	unsigned short promptMode;
	char command[4096] = "";

	int offset = 1;

	promptMode = (int)(char)pkt[0]-(int)'0';

	while(offset < strlen(pkt))
	{
		command[offset-1] = pkt[offset];
		++offset;
	}

	command[strlen(pkt)-1] = '\0';

	if(promptMode < MAX_PROMPTS)
	{
		cmdCallback cb = handleCmd(command,promptMode);
		char sendBuffer[4096] = "";
		sprintf(sendBuffer,"%d%s",cb.promptMode,cb.message);
		sendPacket(sendBuffer);
	}
}

unsigned short sendPacket(char* data)
{
	return send(csock,data,sizeof(char)*(strlen(data)+1),0);
}