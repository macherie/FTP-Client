#include <arpa/inet.h>
#include <stdlib.h>

#define FTP_COMMANDS_LEN 8
extern const char *commands[FTP_COMMANDS_LEN];

int getFTPcommand(char *s);
int getFTPresponse_code(char *response);
int parsePASVresponse(char *response, struct sockaddr_in *result);
