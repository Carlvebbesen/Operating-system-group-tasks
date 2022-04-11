#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

char *getWorkingDir()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        return cwd;
    }
    else
    {
        perror("getcwd() error");
        return "error";
    }
}

int executeCommand(char *inputBuffer)
{
    char *newArg;
    char *args[10];
    newArg = strtok(inputBuffer, " ");
    int i = 0;
    while (newArg != NULL)
    {
        args[i] = newArg;
        i++;
        newArg = strtok(NULL, " ");
    }
    args[i] = NULL;
    execvp(args[0], args);
    exit(0);
}

int main()
{
    char inputBuffer[25];
    char cwd[PATH_MAX];
    int status;
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(inputBuffer, 25);
            printf("%s: ", cwd);
            fgets(inputBuffer, 25, stdin);
            if (feof(stdin))
            {
                exit(0);
            }
            inputBuffer[strcspn(inputBuffer, "\n")] = 0;
            if (fork() == 0)
            {
                executeCommand(inputBuffer);
            }
            else
            {
                waitpid(-1, &status, 0);
                printf("Exit status [%s] = %d \n", inputBuffer, status);
            }
            fflush(stdout);
        }
        else
        {
            perror("getcwd() error");
            return 0;
        }
    }

    return 0;
}