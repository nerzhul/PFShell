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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include "iputils.h"
#include "string_mgmt.h"

char* calc_broadcast(char* IP, char* mask)
{
	struct in_addr _IP;
	struct in_addr _mask;
	struct in_addr bcast;
	char* result;

	inet_aton(IP,&_IP);
	inet_aton(mask,&_mask);

	bcast.s_addr = _IP.s_addr | ~(_mask.s_addr);

	return inet_ntoa(bcast);
}

char* calc_network(char* IP,char* mask)
{
	struct in_addr _IP;
	struct in_addr _mask;
	struct in_addr res;
	char* result;

	inet_aton(IP,&_IP);
	inet_aton(mask,&_mask);

	res.s_addr = _IP.s_addr & _mask.s_addr;
	return inet_ntoa(res);
}

short calc_cidr(char* mask)
{
	struct in_addr _mask;
	unsigned long __mask;
	unsigned short cidr = 0;

	inet_aton(mask,&_mask);

	__mask = ntohl(_mask.s_addr);

	for(short i=0;i<32;i++)
	{
		if((__mask & (1L<<i)) == 0)
			cidr++;
	}

	return (32-cidr);
}

char* calc_mask_from_cidr(char* cidr)
{
	struct in_addr _mask;
	unsigned long mask = (0xffffffff >> (32 - atoi(cidr) )) << (32 - atoi(cidr));
	_mask.s_addr = htonl(mask);
	return inet_ntoa(_mask);
}

short unsigned int is_valid_mask(char* mask)
{
	struct in_addr _mask;
	unsigned long __mask;
	unsigned short zerofound = 0;

	inet_aton(mask,&_mask);

	__mask = ntohl(_mask.s_addr);

	for(short i=0;i<32;i++)
	{
		if((__mask & (1L<<i)) != 0 && zerofound == 0)
			zerofound = 1;
		else if(zerofound && (__mask & (1L<<i)) == 0)
			zerofound = 2;
	}

	return (zerofound == 2 ? 1 : 0);
}

unsigned int calc_num_broadcast(unsigned int IP,unsigned int mask)
{
	return (IP | ~mask);
}

unsigned int calc_num_network(unsigned int IP,unsigned int mask)
{
	return (IP & mask);
}

uint8_t is_valid_ip(char* ip)
{
	if(regexp(ip,"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$") == 0)
		return 0;
	return 1;
}

uint8_t is_valid_ip_and_cidr(char* str)
{
	if(regexp(str,"^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])((/([0-9]|[1-2][0-9]|3[0-2]))?)$") == 0)
		return 0;
	return 1;
}