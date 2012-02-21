/*
* Copyright (c) 2011-2012, Loïc BLOT - Frost Sapphire Studios
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
#include "conf_router_ospf.h"
#include "configuration.h"
#include "../prompt/prompt.h"
#include "../prompt/prompt_msg.h"
#include "../string_mgmt.h"
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
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	// Cutting args
	char* redistargs[5];
	uint8_t nbargs = cutString(args,redistargs);

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
	else
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
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
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	// Cutting args
	char* redistargs[5];
	uint8_t nbargs = cutString(args,redistargs);

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
	else
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
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
	uint8_t nbargs = cutString(args,iface);

	if(nbargs > 1)
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
	uint8_t nbargs = cutString(args,iface);

	if(nbargs > 1)
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
	uint8_t nbargs = cutString(args,rid);

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
	uint8_t nbargs = cutString(args,rid);

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