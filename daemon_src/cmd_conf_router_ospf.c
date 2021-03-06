/*
* Copyright (c) 2011-2012, Loïc BLOT - CNRS
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the BSDRouterd nor the
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
#include "cmd_conf_router_ospf.h"
#include "configuration.h"
#include "interface.h"
#include "iputils.h"
#include "prompt.h"
#include "prompt_msg.h"
#include "string_mgmt.h"
#include "route.h"


cmdCallback crouterCMD_OSPF_exit(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDCOMMON_EXIT_ERROR();
	}
	else
		cb.promptMode = PROMPT_CONF;

	return cb;
}

cmdCallback crouterCMD_OSPF_redistrib(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
		return cb;
	}

	// Cutting args
	char* redistargs[5];
	uint8_t nbargs = cutString(args,redistargs,5);

	if(nbargs == -1)
	{
		freeCutString(redistargs,nbargs);
		cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
		return cb;
	}

	if(strcmp(redistargs[0],"connected") == 0)
	{

		int metric_val = OSPF_DEFAULT_METRIC;
		int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
		if(nbargs >= 3)
		{
			// If its metric & has a value
			if(strcmp(redistargs[1],"metric") == 0)
			{
				// Convert the value
				int tmp_metric_val = atoi(redistargs[2]);

				// If the value is between 0 and 65535 included
				if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
					metric_val = tmp_metric_val;
				else
				{
					cb.message = CMDROUTER_OSPF_REDIST_ERROR();
					freeCutString(redistargs,nbargs);
					return cb;
				}

				// If there is args after metric with value
				if(nbargs == 5)
				{
					// If there is a value and keywork is metric-type
					if(strcmp(redistargs[3],"metric-type") == 0)
					{
						int tmp_metric_type_val = atoi(redistargs[4]);

						// Must be 1 or 2
						if(tmp_metric_type_val < 1 && tmp_metric_type_val > 2)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						metric_type_val = tmp_metric_type_val;
					}
					else
					{
						cb.message = CMDROUTER_OSPF_REDIST_ERROR();
						freeCutString(redistargs,nbargs);
						return cb;
					}
				}
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				return cb;
			}
		}
		ospf_redistrib_conn = 1;
		ospf_redistrib_conn_metric = metric_val;
		ospf_redistrib_conn_type = metric_type_val;

	}
	else if(strcmp(redistargs[0],"static") == 0)
	{
		int metric_val = OSPF_DEFAULT_METRIC;
		int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
		if(nbargs >= 3)
		{
			// If its metric & has a value
			if(strcmp(redistargs[1],"metric") == 0)
			{
				// Convert the value
				int tmp_metric_val = atoi(redistargs[2]);

				// If the value is between 0 and 65535 included
				if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
					metric_val = tmp_metric_val;
				else
				{
					cb.message = CMDROUTER_OSPF_REDIST_ERROR();
					freeCutString(redistargs,nbargs);
					return cb;
				}

				// If there is args after metric with value
				if(nbargs == 5)
				{
					// If there is a value and keywork is metric-type
					if(strcmp(redistargs[3],"metric-type") == 0)
					{
						int tmp_metric_type_val = atoi(redistargs[4]);

						// Must be 1 or 2
						if(tmp_metric_type_val < 1 && tmp_metric_type_val > 2)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							freeCutString(redistargs,nbargs);
							return cb;
						}

						metric_type_val = tmp_metric_type_val;
					}
					else
					{
						cb.message = CMDROUTER_OSPF_REDIST_ERROR();
						freeCutString(redistargs,nbargs);
						return cb;
					}
				}
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				freeCutString(redistargs,nbargs);
				return cb;
			}
		}
		ospf_redistrib_static = 1;
		ospf_redistrib_static_metric = metric_val;
		ospf_redistrib_static_type = metric_type_val;
	}
	else if(strcmp(redistargs[0],"default") == 0)
	{
		int metric_val = OSPF_DEFAULT_METRIC;
		int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
		if(nbargs >= 3)
		{
			// If its metric & has a value
			if(strcmp(redistargs[1],"metric") == 0)
			{
				// Convert the value
				int tmp_metric_val = atoi(redistargs[2]);

				// If the value is between 0 and 65535 included
				if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
					metric_val = tmp_metric_val;
				else
				{
					cb.message = CMDROUTER_OSPF_REDIST_ERROR();
					freeCutString(redistargs,nbargs);
					return cb;
				}

				// If there is args after metric with value
				if(nbargs == 5)
				{
					// If there is a value and keywork is metric-type
					if(strcmp(redistargs[3],"metric-type") == 0)
					{
						int tmp_metric_type_val = atoi(redistargs[4]);

						// Must be 1 or 2
						if(tmp_metric_type_val < 1 && tmp_metric_type_val > 2)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							freeCutString(redistargs,nbargs);
							return cb;
						}

						metric_type_val = tmp_metric_type_val;
					}
					else
					{
						cb.message = CMDROUTER_OSPF_REDIST_ERROR();
						freeCutString(redistargs,nbargs);
						return cb;
					}
				}
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				freeCutString(redistargs,nbargs);
				return cb;
			}
		}
		ospf_redistrib_default = 1;
		ospf_redistrib_default_metric = metric_val;
		ospf_redistrib_default_type = metric_type_val;
	}
	else if(is_valid_ip_and_cidr(redistargs[0]) == 0)
	{
		if(nbargs >= 1)
		{
			int metric_val = OSPF_DEFAULT_METRIC;
			int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
			if(nbargs >= 3)
			{
				// If its metric & has a value
				if(strcmp(redistargs[1],"metric") == 0)
				{
					// Convert the value
					int tmp_metric_val = atoi(redistargs[2]);

					// If the value is between 0 and 65535 included
					if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
						metric_val = tmp_metric_val;
					else
					{
						cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
						freeCutString(redistargs,nbargs);
						return cb;
					}

					// If there is args after metric with value
					if(nbargs == 5)
					{
						// If there is a value and keywork is metric-type
						if(strcmp(redistargs[3],"metric-type") == 0)
						{
							int tmp_metric_type_val = atoi(redistargs[4]);

							// Must be 1 or 2
							if(tmp_metric_type_val < 1 && tmp_metric_type_val > 2)
							{
								cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
								freeCutString(redistargs,nbargs);
								return cb;
							}

							metric_type_val = tmp_metric_type_val;
						}
						else
						{
							cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
							freeCutString(redistargs,nbargs);
							return cb;
						}
					}
				}
				else
				{
					cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
					freeCutString(redistargs,nbargs);
					return cb;
				}
			}
			addRedistNetForOSPF(redistargs[0],metric_type_val,metric_val);
		}
		else
		{
			cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
			freeCutString(redistargs,nbargs);
			return cb;
		}
	}
	else
	{
		cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
		freeCutString(redistargs,nbargs);
		return cb;
	}

	WRITE_RUN();
	WRITE_OSPFD();

	freeCutString(redistargs,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_noredistrib(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
		return cb;
	}

	// Cutting args
	char* redistargs[5];
	uint8_t nbargs = cutString(args,redistargs,5);

	if(nbargs == -1)
	{
		cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
		freeCutString(redistargs,nbargs);
		return cb;
	}


	if(strcmp(redistargs[0],"connected") == 0)
	{
		if(nbargs >= 3)
		{
			// If its metric & has a value
			if(strcmp(redistargs[1],"metric") == 0)
			{
				// Convert the value
				int tmp_metric_val = atoi(redistargs[2]);

				// if value given mismatches saved
				if(tmp_metric_val != ospf_redistrib_conn_metric)
				{
					freeCutString(redistargs,nbargs);
					return cb;
				}

				// If there is args after metric with value
				if(nbargs == 5)
				{
					// If there is a value and keywork is metric-type
					if(strcmp(redistargs[3],"metric-type") == 0)
					{
						int tmp_metric_type_val = atoi(redistargs[4]);

						// if value given mismatches saved
						if(tmp_metric_type_val != ospf_redistrib_conn_type)
						{
							freeCutString(redistargs,nbargs);
							return cb;
						}

					}
				} // if no args given, verify if it's default value
				else if(ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
				{
					freeCutString(redistargs,nbargs);
					return cb;
				}
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				freeCutString(redistargs,nbargs);
				return cb;
			}
		} // if no args given, verify if it's default value
		else if(ospf_redistrib_conn_metric != OSPF_DEFAULT_METRIC)
		{
			freeCutString(redistargs,nbargs);
			return cb;
		}

		ospf_redistrib_conn = 0;
		ospf_redistrib_conn_metric = OSPF_DEFAULT_METRIC;
		ospf_redistrib_conn_type = OSPF_DEFAULT_METRIC_TYPE;
	}
	else if(strcmp(redistargs[0],"static") == 0)
	{
		if(nbargs >= 3)
		{
			// If its metric & has a value
			if(strcmp(redistargs[1],"metric") == 0)
			{
				// Convert the value
				int tmp_metric_val = atoi(redistargs[2]);

				// if value given mismatches saved
				if(tmp_metric_val != ospf_redistrib_static_metric)
				{
					freeCutString(redistargs,nbargs);
					return cb;
				}

				// If there is args after metric with value
				if(nbargs == 5)
				{
					// If there is a value and keywork is metric-type
					if(strcmp(redistargs[3],"metric-type") == 0)
					{
						int tmp_metric_type_val = atoi(redistargs[4]);

						// if value given mismatches saved
						if(tmp_metric_type_val != ospf_redistrib_static_type)
						{
							freeCutString(redistargs,nbargs);
							return cb;
						}

					}
				} // if no args given, verify if it's default value
				else if(ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
				{
					freeCutString(redistargs,nbargs);
					return cb;
				}
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				freeCutString(redistargs,nbargs);
				return cb;
			}
		} // if no args given, verify if it's default value
		else if(ospf_redistrib_static_metric != OSPF_DEFAULT_METRIC)
		{
			freeCutString(redistargs,nbargs);
			return cb;
		}

		ospf_redistrib_static = 0;
		ospf_redistrib_static_metric = OSPF_DEFAULT_METRIC;
		ospf_redistrib_static_type = OSPF_DEFAULT_METRIC_TYPE;
	}
	else if(strcmp(redistargs[0],"default") == 0)
	{
		if(nbargs >= 3)
		{
			// If its metric & has a value
			if(strcmp(redistargs[1],"metric") == 0)
			{
				// Convert the value
				int tmp_metric_val = atoi(redistargs[2]);

				// if value given mismatches saved
				if(tmp_metric_val != ospf_redistrib_default_metric)
				{
					freeCutString(redistargs,nbargs);
					return cb;
				}

				// If there is args after metric with value
				if(nbargs == 5)
				{
					// If there is a value and keywork is metric-type
					if(strcmp(redistargs[3],"metric-type") == 0)
					{
						int tmp_metric_type_val = atoi(redistargs[4]);

						// if value given mismatches saved
						if(tmp_metric_type_val != ospf_redistrib_default_type)
						{
							freeCutString(redistargs,nbargs);
							return cb;
						}

					}
				} // if no args given, verify if it's default value
				else if(ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
				{
					freeCutString(redistargs,nbargs);
					return cb;
				}
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				freeCutString(redistargs,nbargs);
				return cb;
			}
		} // if no args given, verify if it's default value
		else if(ospf_redistrib_conn_metric != OSPF_DEFAULT_METRIC)
		{
			freeCutString(redistargs,nbargs);
			return cb;
		}

		ospf_redistrib_default = 0;
		ospf_redistrib_default_metric = OSPF_DEFAULT_METRIC;
		ospf_redistrib_default_type = OSPF_DEFAULT_METRIC_TYPE;
	}
	else if(is_valid_ip_and_cidr(redistargs[0]) == 0)
	{
		if(nbargs == 1)
		{
			delRedistNetForOSPF(redistargs[0]);
		}
		else
		{
			cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
			freeCutString(redistargs,nbargs);
			return cb;
		}
	}
	else
	{
		cb.message = CMDROUTER_OSPF_REDISTGENERAL_ERROR();
		freeCutString(redistargs,nbargs);
		return cb;
	}

	WRITE_RUN();
	WRITE_OSPFD();

	freeCutString(redistargs,nbargs);
	return cb;
}


cmdCallback crouterCMD_OSPF_passive(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		return cb;
	}

	char* iface[1];
	uint8_t nbargs = cutString(args,iface,1);

	if(nbargs == -1)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		freeCutString(iface,nbargs);
		return cb;
	}

	if(setInterfaceOSPFPassive(iface[0],1) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
	}
	else
	{
		WRITE_RUN();
		WRITE_OSPFD();
	}

	freeCutString(iface,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_nopassive(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};
	if(strlen(args) == 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		return cb;
	}

	char* iface[1];
	uint8_t nbargs = cutString(args,iface,1);

	if(nbargs == -1)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
		freeCutString(iface,nbargs);
		return cb;
	}

	if(setInterfaceOSPFPassive(iface[0],0) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
	}
	else
	{
		WRITE_RUN();
		WRITE_OSPFD();
	}

	freeCutString(iface,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_routerid(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* rid[1];
	uint8_t nbargs = cutString(args,rid,1);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_OSPF_ROUTERID_ERROR();
	}
	else
	{
		if(is_valid_ip(rid[0]) == 0)
		{
			ospf_router_id = rid[0];
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
			cb.message = CMDROUTER_OSPF_ROUTERID_ERROR();
	}

	freeCutString(rid,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_norouterid(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* rid[1];
	uint8_t nbargs = cutString(args,rid,1);

	if(nbargs != 1)
	{
		cb.message = CMDROUTER_OSPF_ROUTERID_ERROR();
	}
	else
	{
		if(strcmp(rid[0],ospf_router_id) == 0)
		{
			ospf_router_id = "";
			WRITE_RUN();
			WRITE_OSPFD();
		}
		else
			cb.message = CMDROUTER_OSPF_ROUTERID_ERROR();
	}

	freeCutString(rid,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_timers(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* timers[3];
	uint8_t nbargs = cutString(args,timers,3);

	if(nbargs != 3 || strcmp(timers[0],"spf") != 0 || is_numeric(timers[1]) != 0 || is_numeric(timers[2]) != 0)
	{
		cb.message = CMDROUTER_OSPF_TIMERS_ERROR();
		freeCutString(timers,nbargs);
		return cb;
	}

	int spf_del = atoi(timers[1]);
	int spf_hold = atoi(timers[2]);

	if(spf_del > 0 && spf_hold > 0)
	{
		ospf_delay_timer = spf_del;
		ospf_holdtime_timer = spf_hold;
		WRITE_RUN();
		WRITE_OSPFD();
	}

	freeCutString(timers,nbargs);
	return cb;
}
cmdCallback crouterCMD_OSPF_notimers(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* timers[3];
	uint8_t nbargs = cutString(args,timers,3);

	if(nbargs != 3 || strcmp(timers[0],"spf") != 0 || is_numeric(timers[1]) != 0 || is_numeric(timers[2]) != 0)
	{
		cb.message = CMDROUTER_OSPF_TIMERS_ERROR();
		freeCutString(timers,nbargs);
		return cb;
	}

	int spf_del = atoi(timers[1]);
	int spf_hold = atoi(timers[2]);

	if(spf_hold > 5 || spf_hold < 1 || spf_del > 10 || spf_del < 1)
	{
		cb.message = CMDROUTER_OSPF_TIMERS_ERROR();
		freeCutString(timers,nbargs);
		return cb;
	}

	if(spf_del == (int)ospf_delay_timer && spf_hold == (int)ospf_holdtime_timer)
	{
		ospf_delay_timer = OSPF_DEFAULT_DELAY;
		ospf_holdtime_timer = OSPF_DEFAULT_HOLDTIME;
		WRITE_RUN();
		WRITE_OSPFD();
	}

	freeCutString(timers,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_network(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* netarea[3];
	uint8_t nbargs = cutString(args,netarea,3);

	if(nbargs != 3)
	{

		cb.message = CMDROUTER_OSPF_NETWORK_ERROR();
		freeCutString(netarea,nbargs);
		return cb;
	}

	if(strcmp(netarea[1],"area") != 0)
	{
		cb.message = CMDROUTER_OSPF_NETWORK_ERROR();
		freeCutString(netarea,nbargs);
		return cb;
	}

	if(is_interface(netarea[0]) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
		freeCutString(netarea,nbargs);
		return cb;
	}

	if(is_numeric(netarea[2]) == 0)
	{
		uint32_t area = atoi(netarea[2]);
		if(addIfaceToOSPFArea(netarea[0],area) != 0)
		{
			cb.message = CMDROUTER_OSPF_AREA_IFACE_ERROR();
		}
		else
		{
			WRITE_RUN();
			WRITE_OSPFD();
		}
	}
	else if(is_valid_ip(netarea[2]) == 0)
	{
		uint32_t area = convert_ip_to_int(netarea[2]);
		if(addIfaceToOSPFArea(netarea[0],area) != 0)
		{
			cb.message = CMDROUTER_OSPF_AREA_IFACE_ERROR();
		}
		else
		{
			WRITE_RUN();
			WRITE_OSPFD();
		}
	}
	else
	{
		cb.message = CMDROUTER_OSPF_NETWORK_ERROR();
	}

	freeCutString(netarea,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_nonetwork(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* netarea[3];
	uint8_t nbargs = cutString(args,netarea,3);

	if(nbargs != 3)
	{

		cb.message = CMDROUTER_OSPF_NETWORK_ERROR();
		freeCutString(netarea,nbargs);
		return cb;
	}

	if(strcmp(netarea[1],"area") != 0)
	{
		cb.message = CMDROUTER_OSPF_NETWORK_ERROR();
		freeCutString(netarea,nbargs);
		return cb;
	}

	if(is_interface(netarea[0]) != 0)
	{
		cb.message = CMD_INTERFACE_UNK();
		freeCutString(netarea,nbargs);
		return cb;
	}

	if(is_numeric(netarea[2]) == 0)
	{
		uint32_t area = atoi(netarea[2]);
		delIfaceFromOSPFArea(netarea[0],area);
		WRITE_RUN();
		WRITE_OSPFD();
	}
	else if(is_valid_ip(netarea[2]) == 0)
	{
		uint32_t area = convert_ip_to_int(netarea[2]);
		delIfaceFromOSPFArea(netarea[0],area);
		WRITE_RUN();
		WRITE_OSPFD();
	}
	else
	{
		cb.message = CMDROUTER_OSPF_NETWORK_ERROR();
	}

	freeCutString(netarea,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_area(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* area[3];
	uint8_t nbargs = cutString(args,area,3);

	if(nbargs < 2 || nbargs > 3 || is_valid_ip(area[0]) != 0 || strcmp(area[1],"stub") != 0 || (nbargs == 3 && strcmp(area[2],"no-summary") != 0))
	{
		cb.message = CMDROUTER_OSPF_AREA_STUB_ERROR();
		freeCutString(area,nbargs);
		return cb;
	}

	uint32_t area_id = convert_ip_to_int(area[0]);
	if(nbargs == 2)
		setOSPFAreaStub(area_id,1,1);
	else
		setOSPFAreaStub(area_id,1,0);

	WRITE_RUN();
	WRITE_OSPFD();
	freeCutString(area,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_noarea(char* args)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	char* area[3];
	uint8_t nbargs = cutString(args,area,3);

	if(nbargs < 2 || nbargs > 3 || is_valid_ip(area[0]) != 0 || strcmp(area[1],"stub") != 0 || nbargs == 3 && strcmp(area[2],"no-summary") != 0)
	{
		cb.message = CMDROUTER_OSPF_AREA_STUB_ERROR();
		freeCutString(area,nbargs);
		return cb;
	}

	if(nbargs == 2 || getOSPFAreaStubSummary(convert_ip_to_int(area[0])) == 1)
	{
		uint32_t area_id = convert_ip_to_int(area[0]);

		setOSPFAreaStub(area_id,0,0);
		WRITE_RUN();
		WRITE_OSPFD();
	}

	freeCutString(area,nbargs);
	return cb;
}

cmdCallback crouterCMD_OSPF_stub(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDROUTER_OSPF_STUB_ERROR();
		return cb;
	}

	ospf_stub_router = 1;

	WRITE_RUN();
	WRITE_OSPFD();
	return cb;
}

cmdCallback crouterCMD_OSPF_nostub(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	if(strlen(_none) > 0)
	{
		cb.message = CMDROUTER_OSPF_STUB_ERROR();
		return cb;
	}

	ospf_stub_router = 0;

	WRITE_RUN();
	WRITE_OSPFD();
	return cb;
}

cmdCallback crouterCMD_OSPF_help(char* _none)
{
	cmdCallback cb = {PROMPT_CONF_ROUTER_OSPF,""};

	if(strlen(_none) > 0)
		return cb;

	char buffer[3072];
	bzero(buffer,3072);
	uint8_t i;
	for(i=0;i<MAX_CONF_ROUTER_OSPF_CMD;i++)
	{
		char buffer2[256];
		bzero(buffer2,256);
		if(strlen(confRouterOSPFCmd[i].name) > 10)
			sprintf(buffer2,"%s%s\n",confRouterOSPFCmd[i].name,confRouterOSPFCmd[i].help);
		else
			sprintf(buffer2,"%s%s\n",confRouterOSPFCmd[i].name,confRouterOSPFCmd[i].help);
		strcat(buffer,buffer2);
	}

	cb.message = (char*)malloc((strlen(buffer)+1)*sizeof(char));
	strcpy(cb.message,buffer);
	return cb;
}
