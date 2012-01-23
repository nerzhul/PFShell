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

#ifndef __PROMPT_MSG_H_
#define __PROMPT_MSG_H_

#include "prompt.h"

#define PRINTERROR(a) printError(a);
#define PRINTSUCCESS(a) printSuccess(a);

// Main Messages
#define CMDMAIN_INIT_FAIL() PRINTERROR("PFShell is corrupt, please reinstall it\n");
#define CMDMAIN_CONFINIT_FAIL() PRINTERROR("System configuration is corrupted !\n");

// Configuration Messages
#define CMD_WRITE_RUN_FAIL() printError("Writing failed for running-config !!!\n");

// Common Messages
#define CMDCOMMON_EXIT_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   exit\n");
#define CMD_UNK() PRINTERROR("Unknown command\n");

// User menu Messages
#define CMDUSER_SHOW_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   show (version)\n");
#define CMDUSER_ENABLE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   enable\n");

// Enable menu Messages
#define CMDEN_SHOW_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   show (acls|firewall|ip|interfaces|version)\n");
#define CMDEN_CONFIGURE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   configure (<firewall>)\n");
#define CMDEN_SHOW_FIREWALL_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   show firewall stats\n");
#define CMDEN_SHOW_IP_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   show ip routes\n");
#define CMDEN_SHOW_INTERFACES_NOTFOUND() PRINTERROR("No hardware interface found !\n");
#define CMDEN_SAVE_SUCCESS() PRINTSUCCESS("Save done with success !\n");

// Configure menu Messages
#define CMDCONF_HOSTNAME_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   hostname <string>\n");
#define CMDCONF_INTERFACE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   interface <name>\n");
#define CMDCONF_INTERFACE_UNK(A) printError("Unknown interface %s\n",A);
#define CMDCONF_IP_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   ip <route|routing|multicast-routing>\n");
#define CMDCONF_FIREWALL_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is: \n   firewall\n");
#define CMDCONF_IPROUTE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n    ip route A.B.C.D M.A.S.K G.A.T.E\n");

// Interface Messages
#define CMDIF_IP_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   ip (address) ...\n");
#define CMDIF_IPADDR_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   ip address A.B.C.D M.A.S.K\n   ip address A.B.C.D/CIDR\n   ip address DHCP\n");
#define CMDIF_SHUTDOWN_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   shutdown\n");
#define CMDIF_NOSHUTDOWN_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   shutdown\n");
#define CMDIF_ACCESS_LIST_ERROR() PRINTERROR("Syntax error !\n Correct syntax is:\n   ip access-group <name> <in|out>\n");
#define CMDIF_ACCESS_LIST_UNK() PRINTERROR("Error !\n No ACL matches this name\n");
#define CMDIF_DESC_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   description <string>\n");
#define CMDIF_FATAL_ERROR() PRINTERROR("Interface Fatal Error !!\n");

// Firewall Messages
#define CMDFW_DISABLE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   disable\n");
#define CMDFW_DEFAULT_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   default (input-policy|output-policy) (allow|deny)\n");
#define CMDFW_DISABLE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   disable\n");
#define CMDFW_DISABLE_SUCCESS() PRINTSUCCESS("Firewall disabled !\n Don't forget to re-enable it faster !!\n");
#define CMDFW_ENABLE_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   enable\n");
#define CMDFW_ENABLE_SUCCESS() PRINTSUCCESS("Firewall enabled.\n");
#define CMDFW_RELOAD_SUCCESS() PRINTSUCCESS("Firewall reloaded.\n");
#define CMDFW_ACL_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   acl <name>\n");

// ACL Messages
#define CMDACL_ERROR() PRINTERROR("Syntax error !\nCorrect syntax is:\n   <allow|deny> <in|out> <tcp|udp|icmp> <any|A.B.C.D/CIDR> (<port>) <any|A.B.C.D/CIDR> (<port>)\n");

#endif
