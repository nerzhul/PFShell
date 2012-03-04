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

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

#include "command.h"

char* readCmd(void)
{
	char buffer[1024] = "";
	char* cmd;
	char tmpchar;
	short offset = 0;

	do
	{
		struct termios t,tfst;

		tcgetattr(0,&t);
		tfst = t;
		t.c_lflag &= ~ECHO;
		t.c_lflag &= ~ICANON;

		tcsetattr(0,TCSANOW,&t);
		fflush(stdout);
		tmpchar=getchar();
		// @TODO: handle up & down
		if(tmpchar == t.c_cc[VERASE])
		{
			if(offset > 0)
			{
				printf("\b \b");
				buffer[offset] = '\0';
				--offset;
			}
		}
		else if(tmpchar == '\t')
		{
			printf("TAB");
			buffer[offset] = '\0';
		}
		else if(tmpchar == '?')
		{
			printf("HELP");
			buffer[offset] = '\0';
		}
		else
		{
			putchar(tmpchar);
			buffer[offset] = tmpchar;
			++offset;
		}
		tcsetattr(0,TCSANOW,&tfst);
		// @ TODO: handle \t
	} while(offset < 1023 && tmpchar != '\n' && tmpchar != '\0');

	cmd = (char*) malloc((offset)*sizeof(char));

	--offset;
	cmd[offset] = '\0';
	--offset;

	while(offset >= 0)
	{
		cmd[offset] = buffer[offset];
		--offset;
	}

	return cmd;
}
