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
#include "../prompt/prompt_msg.h"
#include "route.h"
#include "../prompt/prompt.h"

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
	char* redistargs[2];
	cutFirstWord(args,redistargs);

	if(strcmp(redistargs[0],"connected") == 0)
	{
		// Cutting args
		char* metricsstr[2];
		cutFirstWord(args,redistargs);

		int metric_val = OSPF_DEFAULT_METRIC;
		int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
		if(strlen(redistargs[1]) > 0)
		{
			// Cutting args after redist_type
			char* metcmd1[2];
			cutFirstWord(redistargs[1],metcmd1);

			// If its metric & has a value
			if(strlen(metcmd1[1]) > 0 && strcmp(metcmd1[0],"metric") == 0)
			{
				char* metarg1[2];
				cutFirstWord(metcmd1[1],metarg1);

				// Convert the value
				int tmp_metric_val = atoi(metarg1[0]);

				// If the value is between 0 and 65535 included
				if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
					metric_val = tmp_metric_val;
				else
				{
					cb.message = CMDROUTER_OSPF_REDIST_ERROR();
					return cb;
				}

				// If there is args after metric with value
				if(strlen(metarg1[1]) > 0)
				{
					char* metcmd2[2];
					cutFirstWord(metarg1[1],metcmd2);

					// If there is a value and keywork is metric-type
					if(strlen(metcmd2[1]) > 0 && strcmp(metcmd2[0],"metric-type") == 0)
					{
						char* metarg2[2];
						cutFirstWord(metcmd2[1],metarg2);

						// If there is another keyword after the last arg
						if(strlen(metarg2[1]) > 0)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						int tmp_metric_type_val = atoi(metarg2[0]);

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
		// Cutting args
		char* metricsstr[2];
		cutFirstWord(args,redistargs);

		int metric_val = OSPF_DEFAULT_METRIC;
		int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
		if(strlen(redistargs[1]) > 0)
		{
			// Cutting args after redist_type
			char* metcmd1[2];
			cutFirstWord(redistargs[1],metcmd1);

			// If its metric & has a value
			if(strlen(metcmd1[1]) > 0 && strcmp(metcmd1[0],"metric") == 0)
			{
				char* metarg1[2];
				cutFirstWord(metcmd1[1],metarg1);

				// Convert the value
				int tmp_metric_val = atoi(metarg1[0]);

				// If the value is between 0 and 65535 included
				if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
					metric_val = tmp_metric_val;
				else
				{
					cb.message = CMDROUTER_OSPF_REDIST_ERROR();
					return cb;
				}

				// If there is args after metric with value
				if(strlen(metarg1[1]) > 0)
				{
					char* metcmd2[2];
					cutFirstWord(metarg1[1],metcmd2);

					// If there is a value and keywork is metric-type
					if(strlen(metcmd2[1]) > 0 && strcmp(metcmd2[0],"metric-type") == 0)
					{
						char* metarg2[2];
						cutFirstWord(metcmd2[1],metarg2);

						// If there is another keyword after the last arg
						if(strlen(metarg2[1]) > 0)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						int tmp_metric_type_val = atoi(metarg2[0]);

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
		ospf_redistrib_static = 1;
		ospf_redistrib_static_metric = metric_val;
		ospf_redistrib_static_type = metric_type_val;
	}
	else if(strcmp(redistargs[0],"default") == 0)
	{
		// Cutting args
		char* metricsstr[2];
		cutFirstWord(args,redistargs);

		int metric_val = OSPF_DEFAULT_METRIC;
		int metric_type_val = OSPF_DEFAULT_METRIC_TYPE;
		if(strlen(redistargs[1]) > 0)
		{
			// Cutting args after redist_type
			char* metcmd1[2];
			cutFirstWord(redistargs[1],metcmd1);

			// If its metric & has a value
			if(strlen(metcmd1[1]) > 0 && strcmp(metcmd1[0],"metric") == 0)
			{
				char* metarg1[2];
				cutFirstWord(metcmd1[1],metarg1);

				// Convert the value
				int tmp_metric_val = atoi(metarg1[0]);

				// If the value is between 0 and 65535 included
				if(tmp_metric_val >= 0 && tmp_metric_val <= 65535)
					metric_val = tmp_metric_val;
				else
				{
					cb.message = CMDROUTER_OSPF_REDIST_ERROR();
					return cb;
				}

				// If there is args after metric with value
				if(strlen(metarg1[1]) > 0)
				{
					char* metcmd2[2];
					cutFirstWord(metarg1[1],metcmd2);

					// If there is a value and keywork is metric-type
					if(strlen(metcmd2[1]) > 0 && strcmp(metcmd2[0],"metric-type") == 0)
					{
						char* metarg2[2];
						cutFirstWord(metcmd2[1],metarg2);

						// If there is another keyword after the last arg
						if(strlen(metarg2[1]) > 0)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						int tmp_metric_type_val = atoi(metarg2[0]);

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
		ospf_redistrib_default = 1;
		ospf_redistrib_default_metric = metric_val;
		ospf_redistrib_default_type = metric_type_val;
	}
	else
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	WRITE_RUN();
	WRITE_OSPFD();

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
	char* redistargs[2];
	cutFirstWord(args,redistargs);

	if(strcmp(redistargs[0],"connected") == 0)
	{
		// Cutting args
		char* metricsstr[2];
		cutFirstWord(args,redistargs);

		if(strlen(redistargs[1]) > 0)
		{
			// Cutting args after redist_type
			char* metcmd1[2];
			cutFirstWord(redistargs[1],metcmd1);

			// If its metric & has a value
			if(strlen(metcmd1[1]) > 0 && strcmp(metcmd1[0],"metric") == 0)
			{
				char* metarg1[2];
				cutFirstWord(metcmd1[1],metarg1);

				// Convert the value
				int tmp_metric_val = atoi(metarg1[0]);

				// if value given mismatches saved
				if(tmp_metric_val != ospf_redistrib_conn_metric)
					return cb;

				// If there is args after metric with value
				if(strlen(metarg1[1]) > 0)
				{
					char* metcmd2[2];
					cutFirstWord(metarg1[1],metcmd2);

					// If there is a value and keywork is metric-type
					if(strlen(metcmd2[1]) > 0 && strcmp(metcmd2[0],"metric-type") == 0)
					{
						char* metarg2[2];
						cutFirstWord(metcmd2[1],metarg2);

						// If there is another keyword after the last arg
						if(strlen(metarg2[1]) > 0)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						int tmp_metric_type_val = atoi(metarg2[0]);

						// if value given mismatches saved
						if(tmp_metric_type_val != ospf_redistrib_conn_type)
							return cb;

					}
				} // if no args given, verify if it's default value
				else if(ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
					return cb;
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				return cb;
			}
		} // if no args given, verify if it's default value
		else if(ospf_redistrib_conn_metric != OSPF_DEFAULT_METRIC)
			return cb;

		ospf_redistrib_conn = 0;
		ospf_redistrib_conn_metric = OSPF_DEFAULT_METRIC;
		ospf_redistrib_conn_type = OSPF_DEFAULT_METRIC_TYPE;
	}
	else if(strcmp(redistargs[0],"static") == 0)
	{
		// Cutting args
		char* metricsstr[2];
		cutFirstWord(args,redistargs);

		if(strlen(redistargs[1]) > 0)
		{
			// Cutting args after redist_type
			char* metcmd1[2];
			cutFirstWord(redistargs[1],metcmd1);

			// If its metric & has a value
			if(strlen(metcmd1[1]) > 0 && strcmp(metcmd1[0],"metric") == 0)
			{
				char* metarg1[2];
				cutFirstWord(metcmd1[1],metarg1);

				// Convert the value
				int tmp_metric_val = atoi(metarg1[0]);

				// if value given mismatches saved
				if(tmp_metric_val != ospf_redistrib_static_metric)
					return cb;

				// If there is args after metric with value
				if(strlen(metarg1[1]) > 0)
				{
					char* metcmd2[2];
					cutFirstWord(metarg1[1],metcmd2);

					// If there is a value and keywork is metric-type
					if(strlen(metcmd2[1]) > 0 && strcmp(metcmd2[0],"metric-type") == 0)
					{
						char* metarg2[2];
						cutFirstWord(metcmd2[1],metarg2);

						// If there is another keyword after the last arg
						if(strlen(metarg2[1]) > 0)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						int tmp_metric_type_val = atoi(metarg2[0]);

						// if value given mismatches saved
						if(tmp_metric_type_val != ospf_redistrib_static_type)
							return cb;

					}
				} // if no args given, verify if it's default value
				else if(ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
					return cb;
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				return cb;
			}
		} // if no args given, verify if it's default value
		else if(ospf_redistrib_static_metric != OSPF_DEFAULT_METRIC)
			return cb;

		ospf_redistrib_static = 0;
		ospf_redistrib_static_metric = OSPF_DEFAULT_METRIC;
		ospf_redistrib_static_type = OSPF_DEFAULT_METRIC_TYPE;
	}
	else if(strcmp(redistargs[0],"default") == 0)
	{
		// Cutting args
		char* metricsstr[2];
		cutFirstWord(args,redistargs);

		if(strlen(redistargs[1]) > 0)
		{
			// Cutting args after redist_type
			char* metcmd1[2];
			cutFirstWord(redistargs[1],metcmd1);

			// If its metric & has a value
			if(strlen(metcmd1[1]) > 0 && strcmp(metcmd1[0],"metric") == 0)
			{
				char* metarg1[2];
				cutFirstWord(metcmd1[1],metarg1);

				// Convert the value
				int tmp_metric_val = atoi(metarg1[0]);

				// if value given mismatches saved
				if(tmp_metric_val != ospf_redistrib_default_metric)
					return cb;

				// If there is args after metric with value
				if(strlen(metarg1[1]) > 0)
				{
					char* metcmd2[2];
					cutFirstWord(metarg1[1],metcmd2);

					// If there is a value and keywork is metric-type
					if(strlen(metcmd2[1]) > 0 && strcmp(metcmd2[0],"metric-type") == 0)
					{
						char* metarg2[2];
						cutFirstWord(metcmd2[1],metarg2);

						// If there is another keyword after the last arg
						if(strlen(metarg2[1]) > 0)
						{
							cb.message = CMDROUTER_OSPF_REDIST_ERROR();
							return cb;
						}

						int tmp_metric_type_val = atoi(metarg2[0]);

						// if value given mismatches saved
						if(tmp_metric_type_val != ospf_redistrib_default_type)
							return cb;

					}
				} // if no args given, verify if it's default value
				else if(ospf_redistrib_conn_type != OSPF_DEFAULT_METRIC_TYPE)
					return cb;
			}
			else
			{
				cb.message = CMDROUTER_OSPF_REDIST_ERROR();
				return cb;
			}
		} // if no args given, verify if it's default value
		else if(ospf_redistrib_conn_metric != OSPF_DEFAULT_METRIC)
			return cb;

		ospf_redistrib_default = 0;
		ospf_redistrib_default_metric = OSPF_DEFAULT_METRIC;
		ospf_redistrib_default_type = OSPF_DEFAULT_METRIC_TYPE;
	}
	else
	{
		cb.message = CMDROUTER_RIP_OSPF_REDIST_ERROR();
		return cb;
	}

	WRITE_RUN();
	WRITE_OSPFD();

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

	char* iface[2];
	cutFirstWord(args,iface);

	if(strlen(iface[1]) > 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
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

	char* iface[2];
	cutFirstWord(args,iface);

	if(strlen(iface[1]) > 0)
	{
		cb.message = CMDROUTER_RIP_OSPF_INTERFACE_ERROR();
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

	return cb;
}