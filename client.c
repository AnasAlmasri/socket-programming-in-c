#include <io.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <winsock2.h>
#define MAXBUFF 256

int main(int argc, char const *argv[])
{
	// declaring sockets and variables
	WSADATA wsa;
	SOCKET sockfd;
	struct sockaddr_in server;
	int buffer_width=0, i, response_width;
	char buffer[MAXBUFF]={'\0'};
	char buffer_uppercase[MAXBUFF]={'\0'};
	char serv_response[MAXBUFF]={'\0'};

	// INITIALIZING WINSOCK
	printf("Initializing Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
		printf("initialization failed\n");
		printf("Error code: %d\n", WSAGetLastError());
		return 1; // indicates unsuccessful termination
	}
	printf("initialized\n");

	// CREATING THE SOCKET
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		printf("Socket creation failed.\n");
		printf("Error code: %d\n", WSAGetLastError());
		return 1; // indicates unsuccessful termination
	}
	printf("Socket created.\n");

	// CONNECTING THE SOCKET
	server.sin_family = AF_INET;
	server.sin_port = htons(8989);
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); // connect the server address to a localhost
	if (connect(sockfd, (struct sockaddr*) &server, sizeof(server)) < 0){ // connect the socket to the server
		printf("Connection failed.\n");
		return 1; // indicates unsuccessful termination
	}
	printf("Socket connected.\n");

	// COMMUNICATION BETWEEN THE CONSOLE, THE CLIENT AND THE SERVER
	while(1){ // loop to simulate a messanger chat
		// prompt user to input
		printf("\nYour message: ");
		fgets(buffer, MAXBUFF, stdin); // used instead of scanf() in order to get the spaces in the input

		// loop to copy the input in uppercase and put it in buffer_uppercase string
		// this is done to ease the comparison in the upcoming if-statement
		for (i=0; i<buffer[i]!='\0'; i++){
			buffer_uppercase[i] = toupper(buffer[i]); // change character to uppercase and put it in the string
			buffer_width++; // increment character counter
		}

		// determine what to do with the user's input
		if (!strncmp(buffer_uppercase, "QUIT\0", 4)){ // if input is "QUIT"
			printf("\nClient terminated\n");
			break; // break out of the loop to terminate the client
		}
		else {
			// send the input (buffer) to the server
			send(sockfd, buffer, buffer_width, 0);
			// receive response from the server
			response_width = recv(sockfd, serv_response, MAXBUFF, 0);
			// loop to show the response on the client's console window
			printf("Server's response: ");
			for (i=1; i<=response_width; i++){
				printf("%c", serv_response[i]);
			}
			printf("\n"); // print a new line (for window coordination purposes)
		}

		// reset variables before continuing to the next iteration of the while loop
		// this is done in order to prevent server's responses from concatination
		buffer_width=0;
		buffer[0]='\0';
		serv_response[0]='\0';
	} // end of the while loop

	// close socket and clean up Winsock
	close(sockfd);
	WSACleanup();

	return 0; // indicates successful termination
} // end of function main()