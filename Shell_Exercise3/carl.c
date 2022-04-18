#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

// heavily inspired by :https://www.hackerearth.com/practice/data-structures/linked-list/singly-linked-list/tutorial/
struct LinkedList
{
    int id;
    char *command;
    struct LinkedList *next;
};

typedef struct LinkedList *node;

node createNode()
{
    node temp;
    temp = (node)malloc(sizeof(struct LinkedList));
    temp->next = NULL;
    return temp;
}

node addNode(node head, int pid, char *cmd)
{
    node temp, p;
    temp = createNode();
    temp->command = cmd;
    temp->id = pid;
    if (head == NULL)
    {
        head = temp;
    }
    else
    {
        p = head;
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = temp;
    }
    return head;
}

int getExitStatusZombie(int p_id)
{
    int stat;
    if (waitpid(p_id, &stat, WNOHANG))
    {
        if (WIFEXITED(stat))
        {
            return WEXITSTATUS(stat);
        }
    }
    return -1;
}

void printAndRemoveZombies(node head)
{
    node currentNode = head;
    node newNode = head->next;
    int status;
    do
    {
        status = getExitStatusZombie(currentNode->id);
        if (status != -1)
        {
            printf("Background Exit cmd: %s, status: %d \n", currentNode->command, status);
            free(currentNode);
        }
        else
        {
            currentNode = newNode;
        }
        newNode = currentNode->next;
    } while (currentNode->next != NULL);
}

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
    int ampersAnd = 1;
    node head = NULL;
    pid_t pid;
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
            ampersAnd = strcmp(strlen(inputBuffer) - 1, "&");
            if (ampersAnd == 0)
            {
                inputBuffer[strlen(inputBuffer) - 1] = '\0';
            }
            if (!strcmp(dest, "cd"))
            {
                executeCommand(inputBuffer);
            }
            else
            {
                pid = fork();
                if (pid == 0)
                {
                    handleCommand(inputBuffer);
                }
                else
                {
                    waitpid(-1, &status, 0);
                    printf("Exit status [%s] = %d \n", inputBuffer, status);
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
if (pid == 0)
{
    executeCommand(inputBuffer);
}
