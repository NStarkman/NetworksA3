#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAXRCVLEN 4096

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <fileName> <IP-address:port> [bufSize]\n", argv[0]);
		exit(1);
	}

	char *fileName = argv[1];
	char *ipPort = argv[2];
	int bufSize = (argc == 4) ? atoi(argv[3]) : MAXRCVLEN;

	char buffer[bufSize + 1]; /* +1 so we can add null terminator */
	int len, mysocket;
	struct sockaddr_in dest; // socket info about the machine we are connecting to
	struct sockaddr_in src; // socket info about our machine

	// Parse IP address and port
	char *ip = strtok(ipPort, ":");
	int port = atoi(strtok(NULL, ":"));

	// Create a socket.
	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&dest, 0, sizeof(dest)); // zero the struct
	memset(&src, 0, sizeof(src)); // zero the struct

	// Initialize the destination socket information
	dest.sin_family = AF_INET; // Use the IPv4 address family
	inet_aton(ip, &dest.sin_addr); // Set destination IP number from command line arg
	dest.sin_port = htons(port); // Set destination port number

	// Connect to the server
	connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));
	socklen_t sLen = sizeof(src);
	getsockname(mysocket, (struct sockaddr *) &src, &sLen);

	printf("Outgoing connection from %s on port %d\n", inet_ntoa(src.sin_addr), ntohs(src.sin_port));
	printf("Outgoing connection to %s on port %d\n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));

	// Send the file name
	int fileNameLen = strlen(fileName) + 1; // Include null terminator
	//send(mysocket, &fileNameLen, sizeof(fileNameLen), 0); // Send the length of the file name
	send(mysocket, fileName, fileNameLen, 0); // Send the file name

	// Open the file
	FILE *file = fopen(fileName, "rb");
	if (file == NULL) {
		perror("Error opening file");
		close(mysocket);
		exit(1);
	}

	// Read and send the file in chunks
	while ((len = fread(buffer, 1, bufSize, file)) > 0) {
		send(mysocket, buffer, len, 0);
	}

	fclose(file);
	close(mysocket);

	printf("File %s sent successfully.\n", fileName);
	return EXIT_SUCCESS;
}