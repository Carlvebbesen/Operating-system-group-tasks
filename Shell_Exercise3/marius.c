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

void changeDir(const char *path) {
    //Ignorerer tom path
    if (!strcmp(path, "")) {
        return;
    }

    int dirVal = chdir(path);

    //Hvis path ikke finnes:
    if (dirVal == -1) {
        printf("cd: no such file or directory: %s\n", path);
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
    if (!strcmp(args[0], "cd")) {
        changeDir(args[1]);
    } else {
        execvp(args[0], args);
        exit(0);
    }


}



int main()
{
    char command[25];
    char cwd[PATH_MAX];
    char *enteredText;
    int status;
    char dest[2];

    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(command, 25);
            printf("%s: ", cwd);
            fgets(command, 25, stdin);
            if (feof(stdin))
            {
                exit(0);
            }
            command[strcspn(command, "\n")] = 0;
            strncpy(dest, command, 2);
            
            //For å droppe forking om kommandoen er cd
            if (!strcmp(dest, "cd")) {
                executeCommand(command);
            } else {
                if (fork() == 0)
                {
                    executeCommand(command);
                }
                else
                {
                    waitpid(-1, &status, 0);
                    printf("Exit status [%s] = %d \n", command, status);
                }
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