#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	int status;
	pid_t child;
	
	
	//int sysno = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);   obtiene el numero de la syscall llamada (en el padre)
	

	if (argc <= 1) {
		fprintf(stderr, "Uso: %s comando [argumentos ...]\n", argv[0]);
		exit(1);
	}

	/* Fork en dos procesos */
	child = fork();
	if (child == -1) { perror("ERROR fork"); return 1; }
	if (child == 0) {
		if (ptrace(PTRACE_TRACEME, 0 , NULL , NULL)) 
		{
			perror("ERROR childptrace (PTRACE_TRACEME, ... ) "); 
			exit(1);
		}
		/* Solo se ejecuta en el Hijo */
		execvp(argv[1], argv+1);
		/* Si vuelve de exec() hubo un error */
		perror("ERROR child exec(...)"); exit(1);
	} else {
		/* Solo se ejecuta en el Padre */
		while(1) {
			if (wait(&status) < 0) { perror("waitpid"); break; }
			if (WIFEXITED(status)) break; /* Proceso terminado */
			
			int sysno = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
			int rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL); 
			if (rax == -ENOSYS) //chekeo si el proceso se encuentra entrando a una syscall 
			{ 
				if (sysno == SYS_kill) //chequeo si la syscall a ejecutar es kill
				{
					int sig = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL); // ptrace pero traer data (en particular que señal era)
					if (sig == SIGKILL)
					{
						fprintf(stderr, "se ha evitado el envio de la señal SIGKILL(%i)!\n", SIGKILL); //output de cosas
						ptrace(PTRACE_POKEUSER, child, 8*ORIG_RAX, SYS_exit); // Cambiar la syscall por exit(1)
					}
				}
			}
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
		}
		ptrace(PTRACE_DETACH, child, NULL, NULL); //volvemos a darle el control al hijo
	}
	return 0;
}
