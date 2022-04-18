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

void changeDir(const char *path)
{
    // Ignorerer tom path
    if (!strcmp(path, ""))
    {
        return;
    }

    int dirVal = chdir(path);

    // Hvis path ikke finnes:
    if (dirVal == -1)
    {
        printf("cd: no such file or directory: %s\n", path);
    }
}

int executeCommand(char *command)
{
    char *newArg;
    char *args[10];
    newArg = strtok(command, " \t");
    int i = 0;
    while (newArg != NULL)
    {
        args[i] = newArg;
        i++;
        newArg = strtok(NULL, " \t");
    }
    args[i] = NULL;
    if (!strcmp(args[0], "cd"))
    {
        changeDir(args[1]);
    }
    else
    {
        execvp(args[0], args);
    }
}

int handleCommand(char *inputBuffer)
{
    char *command;
    char *inputDest;
    char *outputDest;

    size_t outputRedLoc = strcspn(inputBuffer, ">");
    size_t inputRedLoc = strcspn(inputBuffer, "<");

    long hasInputRedirection = strlen(inputBuffer) - inputRedLoc;
    long hasOutputRedirection = strlen(inputBuffer) - outputRedLoc;

    if (hasInputRedirection && hasOutputRedirection)
    {
        if (inputRedLoc < outputRedLoc)
        {
            command = strtok(inputBuffer, "<>");
            inputDest = strtok(NULL, "<>");
            outputDest = strtok(NULL, "<>");
        }
        else
        {
            command = strtok(inputBuffer, "<>");
            outputDest = strtok(NULL, "<>");
            inputDest = strtok(NULL, "<>");
        }
        outputDest++;
        inputDest++;
        inputDest[strlen(inputDest) - 1] = '\0';
        freopen(inputDest, "r", stdin);
        freopen(outputDest, "w", stdout);
    }
    else if (hasInputRedirection)
    {
        command = strtok(inputBuffer, "<>");
        inputDest = strtok(NULL, "<>");
        inputDest++;
        freopen(inputDest, "r", stdin);
    }
    else if (hasOutputRedirection)
    {
        command = strtok(inputBuffer, "<>");
        outputDest = strtok(NULL, "<>");
        outputDest++;
        freopen(outputDest, "w", stdout);
    }
    else
    {
        command = inputBuffer;
    }

    executeCommand(command);

    if (hasInputRedirection)
    {
        fclose(stdin);
    }
    if (hasOutputRedirection)
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
    char dest[2];

    while (1)
    {
        fflush(stdin);
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
            strncpy(dest, inputBuffer, 2);
            if (!strcmp(dest, "cd"))
            {
                executeCommand(inputBuffer);
            }
            else if (fork() == 0)
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