/*
 * z1820036.cxx
 * 
 * TCP Server
 * 
 * 	Recieves a pathname to a file or directory from the client
 *  Checks if that path is a file or directory
 *  returns contents of directory of file to the client
 * 	      
 * 	command line arguments:
 * 		argv[1] port to listen on
 * 		argv[2] the pathname to a directory that serves as root to all requested files
 *  
 */
 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

void processClientRequest(int, string);

int main(int argc, char *argv[]) {

//check arguments
	if (argc != 3) {
		cerr << "USAGE: TCPServer port root_directory\n";
		exit(EXIT_FAILURE);
	}
	
	string root = argv[2];

	// Create the TCP socket 
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	// create address structures
	struct sockaddr_in server_address;  // structure for address of server
	struct sockaddr_in client_address;  // structure for address of client
	unsigned int addrlen = sizeof(client_address);
	
	// Construct the server sockaddr_in structure 
	memset(&server_address, 0, sizeof(server_address));   /* Clear struct */
	server_address.sin_family = AF_INET;                  /* Internet/IP */
	server_address.sin_addr.s_addr = INADDR_ANY;          /* Any IP address */
	server_address.sin_port = htons(atoi(argv[1]));       /* server port */
	
	// Bind the socket
	if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}	
	
	// listen: make socket passive and set length of queue
	if (listen(sock, 64) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}	
	
	cout << "Server listening on port: " << argv[1] << endl;
	
	//run until cancelled
	while(true) 
	{
	
	//accept connect to client and communicate over a new connection socket (connSock)
	int connSock = accept(sock, (struct sockaddr *) &client_address, &addrlen);
		if (connSock < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		
		// fork
		if (fork()) { 	    // parent process
			close(connSock);
		} else { 			// child process
			processClientRequest(connSock, root);
		}
	}
	close(sock);
	return 0;
		
}
	
void processClientRequest(int connSock, string root) {
	
	int received;
	char path[1024], buffer[1024];
	
	// read a message from the client
	if ((received = read(connSock, path, sizeof(path))) < 0) {
		perror("receive");
		exit(EXIT_FAILURE);
	}
	cout << "Client request: " << path << endl; 		// EXAMPLE path: "GET /fileone.html" 

	// we need to parse the path into a command "GET" or "INFO" and the pathname of directory or file
	
	char * parsedPath[] = {(char*)0, (char*)0, (char*)0, (char*)0};
	int k = 0;
	char* temp;
	string request;
	
	for (char* tok = strtok(path, " "); tok; tok = strtok(NULL, " ")) {
		parsedPath[k] = tok; 
		
		cout << "TOKEN #" << k << ": " << tok << endl;
		
		if (k == 0)
			request = parsedPath[k];		// identify the command
			
		else if (k == 1)
			strcpy(temp, parsedPath[k]);		// identify the pathname and repurpose path
			
		k++;
		}
		
		//chekc that the requested path begins with '/' and does not contain ".."
		if (strstr(temp, "..") != NULL || temp[0] != '/') {
			close(1);
			dup(connSock);
			cout << "Error: a valid pathname may not conatin \"..\" and must begin with /" << endl;
			exit(EXIT_FAILURE);
			}

		strcpy(path, root.c_str());		//create absolute path
		strcat(path, temp);
		cout << "REQUEST: " << request << endl << "PATH: " << temp << endl;
		cout << "ROOT: " << root << endl;

	if (request == "GET") {
	
		// Lets check if the path is a directory
		int rs;
		struct stat buf;
		rs = stat(path ,&buf);
		if (rs == -1){
			perror("stat");
			exit(EXIT_FAILURE);}
			
		if( S_ISDIR(buf.st_mode) ){
		// if this is true then the pathname is a directory
		
			cout << "Successfully Identicied the Directory" << endl;
			
			// open the directory	
			DIR *dirp = opendir(path);
			if (dirp == 0) {
				// tell client that an error occurred
				// duplicate socket descriptor into error output
				close(2);
				dup(connSock);
				perror(path);
				exit(EXIT_SUCCESS);
			}
			
			// read directory entries
			struct dirent *dirEntry;
			while ((dirEntry = readdir(dirp)) != NULL) {
				
				if (dirEntry->d_name[0] != '.' && "..") {
					strcpy(buffer, dirEntry->d_name);
					strcat(buffer, "\n"); 
				}
				else{
					continue;
				}
				if (write(connSock, buffer, strlen(buffer)) < 0) {
					perror("write");
					exit(EXIT_FAILURE);
				}
				cout << "sent: " << buffer;	
			}
			
		closedir(dirp);
		cout << "done with client request\n";
		close(connSock);
		exit(EXIT_SUCCESS);
	}
	
	else if( S_ISREG(buf.st_mode)) { 
		// its a regular file, therefore print its contents
		cout << "Successfully Identified as a File" << endl;
		
		int fd = open(path, O_RDWR);
		if (fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);}
		
		// read file contents to the buffer
		read(fd, buffer, 1024);
		close(fd);
		
		// send the file contents to the client
		if (write(connSock, buffer, strlen(buffer)) < 0) {
			perror("write");
			exit(EXIT_FAILURE);
			}
			
			cout << "Sent: " << buffer << endl;	
			cout << "done with client request\n";
			close(connSock);
			exit(EXIT_SUCCESS);
		}
	}

	else if (request == "INFO")
	{
		//command is INFO, write the current time and date to the client
		time_t current_time;
		char* strTime;

		current_time = time(NULL);		//get current time and date
		strTime = ctime(&current_time);		//convert to cstring

		cout << "Todays date: " << strTime << endl;
		if (write(connSock, strTime, strlen(strTime)) < 0) {
			perror("write");
			exit(EXIT_FAILURE);
			}
		
		cout << "done with client request\n";
		close(connSock);
		exit(EXIT_SUCCESS);	
	}

	else
	{
		cout << "Failed to identify command: " << request << endl;  
		exit(EXIT_FAILURE);
	}

	exit(EXIT_FAILURE);
}



