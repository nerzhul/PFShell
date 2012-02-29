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

#define RIP_AUTH_NONE	0
#define RIP_AUTH_TEXT	1
#define RIP_AUTH_MD5	2

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
#define OSPF_DEFAULT_DELAY		1
#define OSPF_DEFAULT_HOLDTIME		5

typedef struct ospf_area ospf_area;

struct ospf_area
{
	uint32_t id;
	char* ifacelist;
	uint8_t stub;
	uint8_t stub_summary;
	ospf_area* next;
	ospf_area* prev;
};

typedef struct ospf_redist_net ospf_redist_net;

struct ospf_redist_net
{
	char* net;
	uint8_t type;
	uint32_t metric;
	ospf_redist_net* next;
	ospf_redist_net* prev;
};

void addOSPFArea(uint32_t id);
void delOSPFArea(uint32_t id);
void setOSPFAreaStub(uint32_t id,uint8_t stub,uint8_t sum);
uint8_t getOSPFAreaStub(uint32_t id);
uint8_t getOSPFAreaStubSummary(uint32_t id);
uint8_t addIfaceToOSPFArea(char* iface, uint32_t id);
void delIfaceFromOSPFArea(char* iface, uint32_t id);
uint8_t addRedistNetForOSPF(char* net,uint8_t type, uint32_t metric);
void delRedistNetForOSPF(char* net);

ospf_area* ospfareas;
ospf_redist_net* ospfredistnets;

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
unsigned short ospf_stub_router; // @ TODO
unsigned int ospf_delay_timer;
unsigned int ospf_holdtime_timer;
char* ospf_router_id;


#endif
