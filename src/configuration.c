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
#include <stdio.h>
#include "configuration.h"

unsigned short loadConfiguration()
{
	// Temporary path
	FILE* confFile;
	confFile = fopen("/opt/PFShell/running-config","r");
	if(!confFile)
	{
		system("mkdir -p /opt/PFShell >> /dev/null 2>&1");
		confFile = fopen("/opt/PFShell/running-config", "w+");
		fclose(confFile);
		return 1;
	}
	
	pfpolicies[0] = 1;
	pfpolicies[1] = 1;
	pfpolicies[2] = 1;
	
	// Read file
	char path[1035];
	
	while (fgets(path, sizeof(path)-1, confFile) != NULL) {
		char* keyval[2];
		cutFirstWord(path,keyval);
		// @TODO
		if(strcmp(keyval[0],"hostname") == 0)
		{
			if(strlen(keyval[1]) > 0)
			{
				char* hname[2];
				cutFirstWord(keyval[1],hname);
				if(strlen(hname[1]) > 0)
					hostname = "PFShell";
				else
					hostname = hname[0];
			}
			else
				hostname = "PFShell";
		}
		else if(strcmp(keyval[0],"default") == 0)
		{
			if(strlen(keyval[1]) > 0)
			{
				char* defarg[2];
				cutFirstWord(keyval[1],defarg);
				if(strlen(defarg[1]) > 0)
				{
					if(strcmp(defarg[0],"input-policy") == 0)
					{
						pfpolicies[0] = (strcmp(defarg[1],"deny") == 0) ? 0 : 1;
					}
					else if(strcmp(defarg[0],"output-policy") == 0)
					{
						pfpolicies[1] = (strcmp(defarg[1],"deny") == 0) ? 0 : 1;
					}
					else if(strcmp(defarg[0],"forward-policy") == 0)
					{
						pfpolicies[2] = (strcmp(defarg[1],"deny") == 0) ? 0 : 1;
					}
				}
			}
		}
	}	
	
	fclose(confFile);
	return 1;
}

unsigned short writeRunningConfig()
{
	FILE* confFile;
	confFile = fopen("/opt/PFShell/running-config","w");
	if(confFile == NULL)
	{
		printError("Writing failed for running-config !!!\n");
	}
	else
	{
		fputs("hostname ",confFile);
		fputs(hostname,confFile);
		fputs("\n",confFile);
		fputs("!\n",confFile);
		fputs("default input-policy ",confFile);
		fputs((pfpolicies[0] == 0 ? "deny" : "allow"),confFile);
		fputs("\n",confFile);
		fputs("default output-policy ",confFile);
		fputs((pfpolicies[1] == 0 ? "deny" : "allow"),confFile);
		fputs("\n",confFile);
		fputs("default forward-policy ",confFile);
		fputs((pfpolicies[2] == 0 ? "deny" : "allow"),confFile);
		fputs("\n",confFile);
		// @TODO other fields
		fclose(confFile);
	}
}
