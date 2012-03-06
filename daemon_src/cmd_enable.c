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
#include "cmd_enable.h"
#include "configuration.h"
#include "interface.h"
#include "prompt_msg.h"
#include "route.h"
#include "string_mgmt.h"
#include "sysunix.h"

cmdCallback eCMD_configure(char* args)
{
	cmdCallback cb = {PROMPT_ENABLE,""};
	if(strlen(args) > 0)
	{
		if(strcmp(args,"firewall") == 0)
		{
			cb.promptMode = PROMPT_CONF_FW;
		}
		else
			cb.message = CMDEN_CONFIGURE_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF;
	return cb;
}

cmdCallback eCMD_exit(char* _none)
{
	cmdCallback cb = {PROMPT_ENABLE,""};
	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_USER;
	return cb;
}

cmdCallback eCMD_show(char* args)
{
	cmdCallback cb = {PROMPT_ENABLE,""};
	if(strlen(args) <= 1)
	{
		cb.message = CMDEN_SHOW_ERROR();
	}
	else
	{
		char* showcmd[2];
		cutFirstWord(args,showcmd);
		if(strcmp(showcmd[0],"version") == 0)
		{
			sprintf(cb.message,"PFShell version %s\n",VERSION);
		}
		else if(strcmp(showcmd[0],"acls") == 0)
		{
			char buffer[4000] = "";
			strcat(buffer,"------------- Current ACLs -------------\n");
			unsigned short found = 0;
			acl* cursor = access_lists;
			while(cursor != NULL)
			{
				char buffer2[1000] = "";
				sprintf(buffer2,"Access-List %s\n",cursor->name);
				strcat(buffer,buffer2);
				access_control* cursor2 = cursor->ac;
				while(cursor2 != NULL)
				{
					found = 1;
					strcat(buffer," ");
					if(cursor2->_allow == 0)
						strcat(buffer,"deny ");
					else
						strcat(buffer,"allow ");

					if(cursor2->_proto == 0)
						strcat(buffer,"tcp ");
					else if(cursor2->_proto == 1)
						strcat(buffer,"udp ");
					else
						strcat(buffer,"icmp ");

					sprintf(buffer2,"%s ",cursor2->_saddr);
					strcat(buffer,buffer2);
					if(cursor2->_sport > 0)
					{
						sprintf(buffer2,"%d ",cursor2->_sport);
						strcat(buffer,buffer2);
					}

					sprintf(buffer2,"%s ",cursor2->_daddr);
					strcat(buffer,buffer2);
					if(cursor2->_dport > 0)
					{
						sprintf(buffer2,"%d",cursor2->_dport);
						strcat(buffer,buffer2);
					}
					strcat(buffer,"\n");
					cursor2 = cursor2->next;
				}
				strcat(buffer,"\n");
				cursor = cursor->next;
			}
			if(found == 0) strcat(buffer,"No Access-Lists found !\n");

			cb.message = buffer;
		}
		else if(strcmp(showcmd[0],"firewall") == 0)
		{
			char* fwArg[2];
			cutFirstWord(showcmd[1],fwArg);
			if(strcmp(fwArg[0],"stats") == 0)
			{
				if(strlen(fwArg[1]) > 0)
				{
					cb.message = CMDEN_SHOW_FIREWALL_ERROR();
				}
				else
				{
					char buffer[4000] = "";
					execSystemCommand("/sbin/pfctl -si",buffer);
					cb.message = buffer;
				}
			}
			else
				cb.message = CMDEN_SHOW_FIREWALL_ERROR();
		}
		else if(strcmp(showcmd[0],"interfaces") == 0)
		{
			net_iface* iface = interfaces;
			if(iface == NULL)
			{
				cb.message = CMDEN_SHOW_INTERFACES_NOTFOUND();
			}
			else
			{
				char pbuffer[4000] = "";
				strcpy(pbuffer,"Running Interfaces:\n");
				while(iface != NULL)
				{
					char buffer[1024] = "";
					char output[3900] = "";

					strcpy(buffer,"/sbin/ifconfig ");
					strcat(buffer,iface->name);

					execSystemCommand(buffer,output);

					strcat(pbuffer,output);
					strcat(pbuffer,"\n");
					iface = iface->next;
				}
				cb.message = pbuffer;
			}
		}
		else if(strcmp(showcmd[0],"ip") == 0)
		{
			char* ipt[2];
			cutFirstWord(showcmd[1],ipt);

			if(strcmp(ipt[0],"routes") == 0)
			{
				char output[10240] = "";
				system("netstat -rn -f inet | awk '{print $1\" \"$2\" \"$3\" \"$7\" \"$8}' | grep U > /tmp/routes_tmp");
				FILE* fRoutes = fopen("/tmp/routes_tmp","r");
				char lines[1035] = "";

				while (fgets(lines, sizeof(lines), fRoutes) != NULL) {
					char* entries[5];
					uint8_t nbargs = cutString(lines,entries);

					if(nbargs != 5)
						continue;

					if(strcmp(entries[2],"UC") == 0) // Connected
					{
						strcat(output,"C\t");
						strcat(output,entries[0]);
						strcat(output," ");
						strcat(output,"is directly connected, ");
						strcat(output,entries[4]);
						strcat(output,"\n");
					}
					else if(strcmp(entries[2],"UGS") == 0) // Static
					{
						strcat(output,"S");
						if(strcmp(entries[0],"default") == 0)
						{
							strcat(output,"*\t0.0.0.0/0");
						}
						else
						{
							strcat(output,"\t");
							strcat(output,entries[0]);
						}
						strcat(output," [1/0] via ");
						strcat(output,entries[1]);
						strcat(output,"\n");
					}
					else if(strcmp(entries[2],"UG") == 0)
					{
						if(strcmp(entries[3],"32") == 0)
						{
							strcat(output,"O\t");
						}
						else if(strcmp(entries[3],"40") == 0)
						{
							strcat(output,"R\t");
						}
						strcat(output,entries[0]);
						strcat(output," [");
						strcat(output,entries[3]);
						strcat(output,"] via ");
						strcat(output,entries[1]);
						strcat(output,", ");
						strcat(output,entries[4]);
						strcat(output,"\n");
					}
				}
				cb.message = output;
			}
			else
				cb.message = CMDEN_SHOW_IP_ERROR();
		}
		else if(strcmp(showcmd[0],"running-config") == 0)
		{
			char output[10240] = "";
			execSystemCommand("cat /opt/PFShell/running-config",output);
			cb.message = output;
		}
		else if(strcmp(showcmd[0],"startup-config") == 0)
		{
			char output[10240] = "";
			execSystemCommand("cat /opt/PFShell/startup-config",output);
			cb.message = output;
		}
		else
		{
			cb.message = CMDEN_SHOW_ERROR();
		}
	}
	return cb;
}

cmdCallback eCMD_save(char* _none)
{
	cmdCallback cb = {PROMPT_ENABLE,""};
	// Save PFShell conf
	system("cp /opt/PFShell/running-config /opt/PFShell/startup-config");
	system("/bin/md5 /opt/PFShell/startup-config | /usr/bin/awk '{print $4}' > /opt/PFShell/startup-config.md5");
	system("/bin/sha1 /opt/PFShell/startup-config | /usr/bin/awk '{print $4}' > /opt/PFShell/startup-config.sha1");
	system("/bin/sha256 /opt/PFShell/startup-config | /usr/bin/awk '{print $4}' > /opt/PFShell/startup-config.sha256");
	// Save system interface conf
	saveInterfaces();
	// Save sysctl vars
	saveSysctl();
	system("/bin/md5 /etc/sysctl.conf | /usr/bin/awk '{print $4}' > /etc/sysctl.conf.md5");
	system("/bin/sha1 /etc/sysctl.conf | /usr/bin/awk '{print $4}' > /etc/sysctl.conf.sha1");
	system("/bin/sha256 /etc/sysctl.conf | /usr/bin/awk '{print $4}' > /etc/sysctl.conf.sha256");

	WRITE_HOSTNAME();
	system("cp /etc/myname.run /etc/myname");
	system("/bin/md5 /etc/myname | /usr/bin/awk '{print $4}' > /etc/myname.md5");
	system("/bin/sha1 /etc/myname | /usr/bin/awk '{print $4}' > /etc/myname.sha1");
	system("/bin/sha256 /etc/myname | /usr/bin/awk '{print $4}' > /etc/myname.sha256");

	// Save PacketFilter conf
	system("cp /etc/pf.conf.run /etc/pf.conf");
	system("/bin/md5 /etc/pf.conf | /usr/bin/awk '{print $4}' > /etc/pf.conf.md5");
	system("/bin/sha1 /etc/pf.conf | /usr/bin/awk '{print $4}' > /etc/pf.conf.sha1");
	system("/bin/sha256 /etc/pf.conf | /usr/bin/awk '{print $4}' > /etc/pf.conf.sha256");

	// Save ripd.conf
	system("cp /etc/ripd.conf /etc/ripd.conf.save");
	system("/bin/md5 /etc/ripd.conf | /usr/bin/awk '{print $4}' > /etc/ripd.conf.md5");
	system("/bin/sha1 /etc/ripd.conf | /usr/bin/awk '{print $4}' > /etc/ripd.conf.sha1");
	system("/bin/sha256 /etc/ripd.conf | /usr/bin/awk '{print $4}' > /etc/ripd.conf.sha256");

	// Save ospfd.conf
	system("cp /etc/ospfd.conf /etc/ospfd.conf.save");
	system("/bin/md5 /etc/ospfd.conf | /usr/bin/awk '{print $4}' > /etc/ospfd.conf.md5");
	system("/bin/sha1 /etc/ospfd.conf | /usr/bin/awk '{print $4}' > /etc/ospfd.conf.sha1");
	system("/bin/sha256 /etc/ospfd.conf | /usr/bin/awk '{print $4}' > /etc/ospfd.conf.sha256");

	// Save resolv.conf
	system("cp /etc/resolv.conf /etc/resolv.conf.save");
	system("/bin/md5 /etc/resolv.conf | /usr/bin/awk '{print $4}' > /etc/resolv.conf.md5");
	system("/bin/sha1 /etc/resolv.conf | /usr/bin/awk '{print $4}' > /etc/resolv.conf.sha1");
	system("/bin/sha256 /etc/resolv.conf | /usr/bin/awk '{print $4}' > /etc/resolv.conf.sha256");
	cb.message = CMDEN_SAVE_SUCCESS();
	return cb;
}

cmdCallback eCMD_help(char* _none)
{
	cmdCallback cb = {PROMPT_ENABLE,""};

	if(strlen(_none) > 0)
		return cb;

	char buffer[3072];
	bzero(buffer,3072);
	uint8_t i;
	for(i=0;i<MAX_ENABLE_CMD;i++)
	{
		char buffer2[256];
		bzero(buffer2,256);
		sprintf(buffer2,"%s%s\n",enableCmd[i].name,enableCmd[i].help);
		strcat(buffer,buffer2);
	}

	cb.message = (char*)malloc((strlen(buffer)+1)*sizeof(char));
	strcpy(cb.message,buffer);
	return cb;
}

