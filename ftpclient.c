#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ftpParser.h"

#define FTP_PORT "21" // port used for ftp connection
#define MAXDATASIZE 256

void *get_in_addr(struct sockaddr *sa);

int main(int argc, char *argv[])
{
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  if (argc != 2)
    {
      fprintf(stderr, "usage: ftpclient hostname\n");
      exit(1);
    }

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], FTP_PORT, &hints, &servinfo)) != 0)
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }

  // loop trough all resulst and connect to the first one that we can
  for (p = servinfo; p != NULL; p = p->ai_next)
    {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
	{
	  perror("client: socket");
	  continue;
	}

      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
	{
	  close(sockfd);
	  perror("client: connect");
	  continue;
	}

      break;
    }

  if (p == NULL)
    {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
    }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
  printf("ftpclient: connecting to %s\n", s);

  freeaddrinfo(servinfo);

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
    {
      perror("recv");
      exit(1);
    }

  buf[numbytes] = '\0';
  printf("client recieved %s \n", buf);


  char command[100];
  // display prompt for ftp commands
  for (;;)
    {
      if (getFTPcommand(command) == -1)
	{
	  printf("Error in command!\n");
	  continue;
	}

      if (send(sockfd, command, strlen(command), 0) == -1)
	{
	  perror("send");
	}

      if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
	{
	  perror("recv");
	  exit(1);
	}
	
      buf[numbytes] = '\0';
      
      printf("%s \n", buf);
    }

  
  close(sockfd);

  return 0;
    
}

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    {
      return &(((struct sockaddr_in*)sa)->sin_addr);
    }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


