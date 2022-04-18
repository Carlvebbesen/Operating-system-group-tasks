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

int executeCommand(char *command)
{
    char *newArg;
    char *args[10];

    newArg = strtok(command, " ");
    int i = 0;
    while (newArg != NULL)
    {
        args[i] = newArg;
        i++;
        newArg = strtok(NULL, " ");
    }
    args[i] = NULL;
    execvp(args[0], args);
}

int handleCommand(char *inputBuffer)
{
    char *command;
    char *inputDest;
    char *outputDest;

    //command = strtok(inputBuffer, "<>");
    printf("%ld \n", strcspn(inputBuffer, ">"));
    printf("%ld \n", strcspn(inputBuffer, "&"));
    printf("%ld \n", strcspn(inputBuffer, "<"));

    printf("%s \n", strtok(inputBuffer, "<>"));
    printf("%s \n", strtok(NULL, ">"));
    size_t outputRedLoc = strcspn(inputBuffer, ">");
    size_t inputRedLoc = strcspn(inputBuffer, "<");

    

    if (inputDest != NULL)
    {
        freopen(inputDest, "r", stdin);
    }
    if (outputDest != NULL)
    {
        freopen(outputDest, "w", stdout);
    }

    executeCommand(command);

    if (inputDest != NULL)
    {
        fclose(stdin);
    }
    if (outputDest != NULL)
    {
        fclose(stdout);
    }

    exit(0);
}

int main()
{
    char inputBuffer[50];
    char cwd[PATH_MAX];
    int status;
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(inputBuffer, 50);
            printf("%s: ", cwd);
            fgets(inputBuffer, 50, stdin);

            // Looks for End-of-line character ctrl-d
            if (feof(stdin))
            {
                exit(0);
            }

            inputBuffer[strcspn(inputBuffer, "\n")] = 0;
            if (fork() == 0)
            {
                handleCommand(inputBuffer);
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