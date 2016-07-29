
/* This function sends the USER and PASSWORD command to the server */
int login(int command_socket, char *username, char *password);

/* Fetches a file from the server */
int fetch(int command_socket, int data_socket, char *file_name, int file_name_length);

/* Send a file to the server */
int push(int command_socket, int data_socket, char *file_name);

/* Send a PASV request to the server, used for file transfer
 server_response is used to store the port number */
int pasv_request(int command_socket, char *server_response, int buffer_size);

/* Tries to open the data socket to the server */
int open_data_port(int *data_socket, char *pasv_response);

/* Quits the program */
void quit();

