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

typedef struct LinkedList *node; // Define node as pointer of data type struct LinkedList

node createNode()
{
    node temp;                                      // declare a node
    temp = (node)malloc(sizeof(struct LinkedList)); // allocate memory using malloc()
    temp->next = NULL;                              // make next point to NULL
    return temp;                                    // return the new node
}

node addNode(node head, int pid, char *cmd)
{
    node temp, p;        // declare two nodes temp and p
    temp = createNode(); // createNode will return a new node with data = pid and next pointing to NULL.
    temp->command = cmd; // assign the command to the node
    temp->id = pid;      // add element's pid to data part of node
    if (head == NULL)
    {
        head = temp; // when linked list is empty
    }
    else
    {
        p = head; // assign head to p
        while (p->next != NULL)
        {
            p = p->next; // traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp; // Point the previous last node to the new node created.
    }
    return head;
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

int executeCommand(char *inputBuffer)
{
    char *newArg;
    char *args[10];
    newArg = strtok(inputBuffer, " \t");
    int i = 0;
    while (newArg != NULL)
    {
        args[i] = newArg;
        i++;
        newArg = strtok(NULL, " \t");
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
    int ampersAnd = 1;
    node head = NULL;
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            if (ampersAnd == 0)
            {
                node p;
                p = head;
                while (p != NULL)
                {
                    waitpid(p->id, &status, WNOHANG);
                    if (status == exited)
                    {
                        printf("Exit status [%s] = %d \n", p->command, status);
                    }
                    p = p->next;
                }
            }
            bzero(inputBuffer, 25);
            printf("%s: ", cwd);
            fgets(inputBuffer, 25, stdin);
            if (feof(stdin))
            {
                exit(0);
            }
            inputBuffer[strcspn(inputBuffer, "\n")] = 0;
            printf("Your entered text was: %s\n", inputBuffer);
            ampersAnd = strcmp(inputBuffer[strlen(inputBuffer) - 1], '&');
            if (ampersAnd == 0)
            {
                inputBuffer[strlen(inputBuffer) - 1] = '\0';
            }
            if (fork() == 0)
            {
                addNode(head, getpid(), inputBuffer);
                executeCommand(inputBuffer);
            }
            else if (ampersAnd != 0)
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