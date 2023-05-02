#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_ARGS 20
#define MAX_ARG_LEN 50

int main() {
    char prompt[] = "minishell-cmd> ";
    char line[MAX_ARGS * MAX_ARG_LEN];
    char *args[MAX_ARGS + 1]; 
    int fd_in, fd_out;
    pid_t pid;
    int status;
    printf("\nBem vindo ao minishell da disciplina CSC-33.\n\n");
    while (1) {
        printf("\n%s", prompt);

        // Lendo a linha de comando digitada pelo usuário
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break; // EOF or error
        }

        // Parseando os argumentos via strtok conforme sugerido
        int num_args = 0;
        char *token = strtok(line, " \t\n");
        while (token != NULL && num_args < MAX_ARGS) {
            args[num_args++] = token;
            token = strtok(NULL, " \t\n");
        }
        args[num_args] = NULL;

        // Linha vazia? 
        if (num_args == 0) {
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            printf("Até o segundo bimestre!\n");
            break;
        }

        // Há redirecionamento da entrada ou saída padrão? 
        int i;
        for (i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], ">") == 0) {
                // Redirecting stdout to file
                if (args[i+1] == NULL) {
                    fprintf(stderr, "Error: missing filename for stdout redirection\n");
                    break;
                }
                fd_out = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out == -1) {
                    perror("open");
                    break;
                }
                args[i] = NULL;
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
                break;
            } else if (strcmp(args[i], "<") == 0) {
                // Redirecting stdin from file
                if (args[i+1] == NULL) {
                    fprintf(stderr, "Error: missing filename for stdin redirection\n");
                    break;
                }
                fd_in = open(args[i+1], O_RDONLY);
                if (fd_in == -1) {
                    perror("open");
                    break;
                }
                args[i] = NULL;
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
                break;
            }
        }
        if (args[i] != NULL) {
            // Redirecting command failed
            continue;
        }

        // Novo processo
        pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        } else if (pid == 0) {
            // Processo filho
            execv(args[0], args);
            perror("execv");
            exit(EXIT_FAILURE);
        } else {
            // Processo Pai
            wait(&status);
        }

        //--------------------------------------
        // Fiquei na dúvida se eu teria que re-redirecionar a stdin e stdout 
        //--------------------------------------


        // // Reset the standard input and output
        // if (dup2(STDIN_FILENO, 0) == -1) {
        //     perror("dup2 stdin error");
        //     continue;
        // }

        // if (dup2(STDOUT_FILENO, 1) == -1) {
        //     perror("dup2 stdout error");
        //     continue;
        // }
    }

    return 0;
}
