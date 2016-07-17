#include <stdio.h>
#include <string.h>
#include "ftpParser.h"

const char *commands[FTP_COMMANDS_LEN] = {"USER", "PASS", "QUIT", "PASV", "TYPE", "RETR", "STOR", "NOOP"};

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

int parseFTPresponse(char *response)
{
  int code;
  if ((sscanf(response, "%d", &code)) == 1)
    {
      printf("Server response code: %d\n", code);
      if (code == 227) // When server responds with 227 it means that it's entering passive mode
	{
	  parsePASVresponse(response);
	}
      return 0;
    }
  return -1;
}

int parsePASVresponse(char *response)
{
  int ip1, ip2, ip3, ip4, p1, p2;
  if ((response = strstr(response, "(")) != NULL)
    {
      printf("%s\n", response);
      if ((sscanf(response, "(%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &p1, &p2)) == 6)
	{
	  printf("%d %d %d %d %d %d\n", ip1, ip2, ip3, ip4, p1, p2);
	  return 0;
	}
    }
  else
    {
      return -1;
    }
}

static int checkFTPcommand(char *command)
{
  int ret = 0;
  for (int i = 0; !ret && i < FTP_COMMANDS_LEN; i++)
    {
      if (strcmp(commands[i], command) == 0)
	{
     	  ret = 1;
	}
    }
  return ret;
}
