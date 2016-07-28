int getFTPcommand(char *s);
int getFTPresponse_code(char *response);
int parsePASVresponse(char *response, struct sockaddr_in *result);

