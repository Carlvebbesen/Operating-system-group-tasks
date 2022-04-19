#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

// Linked list inspired by lecture slides.

struct processNode
{
    char *command;
    int pid;
    struct processNode *nextNode;
};

struct linkedList
{
    struct processNode *head;
    struct processNode *tail;
};

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
        if (errno)
        {
            printf("Flush: command not found: %s\n", args[0]);
        }
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

void addProcessNode(struct linkedList *processList, int pid, char *cmd)
{
    struct processNode *newProcessNode = malloc(sizeof(struct processNode));
    char *command = malloc(sizeof(cmd));
    strcpy(command, cmd);
    newProcessNode->pid = pid;
    newProcessNode->command = command;
    newProcessNode->nextNode = NULL;
    if (processList->head == NULL)
    {
        processList->head = newProcessNode;
        processList->tail = newProcessNode;
    }
    else
    {
        processList->tail->nextNode = newProcessNode;
        processList->tail = newProcessNode;
    }
}

int main()
{
    char inputBuffer[50];
    int child_pid;
    char cwd[PATH_MAX];
    int status;
    int backgroundProcessStatus;
    char dest[3];
    char jobs[5];
    struct linkedList *processList = malloc(sizeof(struct linkedList));
    int isBackground = 0;
    int index;

    while (1)
    {
        fflush(stdin);
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(inputBuffer, 50);

            struct processNode *nextProcess = processList->head;
            struct processNode *prevProcess = NULL;

            while (nextProcess != NULL)
            {
                pid_t returnPid = waitpid(nextProcess->pid, &backgroundProcessStatus, WNOHANG);
                if (returnPid == nextProcess->pid)
                {
                    printf("Exit status [%s]: %d \n", nextProcess->command, backgroundProcessStatus);
                    if (prevProcess == NULL)
                    {
                        processList->head = nextProcess->nextNode;
                    }
                    else
                    {
                        prevProcess->nextNode = nextProcess->nextNode;
                    }
                    free(nextProcess);
                }
                else{
                    prevProcess = nextProcess;
                }
                nextProcess = nextProcess->nextNode;
            }

            printf("%s: ", cwd);
            fgets(inputBuffer, 50, stdin);

            // Looks for End-of-line character ctrl-d
            if (feof(stdin))
            {
                exit(0);
            }

            inputBuffer[strcspn(inputBuffer, "\n")] = 0;

            isBackground = strcmp(&inputBuffer[strlen(inputBuffer) - 1], "&") == 0;

            if (isBackground)
            {
                inputBuffer[strlen(inputBuffer) - 1] = '\0';
            }

            strncpy(dest, inputBuffer, 2);
            dest[2] = '\0';
            strncpy(jobs, inputBuffer, 4);
            jobs[4] = '\0';
            if (!strcmp(dest, "cd"))
            {
                index = strlen(inputBuffer) - 1;
                while (index > -1)
                {
                    if (inputBuffer[index] == ' ' || inputBuffer[index] == '\t')
                    {
                        index--;
                    }
                    else
                    {
                        break;
                    }
                }
                inputBuffer[index + 1] = '\0';
                if (strlen(dest) != strlen(inputBuffer))
                {
                    executeCommand(inputBuffer);
                }
            }
            else if (!strcmp(jobs, "jobs"))
            {
                struct processNode *runningProcesses = processList->head;

                while (runningProcesses != NULL)
                {
                    printf("Pid: %d, Command: %s \n", runningProcesses->pid, runningProcesses->command);
                    runningProcesses = runningProcesses->nextNode;
                }
            }
            else
            {
                child_pid = fork();
                if (child_pid == 0)
                {
                    handleCommand(inputBuffer);
                }
                else
                {
                    if (isBackground)
                    {
                        addProcessNode(processList, child_pid, inputBuffer);
                    }
                    else
                    {
                        waitpid(child_pid, &status, 0);
                        printf("Exit status [%s] = %d \n", inputBuffer, status);
                    }
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