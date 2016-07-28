#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> 

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

  strcat(s, command);
  if (data != NULL) /* More arguments to the command */
    {
      s[strlen(s)] = ' '; // add space between the commands
      strcat(s, data);
    }
  
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
      if ((sscanf(response, "(%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &p1, &p2)) == 6)
	{
	  result->sin_family = AF_INET;
	  result->sin_port = htons((p1 % 256) * 256 + (p2 % 256));
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

	  result->sin_addr.s_addr = inet_addr(ip_addr); 
	  return 0;
	}
    }
  else
    {
      return -1;
    }
}
