#include <stdio.h>
#include <string.h>
#include "ftpParser.h"

static int checkFTPcommand(char *command);

int getFTPcommand(char *s)
{
  memset(s, 0, 100);
  char inp[100];
  char *running;
  char *command;
  char *data;

  printf("ftp-> ");
  fgets(inp, 100, stdin);
  running = strdup(inp);
  command = strsep(&running, " ");
  data = running;

  if (checkFTPcommand(command) != 1)
    {
      return -1;
    }

  strcat(s, command);
  s[strlen(s)] = ' '; // add space between the commands
  strcat(s, data);
  
  return 0;
}

static int checkFTPcommand(char *command)
{
  int ret = 0;
  for (int i = 0; !ret && i < command_len; i++)
    {
      if (strcmp(commands[i], command) == 0)
	{
     	  ret = 1;
	}
    }
  return ret;
}
