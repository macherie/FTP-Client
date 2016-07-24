#include <stdio.h>
#include <string.h>
#include "ftpParser.h"

#define DATA_SIZE 256

int recv_file(int data_socket, int command_socket, char *file_name)
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

int send_file(int data_socket, int command_socket, char *file_name)
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
