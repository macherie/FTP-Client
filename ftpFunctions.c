#include <stdio.h>
#include <string.h>
#include "ftpParser.h"

int recv_all(int data_socket, int command_socket, char *file_name)
{
  printf("Recv all\n");
  int bytes = 0;
  char buf[256];

  bytes = recv(command_socket, buf, 255, 0);
  buf[bytes] = '\0';
  printf("%s\n", buf);
  FILE *ptr;
  ptr = fopen(file_name, "wb");
  
  while ((bytes = recv(data_socket, buf, 255, 0)) != 0)
    {
        fwrite(buf, sizeof buf, 1, ptr);
    }
  
  return 0;
}

