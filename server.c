#include <io.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define MY_PORT		8989
#define MAXBUF		256

int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET sockfd , clientfd;
    struct sockaddr_in self;
	char buffer[MAXBUF], buffer_rev[MAXBUF];
	int i, j, char_count;

    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
     
    printf("Initialised.\n");

	/*---create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket");
		exit(errno);
	}

        printf("Socket created.\n");

	/*---initialize address/port structure---*/
	/* bzero(&self, sizeof(self));*/
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("socket--bind");
		exit(errno);
	}

        puts("Bind done");

	/*---make it a "listening socket"---*/
	if ( listen(sockfd, 20) != 0 )
	{
		perror("socket--listen");
		exit(errno);
	}
        
        puts("Waiting for incoming connections...");

	/*---forever... ---*/
	while (1)
	{	struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);

		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		
		// loop while the received message is valid and error-free
		while ((char_count = recv(clientfd, buffer, MAXBUF, 0))!=SOCKET_ERROR)
		{
			// output client's connection information if the message is not an 'Enter' button click
			if (buffer[0]!='\r'){
				// output headers
				printf("\n------------------------------------\n");
				printf("\nClient's connection information:\n");
				printf("IP address\tPort number\n");
				// change IP to readable string and type-cast port into an integer then output them
				printf("%s\t%d\n", inet_ntoa(client_addr.sin_addr), (int) ntohs(client_addr.sin_port));
				// output the buffer character counter
				printf("\nLength of the message: %d characters\n", char_count);
			}
			
			// loop to reverse the message and change the characters to uppercase
			for (i=char_count-1, j=0; i>=0; i--)
			{
				// change the character to uppercase then put it in the reversed buffer string
				buffer_rev[j++] = toupper(buffer[i]);
			}

			// send the reversed message (buffer_rev) to the client
			send(clientfd, buffer_rev, char_count, 0);

			// close the connection
			close(clientfd);
		}
	}

	/*---clean up (should never get here!)---*/
	close(sockfd);
    WSACleanup();

	return 0;
}
