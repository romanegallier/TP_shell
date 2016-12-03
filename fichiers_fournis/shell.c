#include <stdio.h>
#include "readcmd.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	while (1) {
		struct cmdline *l;
		int i, j;

		printf("shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n, je ne traite pas encore ce cas\n", l->in);
		if (l->out) printf("out: %s\n, je ne traite pas encore ce cas\n", l->out);

		/* Display each command of the pipe */ 
		//ne sert a rien ici
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("%s ", cmd[j]);
			}
			printf("\n");
		}
		int taille=0;
		while (l->seq[taille]!= NULL){
			taille ++;
		}
		printf ("taille :%i\n", taille);
		//cas d'une commande simple sans pipe:
		if (taille ==1){
			int pid,status;
			switch( pid= fork()){
				case -1: printf ("Probleme !!!!!\n"); exit (-1);
				case 0 : {
				
					if (l->in) {
						int f= open (l->in,O_RDONLY,S_IRUSR|S_IWUSR);
						dup2(f, STDIN_FILENO);
						close(f);
					}
					if (l->out){
						
						int f= open (l->out,O_WRONLY | O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);
					
						dup2(f, STDOUT_FILENO);
						close(f);
					}
					execvp(l->seq[0][0],l->seq[0]);
				}	 
				default : 
					if (waitpid(pid,&status,0)==-1){printf("Probleme !!!!\n");exit(-1);}
					
			}
		}
		
	}
}

 

