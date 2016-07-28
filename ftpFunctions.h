
/* This function sends the USER and PASSWORD command to the server */
int login(int command_socket, char *username, char *password);

/* Fetches a file from the server */
int fetch(int command_socket, int data_socket, char *file_name);

/* Send a file to the server */
int push(int command_socket, int data_socket, char *file_name);

/* Send a PASV request to the server, used for file transfer 
This function will also try and open the data port */
int pasv_request(int command_socket, int *data_socket);

/* Quits the program */
void quit();

