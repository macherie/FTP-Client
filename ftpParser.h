const char *commands[] = {"USER", "PASS", "QUIT", "MODE", "TYPE", "RETR", "STOR", "NOOP"};
const int command_len = 8;

int getFTPcommand(char *s);
int chooseFTPcommand(char *command);
