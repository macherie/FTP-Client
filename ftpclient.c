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
#include <argp.h>
#include "ftpParser.h"
#include "ftpFunctions.h"

#define FTP_PORT "21" // port used for ftp connection
#define MAXDATASIZE 256

void *get_in_addr(struct sockaddr *sa);
int connect_list(struct addrinfo *server_info, int *sock);
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/* Version and email used fby argp */
const char *FTP_client_version = "FTP client 0.2";
const char *FTP_client_bug_address = "patricjgustafsson@outlook.com";

/* Program documentation */
static char doc[] = "FTP client - A client for use with FTP.";

/* We accept these arguments */
static char args_doc[] = "ARG1";

/* All the options that the client provides */
static struct argp_option options[] = {
  {"anonymous", 'a', 0, 0, "Login as anonymous" },
  { 0 }
};

/* Used by main to communicate with parse_opt */
struct arguments
{
  char *args[1];
  int anonymous;
};

/* Argp parser */
static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv)
{
  int command_socket, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *server_info;
  int rv;
  struct arguments arguments;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  /* Default values for command line arguments */
  arguments.anonymous = 0;
  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  /* Try to get info from server */
  if ((rv = getaddrinfo(arguments.args[0], FTP_PORT, &hints, &server_info)) != 0)
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }

  /* Go trough the linked list of nodes and connect to first one that is available */
  if (connect_list(server_info, &command_socket) != 0)
    {
      fprintf(stderr, "client: failed to connect\n");
      return 1;
    }

  if ((numbytes = recv(command_socket, buf, MAXDATASIZE - 1, 0)) == -1)
    {
      perror("recv");
      exit(1);
    }

  buf[numbytes] = '\0';
  printf("client recieved %s \n", buf);

  /* Do anonymous login if argument is there, otherwise prompt user for username and password */
  if (arguments.anonymous)
    {
      if (!login(command_socket, NULL, NULL))
	{
	  printf("Anonymous login failed.\n");
	  exit(1);
	}
    }
  else
    {
      /* Get the password and username
       The username and password have a arbitrarily chosen length of 256*/
      char username[256];
      char password[256];

      printf("username: ");
      fgets(username, 255, stdin);
      printf("password: ");
      fgets(password, 255, stdin);

      if (!login(command_socket, username, password))
	{
	  printf("Login failed.\n");
	  exit(1);
	}
    }
  
  char command[100];
  int data_socket;
  char response_buffer[100];
  // display prompt for ftp commands
  for (;;)
    {
      getFTPcommand(command);
      int cmd_len = strlen(command);

      if (cmd_len < 3)
	{
	  printf("Command too short!");
	  continue;
	}
      else if (strncmp("login", command, 5) == 0)
	{
	  login(command_socket, NULL, NULL);
	}
      else if (strncmp("fetch", command, 5) == 0)
	{
	  char file_name[95];
	  int file_name_given = (sscanf(command, "%*s %93s", file_name) == 1);

	  if (!file_name_given)
	    {
	      printf("fetch requires a parameter!\n");
	      continue;
	    }
	  
	  int PASV_request_accepted = pasv_request(command_socket, response_buffer, sizeof response_buffer);
	  int data_port_opened = open_data_port(&data_socket, response_buffer);
	  
	  if (PASV_request_accepted && data_port_opened)
	    {
	      fetch(data_socket, command_socket, file_name, strlen(file_name));
	      // TODO, check for 226 as respone from server
	    }
	}
      else if (strncmp("cd", command, 2) == 0)
	{
	  char path[95];
	  int directory_given = (sscanf(command, "%*s %93s", path) == 1);

	  if (!directory_given)
	    {
	      printf("cd requires a parameter!\n");
	      continue;
	    }
	  
	  change_directory(command_socket, path);
	}
    }

  close(command_socket);

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

int connect_list(struct addrinfo *server_info, int *sock)
{
  struct addrinfo *p;
  char s[INET6_ADDRSTRLEN];
  
  // loop trough all resulst and connect to the first one that we can
  for (p = server_info; p != NULL; p = p->ai_next)
    {
      if ((*sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
	{
	  perror("client: socket");
	  continue;
	}

      if (connect(*sock, p->ai_addr, p->ai_addrlen) == -1)
	{
	  close(*sock);
	  perror("client: connect");
	  continue;
	}

      break;
    }

  if (p == NULL)
    {
      return 2;
    }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
  printf("ftpclient: connecting to %s\n", s);
  freeaddrinfo(server_info);
  return 0;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'a':
      arguments->anonymous = 1;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
	argp_usage(state);

      arguments->args[state->arg_num] = arg;
      break;

    case ARGP_KEY_END:
      if (state->arg_num < 1)
	argp_usage(state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}


