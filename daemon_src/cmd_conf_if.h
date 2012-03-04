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

#ifndef __CONF_IF_H__
#define __CONF_IF_H__

#include "cmd_command.h"

char* current_iface;
uint16_t current_iface_id;

cmdCallback cifCMD_exit(char* _none);

// IP management
cmdCallback cifCMD_ip(char* args);
cmdCallback cifCMD_ip_address(char* args);
cmdCallback cifCMD_noip(char* args);

// ACLS
cmdCallback cifCMD_access_list(char* args);
cmdCallback cifCMD_noaccess_list(char* args);

// Interface Mgmt
cmdCallback cifCMD_shutdown(char* _none);
cmdCallback cifCMD_noshutdown(char* _none);

// RIP
cmdCallback cifCMD_ip_rip(char* args);
cmdCallback cifCMD_noip_rip(char* args);

// OSPF
cmdCallback cifCMD_ip_ospf(char* args);
cmdCallback cifCMD_noip_ospf(char* args);

// Misc
cmdCallback cifCMD_description(char* args);
cmdCallback cifCMD_nodescription(char* args);
cmdCallback cifCMD_encap(char* args);
cmdCallback cifCMD_noencap(char* args);
cmdCallback cifCMD_ip_helper(char* args);
cmdCallback cifCMD_noip_helper(char* args);


#endif
