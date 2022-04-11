#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

char *splitPath(char *path)
{
    printf("path: %s\n", path);
    char *param;
    const char *arguments[10];
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
        arguments[i] = path;
        return arguments;
    }
    while (param != NULL)
    {
        printf("args: %s\n", &arguments[i]);
        param = strtok(NULL, delemit);
        printf("param %s\n", param);

        if (param != NULL)
        {
            arguments[i] = param;
            i++;
        }
    }
    printf("here");
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
            strcpy(command, *splitPath(enteredText));
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
