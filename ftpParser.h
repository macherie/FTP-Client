/* Gets a FTP command
   At the moment onyl one argument can be provided to a command */
int getFTPcommand(char *s);

/* Gets the response code from a FTP response */
int getFTPresponse_code(char *response);

/* Parse the PASV response
   This response have the following structure 227 (h1, h2, h3, h4, p1, p2)
   Where h1.h2.h3.h4 is the servers IPV4 address and p1 and p2 is numbers needed for the port.
   The port number is calculated the following way: port = (p1 % 256) * 256 + (p2 % 256)
*/
int parsePASVresponse(char *response, struct sockaddr_in *result);

/* Check the FTP response code
   In general the FTP response codes are handeled in the following way:
   1xy, accepted request, the client should await more responses (multiline response)
   2xy, command successfull, client can proceed with next request
   3xy, command successfull, the server need more information before it can proceed (ex. password)
   4xy, command not succesfull, the client should repeat the same request or do another one
   5xy, command not sucessfull, permanent error, client is advised not to repeat same command

   There are some special codes that need to be parsed separately, these are:
   code 220, server accepted connection (inital connection for command transfering
   code 227, data port can be opened, in response to PASV request
   code 257, PWD command rejected 
   code 421, connection refused, server closes connection and does not accept any more commands
 */
int parse_response_code(char *response);

