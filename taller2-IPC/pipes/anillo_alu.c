#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{	
	int status, pid, n, s;
	int buffer[1];
	int fildes[2];
	//int p = 0;

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}

    n = atoi(argv[1]);
	buffer[0] = atoi(argv[2]);
	s = atoi(argv[3]);

  	
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], s);


	//abrir el PIPE
	if (pipe(fildes) == -1) 
	{
		perror("pipe");
		exit(1);
	}

	// int anillo_pipes[][2] = malloc(sizeof(int) * 2 * n);
	int anillo_pipes[n][2];
	//guardo mi pid
	pid = getpid();

	int pidHijo = 0;
	int numHijo; 
	int pidInicial;
	for (numHijo = 0; numHijo < n; numHijo++)
	{
		pidHijo = fork();
		if (pidHijo == 0) break;
		if (numHijo == s) pidInicial = pidHijo;
	}


	if (pidHijo == 0)
	{
		//soy un hijo
		//int id_hijo = getpid() - pid;
		if (numHijo == s) // soy el hijo inicial
		{
			//arranco el mensaje
			int p = rand();

			buffer[0]++;
			write(anillo_pipes[numHijo][1], buffer, sizeof(buffer));

			while (buffer[0] < p)
			{
				read(anillo_pipes[PREV(numHijo, n)][0], buffer, sizeof(buffer));
				buffer[0]++;

				write(anillo_pipes[numHijo][1], buffer, sizeof(buffer));
			}
		}
		else // soy un hijo
		{
			//aca cerrar TODOS los irrelevantes
			for (int i = 0; i < n; i++)
			{
				if (i != PREV(numHijo, n)) close(anillo_pipes[i][0]);
				if (i != numHijo) close(anillo_pipes[i][1]);
			}
			
			while (1)
			{
				read(anillo_pipes[PREV(numHijo, n)][0], buffer, sizeof(buffer));

				if (buffer[0] == EOF) break;
				buffer[0]++; //incrementar

				write(anillo_pipes[numHijo][1], buffer, sizeof(buffer));
				
			}
			close(anillo_pipes[PREV(numHijo, n)][0]);
			close(anillo_pipes[numHijo][1]);
			exit(0);
		}
	}
	else
	{
		//soy el padre
		waitpid(pidInicial, NULL, 0);

		read(anillo_pipes[s][0], buffer, sizeof(buffer));
		close(anillo_pipes[s][0]);

		printf("Soy el proceso PADRE, con testigo %d\n", buffer[0]);
		
		exit(0);
	}
    
    /* COMPLETAR */
		// DALE AL SAVE ANTES DE CERRAR, sisi ya lo hice

	free (anillo_pipes);
}
