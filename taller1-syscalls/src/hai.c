#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int i = 0;
int sigint = 0;

void HandleSigint() {
	sigint = 1;
}

void HandleSigurg() {
	printf("ya va\n");
	i++;
}


int main(int argc, char* argv[]) {

	int pid = fork();
	
	
	if( pid == 0) {
		// el hijo
		signal(SIGURG, &HandleSigurg);
		
		while(1) {
			if(i==5)
			{
				kill(getpid()-1, SIGINT);
				return 0;
			}
		}		
	} else {
		// el padre
		signal(SIGINT, &HandleSigint);
		
		while(sigint == 0)
		{
			printf("sup!\n");

			kill(pid, SIGURG);
			sleep(1);
		}
		
		wait(NULL);
		
		execvp(argv[1], argv+1);
	}
	
  	// Completar
	return 0;
}
