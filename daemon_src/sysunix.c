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
#include "prompt/prompt_msg.h"
#include "sysunix.h"

unsigned short checkSystemIntegrity()
{
	printf("Starting configuration checks...");
	fflush(stdout);

	char buffer[1024] = "";
	char buffer2[1024] = "";

	unsigned short error = 0;

	printf(".");
	fflush(stdout);

	execSystemCommand("/bin/md5 /etc/sysctl.conf | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/sysctl.conf.md5",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nsysctl.conf is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha1 /etc/sysctl.conf | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/sysctl.conf.sha1",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nsysctl.conf is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha256 /etc/sysctl.conf | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/sysctl.conf.sha256",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nsysctl.conf is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	// Resolv.conf check
	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/md5 /etc/resolv.conf.save | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/resolv.conf.md5",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nresolv.conf is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha1 /etc/resolv.conf.save | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/resolv.conf.sha1",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nresolv.conf is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha256 /etc/resolv.conf.save | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/resolv.conf.sha256",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nresolv.conf is corrupted !\n");
		error = 1;
	}

	// check Startup Config
	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/md5 /opt/PFShell/startup-config  | awk '{print $4}'",buffer);
	execSystemCommand("cat /opt/PFShell/startup-config.md5",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nstartup-config is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha1 /opt/PFShell/startup-config  | awk '{print $4}'",buffer);
	execSystemCommand("cat /opt/PFShell/startup-config.sha1",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nstartup-config is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha256 /opt/PFShell/startup-config  | awk '{print $4}'",buffer);
	execSystemCommand("cat /opt/PFShell/startup-config.sha256",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nstartup-config is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/md5 /etc/pf.conf | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/pf.conf.md5",buffer2);

	printf(".");
	fflush(stdout);

	// Check packet filter
	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nPacket Filter configuration is corrupted !\n");
		error = 1;
	}

	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha1 /etc/pf.conf | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/pf.conf.sha1",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nPacket Filter configuration is corrupted !\n");
		error = 1;
	}


	printf(".");
	fflush(stdout);

	buffer[1024] = "";
	buffer2[1024] = "";
	execSystemCommand("/bin/sha256 /etc/pf.conf | /usr/bin/awk '{print $4}'",buffer);
	execSystemCommand("cat /etc/pf.conf.sha256",buffer2);

	printf(".");
	fflush(stdout);

	if(strcmp(buffer,buffer2) != 0)
	{
		printf("\x1b[31mERROR\x1b[0m\nPacket Filter configuration is corrupted !\n");
		error = 1;
	}

	return error;
}

unsigned short execSystemCommand(char* cmd, char* output)
{
	FILE *fp;
	char path[1035] = "";

	strcpy(output,"");

	fp = popen(cmd, "r");
	if (fp == NULL) {
		printError("Failed to run command %s\n",cmd);
		return -1;
	}

	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		strcat(output,path);
	}

	pclose(fp);
	return 0;
}