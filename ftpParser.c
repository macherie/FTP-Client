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

int getFTPresponse_code(char *response)
{
  int code;
  if ((sscanf(response, "%d", &code)) == 1)
    {
      return code;
    }
  return -1;
}

int parsePASVresponse(char *response, struct sockaddr_in *result)
{
  int ip1, ip2, ip3, ip4, p1, p2;
  char ip_addr[16];
  
  if ((response = strstr(response, "(")) != NULL)
    {
      printf("%s\n", response);
      if ((sscanf(response, "(%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &p1, &p2)) == 6)
	{
	  result->sin_family = AF_INET;
	  result->sin_port = (p1 * 256) + p2;
	  int dot1, dot2, dot3;
	  
	  // construct the IP address
	  sprintf(&ip_addr[0], "%d", ip1);
	  dot1 = strlen(ip_addr);
	  ip_addr[dot1] = '.'; 
	  sprintf(&ip_addr[dot1 + 1], "%d", ip2);
	  dot2 = strlen(ip_addr);
	  ip_addr[dot2] = '.';
	  sprintf(&ip_addr[dot2 + 1], "%d", ip3);
	  dot3 = strlen(ip_addr);
	  ip_addr[dot3] = '.';
	  sprintf(&ip_addr[dot3 + 1], "%d", ip4);

	  inet_pton(AF_INET, ip_addr, &(result->sin_addr));
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
