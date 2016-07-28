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
#include "ftpFunctions.h"

const int DATA_SIZE = 256;
  
int push(int data_socket, int command_socket, char *file_name)
{
  printf("Recv all\n");
  int bytes = 0;
  char buf[DATA_SIZE];

  bytes = recv(command_socket, buf, DATA_SIZE - 1, 0);
  buf[bytes] = '\0';
  printf("%s\n", buf);
  FILE *ptr;
  ptr = fopen(file_name, "wb");
  
  while ((bytes = recv(data_socket, buf, DATA_SIZE - 1, 0)) != 0)
    {
        fwrite(buf, sizeof buf, 1, ptr);
    }
  
  return 0;
}

int fetch(int data_socket, int command_socket, char *file_name)
{
  int bytes = 0;
  char buf[DATA_SIZE];

  bytes = recv(command_socket, buf, DATA_SIZE - 1, 0);
  buf[bytes] = '\0';

  if (getFTPresponse_code(buf) != 150)
    {
      printf("Server cannot store that file..\n");
      return -1;
    }

  FILE *ptr;
  if ((ptr = fopen(file_name, "rb")) == NULL)
    {
      perror("Could not open file..\n");
      return -1;
    }

  while ((bytes = fread(buf, 1, sizeof buf, ptr)) == sizeof buf)
    {
      if ((bytes = send(data_socket, buf, sizeof buf, 0)) != sizeof buf)
	{
	  printf("Could not send the file..\n");
	  return -1;
	}
    }

  // send the rest of the binary file
  // if there is less than 255 bytes of data left to read this will send it
  if ((bytes = send(data_socket, buf, bytes, 0)) != bytes)
    {
      printf("Could not send the file..\n");
      return -1;
    }

  if (feof(ptr))
    {
      printf("end of file\n");
    }
  
  if (ferror(ptr))
    {
      printf("file error\n");
    }

  // Server responds with code 226 if it recieved the entire file
  if ((bytes = recv(command_socket, buf, DATA_SIZE - 1, 0)) == -1)
    {
      printf("Error getting response from server..\n");
      return -1;
    }

  buf[bytes] = '\0';
  int response_code;
  if ((response_code = getFTPresponse_code(buf)) == 226)
    {
      printf("Server succesfully stored the file %s\n", file_name);
    }
  else
    {
      printf("Server did not recieve the entire file, something happened..\n");
      return -1;
    }
  
  return 0;
}

int login(int command_socket, char *username, char *password)
{
  char user[260] = "USER ";
  char pass[260] = "PASS ";
  int len = 0;
  
  if (username == NULL)
    {
      strcat(user, "anonymous\n");
    }
  else
    {
      strncat(user, username, 254);
      user[strlen(user)] = '\n'; /* Add newline because the server expects it */
    }

  if (password == NULL)
    {
      strcat(pass, "anonymous@mail.com\n");
    }
  else
    {
      strncat(pass, password, 254);
      pass[strlen(user)] = '\n';
    }

  printf("%s%s\n", user, pass);
  
  char buf[DATA_SIZE];
  int bytes = 0;

  if (send(command_socket, user, strlen(user), 0) == -1)
    {
      perror("send");
      return 0;
    }

  if ((bytes = recv(command_socket, buf, DATA_SIZE - 1, 0)) == -1)
    {
      perror("recv");
      return 0;
    }

  buf[bytes] = '\0';
  printf("%s\n", buf);

  if (send(command_socket, pass, strlen(pass), 0) == -1)
    {
      perror("send");
      return 0;
    }

  if ((bytes = recv(command_socket, buf, DATA_SIZE - 1, 0)) == -1)
    {
      perror("recv");
      return 0;
    }

  buf[bytes] = '\0';
  printf("%s\n", buf);

  return 1;
}

void quit()
{
  exit(0);
}

int pasv_request(int command_socket, int *data_socket)
{
  char message[] = "PASV\n";
  char buf[DATA_SIZE];
  int bytes = 0;
  
  if (send(command_socket, message, strlen(message), 0) == -1)
    {
      return -1;
    }

  if ((bytes = recv(command_socket, buf, DATA_SIZE - 1, 0)) == -1)
    {
      perror("recv");
      return -1;
    }

  buf[bytes] = '\0';

  // The server return the code 227 if it accept the PASV request
  int response_code = getFTPresponse_code(buf);
  if (response_code != 227)
    {
      return -1;
    }

  // Now ready to open the data socket
  struct sockaddr_in data_connection_info; 
  if (parsePASVresponse(buf, &data_connection_info) == -1)
    {
      perror("parse error after PASV command!");
      exit(3);
    }

  if ((*data_socket = socket(AF_INET, SOCK_STREAM, 0)) == 1)
    {
      perror("could not create socket for data..");
      exit(3);
    }

  if ((connect(*data_socket, (struct sockaddr *)&data_connection_info,
	       sizeof data_connection_info)) == -1)
    {
      close(*data_socket);
      perror("could not open data connection..");
      exit(3);
    }

  printf("Data port open on port: %d\n", ntohs(data_connection_info.sin_port));

  return 0;
  
}




