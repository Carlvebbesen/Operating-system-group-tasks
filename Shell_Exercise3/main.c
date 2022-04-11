#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

char **splitPath(char *path)
{
    char *param;
    char *arguments[10];
    int i = 0;
    char delemit[] = " \t";
    param = strtok(path, delemit);
    if (param != NULL)
    {
        arguments[i] = param;
        i++;
    }
    else
    {
        arguments[i] = *path;
        return arguments;
    }
    while (param != NULL)
    {
        param = strtok(NULL, delemit);
        if (param != NULL)
        {
            arguments[i] = param;
            i++;
        }
    }
    return arguments;
}

int main()
{
    char *enteredText;
    char cwd[PATH_MAX];
    char command[100];
    char *arguments[10];
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(enteredText, 25);
            printf("%s: ", cwd);
            scanf("%25s", enteredText);
            printf("Your entered text was: %s\n", enteredText);
            strcpy(command, splitPath(enteredText));
            printf("Your command was: %s \n", command);
        }
        else
        {
            perror("getcwd() error");
            return 0;
        }
    }

    return 0;
}
