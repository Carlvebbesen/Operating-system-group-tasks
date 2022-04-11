#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

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

int main()
{
    char command[25];
    char cwd[PATH_MAX];
    char *enteredText;
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(command, 25);
            printf("%s: ", cwd);
            fgets(command, 25, stdin);

            printf("command: %s", command);
            if (!strcmp(command, "cd")) {
                changeDir("..");
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