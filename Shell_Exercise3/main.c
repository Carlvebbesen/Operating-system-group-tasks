#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

char **splitPath(char *path)
{
    char *param;
    char **arguments;
    int i = 0;
    do
    {
        param = strtok(path, " ");
        if (param == NULL)
        {
            param = strtok(path, "\t");
        }
        if (param != NULL)
        {
            arguments[i] = param;
            i++;
        }
    } while (param != NULL);
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
