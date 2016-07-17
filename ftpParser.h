#define FTP_COMMANDS_LEN 8
extern const char *commands[FTP_COMMANDS_LEN];

int getFTPcommand(char *s);
int parseFTPresponse(char *response);
int parsePASVresponse(char *response);
