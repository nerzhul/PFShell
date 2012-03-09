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
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include "string_mgmt.h"

void cutFirstWord(char* string,char** result)
{
	char firstWord[1024];
	bzero(firstWord,1024);
	char followWords[1024];
	bzero(followWords,1024);

	int16_t offset = 0;
	int16_t offset2 = 0;
	int16_t first_written = 0;

	while(offset <= (int16_t)strlen(string))
	{
		if(!first_written)
		{
			if(string[offset] == ' ' || string[offset] == '\n')
			{
				first_written = 1;
				firstWord[offset] = '\0';
			}
			else
				firstWord[offset] = string[offset];
		}
		else
		{
			followWords[offset2] = string[offset];
			++offset2;
		}

		++offset;
	}

	if(first_written)
	{
		followWords[offset] = '\0';

		result[0] = (char*) malloc(offset*sizeof(char));
		result[1] = (char*) malloc(offset2*sizeof(char));

		offset = strlen(firstWord);
		while(offset >= 0)
		{
			result[0][offset] = firstWord[offset];
			--offset;
		}

		offset = strlen(followWords);
		while(offset >= 0)
		{
			result[1][offset] = followWords[offset];
			--offset;
		}
	}
	else
	{
		result[0] = string;
		result[1] = "";
	}
}

uint8_t cutString(char* string,char** result,uint8_t maxexpectedwords)
{
	uint8_t space_count = 0;
	uint32_t offset = 0;

	// Count spaces
	while(offset <= strlen(string))
	{
		if(string[offset] == ' ' || string[offset] == '\n' || string[offset] == '\t')
			++space_count;

		++offset;
	}

	if(space_count >= maxexpectedwords)
		return -1;

	offset = 0;
	uint8_t word_nb = 0;

	char buffer[512];
	bzero(buffer,512);
	uint16_t buffer_offset = 0;

	while(offset <= strlen(string))
	{
		if(string[offset] == ' ' || string[offset] == '\n' || string[offset] == '\t' || string[offset] == '\0')
		{
			*(result+word_nb) = (char*)malloc(buffer_offset*sizeof(char));
			*(result+word_nb) = strdup(buffer);
			++word_nb;
			bzero(buffer,512);
			buffer_offset = 0;
		}
		else
		{
			buffer[buffer_offset] = string[offset];
			++buffer_offset;
		}
		++offset;
	}
	if(strlen(buffer) > 0)
	{
		*(result+word_nb) = (char*)malloc(buffer_offset*sizeof(char));
		*(result+word_nb) = strdup(buffer);
		++word_nb;
	}

	return word_nb;
}

void freeCutString(char** elem,uint8_t size)
{
	if(elem == NULL)
		return;

	uint8_t i;
	for(i=0;i<size;i++)
	{
		if(elem[i] != NULL)
			free(elem[i]);
	}
}


void cutByChar(char* string,char** result,char cutter)
{
	char firstWord[1024];
	char followWords[1024];
	bzero(firstWord,1024);
	bzero(followWords,1024);

	int16_t offset = 0;
	int16_t offset2 = 0;
	int16_t first_written = 0;

	while(offset <= (int16_t)strlen(string))
	{
		if(!first_written)
		{
			if(string[offset] == cutter)
			{
				first_written = 1;
				firstWord[offset] = '\0';
			}
			else
				firstWord[offset] = string[offset];
		}
		else
		{
			followWords[offset2] = string[offset];
			++offset2;
		}

		++offset;
	}

	if(first_written)
	{
		followWords[offset] = '\0';

		result[0] = (char*) malloc(offset*sizeof(char));
		result[1] = (char*) malloc(offset2*sizeof(char));

		offset = strlen(firstWord);
		while(offset >= 0)
		{
			result[0][offset] = firstWord[offset];
			--offset;
		}

		offset = strlen(followWords);
		while(offset >= 0)
		{
			result[1][offset] = followWords[offset];
			--offset;
		}
	}
	else
	{
		result[0] = string;
		result[1] = "";
	}
}

unsigned short regexp(char* str, char* pattern)
{
	int err,match;
	regex_t preg;
	err = regcomp(&preg, pattern, REG_EXTENDED);
	if(err == 0)
	{
		match = regexec(&preg, str, 0, NULL, 0);
		regfree (&preg);
		if (match == 0) return 0;
	}
	return 1;
}

uint8_t is_numeric(char* str)
{
	if(regexp(str,"^[0-9]+$") == 0)	return 0;
	return 1;
}