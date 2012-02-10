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

#include "prompt.h"

#include "configuration.h"

unsigned int initPrompts()
{
	// Set default mode to usermode
	promptMode = PROMPT_USER;

	promptTable[0].action = &promptU;
	promptTable[1].action = &promptE;
	promptTable[2].action = &promptC;
	promptTable[3].action = &promptCIf;
	promptTable[4].action = &promptCFW;
	promptTable[5].action = &promptCRD;
	promptTable[6].action = &promptCACL;

	return 1;
}

void prompt()
{
	fflush(stdout);
	(*promptTable[promptMode].action)();
}

// User prompt
void promptU() { printf("%s> ",hostname); }
// Enable prompt
void promptE() { printf("%s# ",hostname); }
// Configure prompt
void promptC() { printf("%s(conf)# ",hostname);}
// Configure prompt for Network Interfaces
void promptCIf() { printf("%s(conf-iface)# ",hostname);}
// Configure prompt for Firewall
void promptCFW() { printf("%s(conf-fw)# ",hostname); }
// Configure prompt for CARP redundancy
void promptCRD() { printf("%s(conf-redundancy)# ",hostname);}
// Configure prompt for ACLs
void promptCACL() { printf("%s(conf-acl)# ",hostname);}
// Configure prompt for router rip && router ospf
void promptCRouter() { printf("%s(conf-router)# ",hostname);}