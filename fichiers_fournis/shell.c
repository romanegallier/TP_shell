#include <stdio.h>
#include "readcmd.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


void commande_redirection(int i, int in, int out, struct cmdline * l);

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

//		if (l->in) printf("in: %s\n, je ne traite pas encore ce cas\n", l->in);
//		if (l->out) printf("out: %s\n, je ne traite pas encore ce cas\n", l->out);

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
		
		int p[2];
		int in = STDIN_FILENO, out;
		i=0;
		while(i<taille -1)
		{
			if(taille>1) 
			{
				if(pipe(p)==-1)
				{
					printf("Probleme pipe(p)\n");
					exit(1);
				} 
			}
		
			out = p[1];
			
			commande_redirection(i,in, out, l);
		
			//on ne ferme que la sortie car le fils va écrire sur l'entrée
			close(out);
			
			//Le prochain fils  se servira de p[0] pour lire
			in = p[0];

			//On passe à la commande suivante
			i++;
			
		}

		// pour la dernière commande, la sortie est la console ou le fichier indiqué par la redirection
		commande_redirection(i, in, -1, l);

	}
}


		//cas d'une commande simple sans pipe:
		
	void commande_redirection(int i, int in, int out, struct cmdline * l){
		int pid,status;
		switch( pid= fork()){
			case -1: printf ("Probleme !!!!!\n"); exit (-1);
			case 0 : {
			
				if (l->in) {
					int f= open (l->in,O_RDONLY,S_IRUSR|S_IWUSR);
					dup2(f, STDIN_FILENO);
					close(f);
				}
				else {
					dup2(in, STDIN_FILENO);
					close(in);
				}
				
				if (l->out){
					
					int f= open (l->out,O_WRONLY | O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);
				
					dup2(f, STDOUT_FILENO);
					close(f);
				} else {
					dup2(out,STDOUT_FILENO);
					close(out);
				}
				execvp(l->seq[i][0],l->seq[i]);
			}	 
			default : 
				if (waitpid(pid,&status,0)==-1){printf("Probleme !!!!\n");exit(-1);}
				
	}
}
	



 

