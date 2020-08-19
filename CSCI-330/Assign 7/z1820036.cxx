/*
 * z1820036.cxx
 * Programmer: Tristen Anderson
 * Purpose: A simple UNIX command interpretter
 *
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string.h>

using namespace std;

int main()
{
	//declare variables to be used 
	char cmd[80];
	char* argv[] = {(char*)0, (char*)0, (char*)0, (char*)0, (char*)0, (char*)0, (char*)0};
	int pid, rs, status;

	//make new processes until the command entered is exit
	while (1) {
	
	//Get command before creating a fork such that if the cmd is exit we can kill the program
	cout << "Enter a command to run: ";
	cin.getline(cmd, 80);
	if (strcmp(cmd, "exit") == 0)	//check for the exit cmd
		exit(0);	
	
	pid = fork();			//if not the exit keyword create a child process to exec the cmd
	if (pid == -1) 			//if fork fails print error
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (pid == 0)
	{	//Childe Process: executes the command afte parsing the cmd string

		//pasre the command line string into tokens
		int index = 0;
		for (char* tok = strtok(cmd, " "); tok; tok = strtok(NULL, " "))
		{
		       	argv[index] = tok;
			index++;
			if (index > 5){
				cerr << "Too many arguments: Enter at most 4 arguments" << endl;
				exit(EXIT_FAILURE);}
		}

		//execute the command 
		rs = execvp( argv[0], argv);
		if (rs == -1)
		{
			perror(argv[0]);
		       	exit(EXIT_FAILURE);
		}

	}

	else 
	{
		//parent process: wait for the child to finish
		wait(&status); 		
	}
	}
	
	return 0;
}
