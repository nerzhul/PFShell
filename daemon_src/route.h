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

#ifndef __ROUTE_H__
#define __ROUTE_H__

typedef struct route route;

struct route
{
	char* net;
	char* mask;
	char* gate;

	route* next;
	route* prev;
};

void addRoute(char* ip, char* mask, char* gate);
void delRoute(char* ip, char* mask, char* gate);

void saveSysctl();
void saveRipd();
void saveOspfd();

// Pointer for static routes
route* routes;
// sysctl vars
unsigned short iprouting;
unsigned short mcastrouting;

// For RIP

#define RIP_DEFAULT_COST		1

unsigned short rip_enabled;
unsigned short rip_redistrib_static;
unsigned short rip_redistrib_conn;
unsigned short rip_redistrib_default;
unsigned short rip_split_horizon;
unsigned int rip_update_timer;
unsigned int rip_fail_timer;
unsigned int rip_dead_timer;

// For OSPF

#define OSPF_DEFAULT_METRIC		100
#define OSPF_DEFAULT_METRIC_TYPE	1
#define OSPF_DEFAULT_COST		10

unsigned short ospf_enabled;
unsigned short ospf_redistrib_static;
unsigned short ospf_redistrib_static_type;
unsigned short ospf_redistrib_static_metric;
unsigned short ospf_redistrib_conn;
unsigned short ospf_redistrib_conn_type;
unsigned short ospf_redistrib_conn_metric;
unsigned short ospf_redistrib_default;
unsigned short ospf_redistrib_default_type;
unsigned short ospf_redistrib_default_metric;
unsigned short ospf_stub_router;
unsigned int ospf_delay_timer;
unsigned int ospf_holdtime_timer;
char* ospf_router_id;

#endif
