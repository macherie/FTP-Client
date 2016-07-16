#include <stdio.h>
#include <string.h>
#include "ftpParser.h"

int user(int sockfd, char *parameter)
{
  printf("User");
}

int pass(int sockfd, char *parameter)
{
  printf("pass");
}
