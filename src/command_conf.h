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

#ifndef __COMMAND_CONF_H__
#define __COMMAND_CONF_H__

#define CMDCONF_HOSTNAME_ERROR() printError("Syntax error !\nCorrect syntax is: \n   hostname <string>\n");
#define CMDCONF_INTERFACE_ERROR() printError("Syntax error !\nCorrect syntax is: \n   interface <name>\n");
#define CMDCONF_INTERFACE_UNK(A) printError("Unknown interface %s\n",A);
#define CMDCONF_IP_ERROR() printError("Syntax error !\nCorrect syntax is: \n   ip <routing>\n");

void cCMD_exit(char* _none);
void cCMD_firewall(char* _none);
void cCMD_hostname(char* args);
void cCMD_interface(char* args);
void cCMD_ip(char* args);
#endif
