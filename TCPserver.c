// 50570
// gcc -Wall -std=c99 TCPclient.c -o sendFile
// gcc -Wall -std=c99 TCPserver.c -o server

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define DEFAULT_BUFSIZE 4096
#define MAX_FILENAME_LEN 256

int mysocket;            // socket used to listen for incoming connections
int consocket;

static void sigintCatcher(int signal,  siginfo_t* si, void *arg)
{
	printf("\n\n************** Caught SIG_INT: shutting down the server ********************\n");

	close(consocket);
	close(mysocket);
	exit(0);
}

int file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

// Generate a unique filename if it already exists
void get_unique_filename(char *filename) {
    if (!file_exists(filename)) return;

    char unique_filename[MAX_FILENAME_LEN];
    int counter = 1;
    char *dot = strrchr(filename, '.');

    do {
        if (dot) {
            snprintf(unique_filename, MAX_FILENAME_LEN, "%.*s(%d)%s",
                     (int)(dot - filename), filename, counter++, dot);
        } else {
            snprintf(unique_filename, MAX_FILENAME_LEN, "%s(%d)", filename, counter++);
        }
    } while (file_exists(unique_filename));

    strncpy(filename, unique_filename, MAX_FILENAME_LEN);
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <port-number> [bufSize]\n", argv[0]);
		exit(1);
	}

	int port = atoi(argv[1]);
	int bufSize = (argc == 3) ? atoi(argv[2]) : DEFAULT_BUFSIZE;

	char buffer[bufSize + 1]; // +1 so we can add null terminator
	int len;
	struct sockaddr_in dest; // socket info about the machine connecting to us
	struct sockaddr_in serv; // socket info about our server

	// set up the sigint handler
	struct sigaction signaler;
	memset(&signaler, 0, sizeof(struct sigaction));
	sigemptyset(&signaler.sa_mask);
	signaler.sa_sigaction = sigintCatcher;
	signaler.sa_flags = SA_SIGINFO;
	sigaction(SIGINT, &signaler, NULL);

	// Set up socket info
	socklen_t socksize = sizeof(struct sockaddr_in);
	memset(&serv, 0, sizeof(serv));           // zero the struct before filling the fields
	serv.sin_family = AF_INET;                // Use the IPv4 address family
	serv.sin_addr.s_addr = htonl(INADDR_ANY); // Set our address to any interface 
	serv.sin_port = htons(port);              // Set the server port number 

	// Create a socket.
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mysocket == -1)
		printf("Issue with socket | errno = %d\n", errno);

	// bind serv information to mysocket
	int flag = 1;
	if (setsockopt(mysocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
		printf("setsockopt() failed\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}

	if (bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) != 0) {
		printf("Unable to open TCP socket on localhost:%d\n", port);
		printf("%s\n", strerror(errno));
		close(mysocket);
		return 0;
	}

	// start listening, allowing a queue of up to 1 pending connection
	int listened = listen(mysocket, 1);
	if (listened == -1)
		printf("Issue listening | errno = %d\n", errno);

	while (1) {
		// Create a socket to communicate with the client that just connected
		consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
		if (consocket < 0) {
			perror("accept");
			continue;
		}

		printf("Incoming connection from %s on port %d\n", inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));

		// Receive the file name
		len = recv(consocket, buffer, bufSize, 0);
		if (len <= 0) {
			perror("recv");
			close(consocket);
			continue;
		}
		buffer[len] = '\0';
		char* fileName = strdup(buffer);

		get_unique_filename(fileName);

		printf("fileName = %s\n", fileName);

		// Open the file for writing
		FILE* file = fopen(fileName, "wb"); // filename --> output
		if (file == NULL) {
			perror("Error opening file");
			close(consocket);
			free(fileName);
			continue;
		}

		// Receive the file contents
		int totalBytesReceived = 0;
		while ((len = recv(consocket, buffer, bufSize, 0)) > 0) {
			fwrite(buffer, 1, len, file);
			totalBytesReceived += len;
		}

		fclose(file);
		close(consocket);

		printf("Total size: %d bytes\n", totalBytesReceived);
		printf("Received from: %s\n", inet_ntoa(dest.sin_addr));
		printf("Buffer size: %d bytes\n", bufSize);

		free(fileName);
	}

	close(mysocket);
	return EXIT_SUCCESS;
}